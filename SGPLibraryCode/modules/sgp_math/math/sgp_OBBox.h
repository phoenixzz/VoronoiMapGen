#ifndef __SGP_OBBOX_HEADER__
#define __SGP_OBBOX_HEADER__

class AABBox;

class OBBox
{
public:
	float      fA0,   fA1,  fA2; // half axis length
	Vector3D   vcA0, vcA1, vcA2; // box axis
	Vector3D   vcCenter;         // centerpoint

	//---------------------------------------

	OBBox(void) : fA0(0), fA1(0), fA2(0) { /* nothing to do */ ; }
	OBBox(const AABBox* pAABBox);

	void DeTransform(const OBBox &obb, const Matrix4x4 &m);

	bool Intersects(const Ray &_Ray, float *t);
	bool Intersects(const Ray &_Ray, float fL, float *t);
	bool Intersects(const OBBox &Obb);
	bool Intersects(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2);

	int  Cull(const Plane *pPlanes, int nNumPlanes);      

private:
	void ObbProj(const OBBox &Obb, const Vector3D &vcV, float *pfMin, float *pfMax);
	void TriProj(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2, 
				 const Vector3D &vcV, float *pfMin, float *pfMax);
};

#endif