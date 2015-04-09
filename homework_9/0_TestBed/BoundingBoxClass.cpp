#include "BoundingBoxClass.h"
//  BoundingBoxClass
void BoundingBoxClass::Init(void)
{
	m_bInitialized = false;
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_v3Centroid = vector3(0.0f);
	m_sName = "NULL";
}
void BoundingBoxClass::Swap(BoundingBoxClass& other)
{
	std::swap(m_bInitialized, other.m_bInitialized);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
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
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
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
vector3 BoundingBoxClass::GetMinimumOBB(void){ return m_v3Min; }
vector3 BoundingBoxClass::GetMaximumOBB(void){ return m_v3Max; }
vector3 BoundingBoxClass::GetCentroid(void){ return m_v3Centroid; }
String BoundingBoxClass::GetName(void){return m_sName;}
//Methods
void BoundingBoxClass::GenerateOrientedBoundingBox(String a_sInstanceName)
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
		m_v3Max = lVertices[0];
		m_v3Min = lVertices[0];
		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++)
		{
			//m_v3Centroid += lVertices[nVertex];
			if(m_v3Min.x > lVertices[nVertex].x)
				m_v3Min.x = lVertices[nVertex].x;
			else if(m_v3Max.x < lVertices[nVertex].x)
				m_v3Max.x = lVertices[nVertex].x;
			
			if(m_v3Min.y > lVertices[nVertex].y)
				m_v3Min.y = lVertices[nVertex].y;
			else if(m_v3Max.y < lVertices[nVertex].y)
				m_v3Max.y = lVertices[nVertex].y;

			if(m_v3Min.z > lVertices[nVertex].z)
				m_v3Min.z = lVertices[nVertex].z;
			else if(m_v3Max.z < lVertices[nVertex].z)
				m_v3Max.z = lVertices[nVertex].z;
		}
		m_v3Centroid = (m_v3Min + m_v3Max) / 2.0f;

		m_v3Size.x = glm::distance(vector3(m_v3Min.x, 0.0f, 0.0f), vector3(m_v3Max.x, 0.0f, 0.0f));
		m_v3Size.y = glm::distance(vector3(0.0f, m_v3Min.y, 0.0f), vector3(0.0f, m_v3Max.y, 0.0f));
		m_v3Size.z = glm::distance(vector3(0.0f, 0.0f, m_v3Min.z), vector3(0.0f, 0.0f, m_v3Max.z));


		m_bInitialized = true;
	}
}
void BoundingBoxClass::GenerateAxisAlignedBoundingBox(matrix4 a_m4ModeltoWorld)
{
	//Generate the Axis Aligned Bounding Box here based on the Oriented Bounding Box

	/*
	std::vector<vector3> lVertices = pMeshMngr->GetVertices(m_sName);
	unsigned int nVertices = lVertices.size();
	m_v3Centroid = lVertices[0];
	m_v3Max = lVertices[0];
	m_v3Min = lVertices[0];*/

	vector3 n_MaxOBB = this->GetMaximumOBB(); 
	vector3 n_MinOBB = this->GetMinimumOBB();
	vector3 n_Centroid = (n_MaxOBB + n_MinOBB) / 2.0f;
	vector3 n_Half = n_MaxOBB - n_Centroid;
	vector3 n_Max;
	vector3 n_Min;


	
	//Sideways Rotation of Bounding Box generates these points:
	//n_max.x, n_max.y
	//n_min.x, n_min.y
	//n_max.x, n_min.y
	//n_min.x, n_max.y

	//matrix4 turned into a matrix3
	//mat3 = (m_lMatrix[index] * vec3);
	glm::mat3 threePoints = glm::mat3(a_m4ModeltoWorld);

	
	//Loop through x, y, and z points
	for(int x = -1; x < 2; x += 2) {
		for(int y = -1; y < 2; y += 2) {
			for(int z = -1; z < 2; z += 2) {
				vector3 newPoint;
				newPoint.x = x * n_Half.x;
				newPoint.y = y * n_Half.y;
				newPoint.z = z * n_Half.z;
				newPoint += n_Centroid;
				newPoint = threePoints * newPoint;
				n_Max = glm::max(n_Max, newPoint);
				n_Min = glm::min(n_Min, newPoint);
			}
		}
	}

	//m_v3Centroid = (m_v3Min + m_v3Max) / 2.0f;

	n_Center = (n_Min + n_Max) / 2.0f;

	
	/*m_v3Size.x = glm::distance(vector3(m_v3Min.x, 0.0f, 0.0f), vector3(m_v3Max.x, 0.0f, 0.0f));
	m_v3Size.y = glm::distance(vector3(0.0f, m_v3Min.y, 0.0f), vector3(0.0f, m_v3Max.y, 0.0f));
	m_v3Size.z = glm::distance(vector3(0.0f, 0.0f, m_v3Min.z), vector3(0.0f, 0.0f, m_v3Max.z));*/

	n_Size.x = glm::distance(vector3(n_Min.x, 0.0f, 0.0f), vector3(n_Max.x, 0.0f, 0.0f));
	n_Size.y = glm::distance(vector3(0.0f, n_Min.y, 0.0f), vector3(0.0f, n_Max.y, 0.0f));
	n_Size.z = glm::distance(vector3(0.0f, 0.0f, n_Min.z), vector3(0.0f, 0.0f, n_Max.z));

	glm::mat4 n_Translation = glm::mat4(1.0f);
	n_Translation[3] = a_m4ModeltoWorld[3];

	/*MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	pMeshMngr->AddAxisToQueue(a_m4ModelToWorld * glm::translate(m_v3Centroid));
	pMeshMngr->AddCubeToQueue(a_m4ModelToWorld * glm::translate(m_v3Centroid) * glm::scale(m_v3Size), a_vColor, MERENDER::WIRE);*/

	
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	pMeshMngr->AddAxisToQueue(n_Translation * glm::translate(n_Center));
	pMeshMngr->AddCubeToQueue(n_Translation * glm::translate(n_Center) * glm::scale(n_Size), n_Color, MERENDER::WIRE);

}

void BoundingBoxClass::AddBoxToRenderList(matrix4 a_m4ModelToWorld, vector3 a_vColor, bool a_bRenderCentroid)
{
	if(!m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(a_bRenderCentroid)
	pMeshMngr->AddAxisToQueue(a_m4ModelToWorld * glm::translate(m_v3Centroid));
	pMeshMngr->AddCubeToQueue(a_m4ModelToWorld * glm::translate(m_v3Centroid) * glm::scale(m_v3Size), a_vColor, MERENDER::WIRE);
}