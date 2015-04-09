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
	//Generates the 8 corners of the bounding box from the m_v3Max and m_v3Min variables derived in the above GenerateOrientedBoundingBox method 
	std::vector<vector3> vertices = std::vector<vector3>();
	vertices[0] = m_v3Max;
	vertices[1] = vector3(m_v3Max.x, m_v3Max.y, m_v3Min.z);
	vertices[2] = vector3(m_v3Max.x, m_v3Min.y, m_v3Max.z);
	vertices[3] = vector3(m_v3Min.x, m_v3Max.y, m_v3Max.z);
	vertices[4] = vector3(m_v3Min.x, m_v3Min.y, m_v3Max.z);
	vertices[5] = vector3(m_v3Min.x, m_v3Max.y, m_v3Min.z);
	vertices[6] = vector3(m_v3Max.x, m_v3Min.y, m_v3Min.z);
	vertices[7] = m_v3Min;

	//Translates each of the 8 corner vertices into world space
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = (vector3)(a_m4ModeltoWorld * vector4(vertices[i].x, vertices[i].y, vertices[i].z, 1));
	}

	vector3 min, max = vertices[0];
	vector3 centroid, scale;

	//Finds the new min and max vertices in world space
	for (int i = 1; i < vertices.size(); i++)
	{
		if (vertices[i].x > max.x)
		{
			max.x = vertices[i].x;
		}
		else if (vertices[i].x < min.x)
		{
			min.x = vertices[i].x;
		}

		if (vertices[i].y > max.y)
		{
			max.y = vertices[i].y;
		}
		else if (vertices[i].y < min.y)
		{
			min.y = vertices[i].y;
		}

		if (vertices[i].z > max.z)
		{
			max.z = vertices[i].z;
		}
		else if (vertices[i].z < min.z)
		{
			min.z = vertices[i].z;
		}
	}

	//Uses the found min and max vertices to find the model's midpoint
	centroid = vector3 ((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, (min.z + max.z) / 2.0f);

	//Uses the found min and max vertices to determine the model's scale
	scale = vector3(max.x - min.x, max.y - min.y, max.z - min.z);

	//Adds an axis and cube to queue that are both rendered based on the information we just found
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	pMeshMngr->AddAxisToQueue(a_m4ModeltoWorld * glm::translate(centroid));
	pMeshMngr->AddCubeToQueue(a_m4ModeltoWorld * glm::translate(centroid) * glm::scale(scale), vector3(0, 1.0f, 0), MERENDER::WIRE);
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