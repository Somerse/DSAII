#ifndef __BOUNDINGBOXCLASS_H_
#define __BOUNDINGBOXCLASS_H_

#include "MyEngine.h"

class BoundingBoxClass
{
	bool m_bInitialized; //Has the elements of this object been initialized before?
	float m_fHeight;//Radius of the collision sphere
	float m_fWidth;//Radius of the collision sphere
	float m_fLength;//Radius of the collision sphere
	vector3 m_v3Centroid;//Center point of the model, the origin in GSpace in which the sphere is going to be located
	String m_sName;//Name of the BoundingSphere (to relate to the instance)

public:
	BoundingBoxClass(void);
	BoundingBoxClass(BoundingBoxClass const& other);
	BoundingBoxClass& operator=(BoundingBoxClass const& other);
	~BoundingBoxClass(void);

	void Swap(BoundingBoxClass& other);
	bool IsInitialized(void);
	float GetHeight(void);
	float GetWidth(void);
	float GetLength(void);
	vector3 GetCentroid(void);
	String GetName(void);
	void GenerateBoundingBox(String a_sInstanceName);
	void AddBoxToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid = false);

private:
	void Release(void);
	void Init(void);
};

#endif

