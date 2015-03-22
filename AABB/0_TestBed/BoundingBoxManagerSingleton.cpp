#include "BoundingBoxManagerSingleton.h"

//  BoundingBoxManagerSingleton
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::m_pInstance = nullptr;
void BoundingBoxManagerSingleton::Init(void)
{
	m_nBoxs = 0;
}
void BoundingBoxManagerSingleton::Release(void)
{
	//Clean the list of Boxs
	for(int n = 0; n < m_nBoxs; n++)
	{
		//Make sure to release the memory of the pointers
		if(m_lBox[n] != nullptr)
		{
			delete m_lBox[n];
			m_lBox[n] = nullptr;
		}
	}
	m_lBox.clear();
	m_lMatrix.clear();
	m_lColor.clear();
	m_nBoxs = 0;
}
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new BoundingBoxManagerSingleton();
	}
	return m_pInstance;
}
void BoundingBoxManagerSingleton::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//The big 3
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(){Init();}
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(BoundingBoxManagerSingleton const& other){ }
BoundingBoxManagerSingleton& BoundingBoxManagerSingleton::operator=(BoundingBoxManagerSingleton const& other) { return *this; }
BoundingBoxManagerSingleton::~BoundingBoxManagerSingleton(){Release();};
//Accessors
int BoundingBoxManagerSingleton::GetBoxTotal(void){ return m_nBoxs; }

//--- Non Standard Singleton Methods
void BoundingBoxManagerSingleton::GenerateBoundingBox(String a_sInstanceName)
{
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	//Verify the instance is loaded
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{//if it is check if the Box has already been created
		if(IdentifyBox(a_sInstanceName) == -1)
		{
			//Create a new bounding Box
			BoundingBoxClass* pBS = new BoundingBoxClass();
			//construct its information out of the instance name
			pBS->GenerateBoundingBox(a_sInstanceName);
			//Push the Box back into the list
			m_lBox.push_back(pBS);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the Box is going to have
			m_lColor.push_back(vector3(1.0f));
			//Increase the number of Boxs
			m_nBoxs++;
		}
	}
}

void BoundingBoxManagerSingleton::SetBoundingBoxSpace(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	int nBox = IdentifyBox(a_sInstanceName);
	//If the Box was found
	if(nBox != -1)
	{
		//Set up the new matrix in the appropriate index
		m_lMatrix[nBox] = a_mModelToWorld;
	}
}

int BoundingBoxManagerSingleton::IdentifyBox(String a_sInstanceName)
{
	//Go one by one for all the Boxs in the list
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//If the current Box is the one we are looking for we return the index
		if(a_sInstanceName == m_lBox[nBox]->GetName())
			return nBox;
	}
	return -1;//couldnt find it return with no index
}

void BoundingBoxManagerSingleton::AddBoxToRenderList(String a_sInstanceName)
{
	//If I need to render all
	if(a_sInstanceName == "ALL")
	{
		for(int nBox = 0; nBox < m_nBoxs; nBox++)
		{
			m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
	else
	{
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox != -1)
		{
			m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
}

void BoundingBoxManagerSingleton::CalculateCollision(void)
{
	//Create a placeholder for all center points
	std::vector<vector3> lCentroid;
	//for all Boxs...
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//Make all the Boxs white
		m_lColor[nBox] = vector3(1.0f);
		//Place all the centroids of Boxs in global space
		lCentroid.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetCentroid(), 1.0f)));
	}

	//now the actual check for all Boxs among all Boxs (so... one by one), this is not the most optimal way, there is a more clever one
	for(int i = 0; i < m_nBoxs; i++)
	{
		for(int j = 0; j < m_nBoxs; j++)
		{
			if(i != j)
			{
				//If the distance between the center of both Boxs is less than the sum of their radius there is a collision
				if(glm::distance(lCentroid[i].x, lCentroid[j].x) < (m_lBox[i]->GetWidth() / 2 + m_lBox[j]->GetWidth() / 2) && glm::distance(lCentroid[i].y, lCentroid[j].y) < (m_lBox[i]->GetHeight() / 2 + m_lBox[j]->GetHeight() / 2) && glm::distance(lCentroid[i].z, lCentroid[j].z) < (m_lBox[i]->GetLength() / 2 + m_lBox[j]->GetLength() / 2))
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
			}
		}
	}
	

	////This way is more optimal, just half the checks are needed
	//for(int i = 0; i < m_nBoxs - 1; i++)
	//{
	//	for(int j = i + 1; j < m_nBoxs; j++)
	//	{
	//		//If the distance between the center of both Boxs is less than the sum of their radius there is a collision
	//		if(glm::distance(lCentroid[i], lCentroid[j]) < (m_lBox[i]->GetRadius() + m_lBox[j]->GetRadius()))
	//			m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
	//	}
	//}
}