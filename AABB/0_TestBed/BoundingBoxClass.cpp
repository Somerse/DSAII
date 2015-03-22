#include "BoundingBoxClass.h"


void BoundingBoxClass::Init(void)
{
	m_bInitialized = false;
	m_fHeight = 0.0f;
	m_fWidth = 0.0f;
	m_fLength = 0.0f;
	m_v3Centroid = vector3(0.0f);
	m_sName = "NULL";
}
void BoundingBoxClass::Swap(BoundingBoxClass& other)
{
	std::swap(m_bInitialized, other.m_bInitialized);
	std::swap(m_fHeight, other.m_fHeight);
	std::swap(m_fWidth, other.m_fWidth);
	std::swap(m_fLength, other.m_fLength);
	std::swap(m_v3Centroid, other.m_v3Centroid);
	std::swap(m_sName, other.m_sName);
}
void BoundingBoxClass::Release(void)
{
	//No pointers to release
}
//The big 3
BoundingBoxClass::BoundingBoxClass(){Init();}
BoundingBoxClass::BoundingBoxClass(BoundingBoxClass const& other)
{
	m_bInitialized = other.m_bInitialized;
	m_fHeight = other.m_fHeight;
	m_fWidth = other.m_fWidth;
	m_fLength = other.m_fLength;
	m_v3Centroid = other.m_v3Centroid;
	m_sName = other.m_sName;
}
BoundingBoxClass& BoundingBoxClass::operator=(BoundingBoxClass const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		BoundingBoxClass temp(other);
		Swap(temp);
	}
	return *this;
}
BoundingBoxClass::~BoundingBoxClass(){Release();};
//Accessors
bool BoundingBoxClass::IsInitialized(void){ return m_bInitialized; }
float BoundingBoxClass::GetHeight(void){ return m_fHeight; }
float BoundingBoxClass::GetWidth(void){ return m_fWidth; }
float BoundingBoxClass::GetLength(void){ return m_fLength; }
vector3 BoundingBoxClass::GetCentroid(void){ return m_v3Centroid; }
String BoundingBoxClass::GetName(void){return m_sName;}
//Methods
void BoundingBoxClass::GenerateBoundingBox(String a_sInstanceName)
{
	//If this has already been initialized there is nothing to do here
	if(m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{
		m_sName = a_sInstanceName;
		
		std::vector<vector3> lVertices = pMeshMngr->GetVertices(m_sName);
		unsigned int nVertices = lVertices.size();
		m_v3Centroid = lVertices[0];
		vector3 v3MaxHeight(lVertices[0]);
		vector3 v3MinHeight(lVertices[0]);
		vector3 v3MaxWidth(lVertices[0]);
		vector3 v3MinWidth(lVertices[0]);
		vector3 v3MaxLength(lVertices[0]);
		vector3 v3MinLength(lVertices[0]);
		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++)
		{
			//m_v3Centroid += lVertices[nVertex];
			if(v3MinWidth.x > lVertices[nVertex].x)
				v3MinWidth.x = lVertices[nVertex].x;
			else if(v3MaxWidth.x < lVertices[nVertex].x)
				v3MaxWidth.x = lVertices[nVertex].x;
			
			if(v3MinHeight.y > lVertices[nVertex].y)
				v3MinHeight.y = lVertices[nVertex].y;
			else if(v3MaxHeight.y < lVertices[nVertex].y)
				v3MaxHeight.y = lVertices[nVertex].y;

			if(v3MinLength.z > lVertices[nVertex].z)
				v3MinLength.z = lVertices[nVertex].z;
			else if(v3MaxLength.z < lVertices[nVertex].z)
				v3MaxLength.z = lVertices[nVertex].z;
		}
		m_v3Centroid = vector3((v3MinWidth.x + v3MaxWidth.x) / 2.0f, (v3MinHeight.y + v3MaxHeight.y) / 2.0f, (v3MinLength.z + v3MaxLength.z) / 2.0f);
		m_fHeight = v3MaxHeight.y - v3MinHeight.y;
		m_fWidth = v3MaxWidth.x - v3MinWidth.x;
		m_fLength = v3MaxLength.z - v3MinLength.z;

		m_bInitialized = true;
	}
}
void BoundingBoxClass::AddBoxToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid)
{
	if(!m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(a_bRenderCentroid)
		pMeshMngr->AddAxisToQueue(a_mModelToWorld * glm::translate(m_v3Centroid));
	pMeshMngr->AddCubeToQueue(a_mModelToWorld * glm::translate(m_v3Centroid) * glm::scale(vector3(m_fWidth, m_fHeight, m_fLength)), a_vColor, MERENDER::WIRE);
}
