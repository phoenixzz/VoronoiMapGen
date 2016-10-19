#ifndef __SGP_AABBOX_HEADER__
#define __SGP_AABBOX_HEADER__

class OBBox;
class Frustum;

class AABBox
{
public:
	Vector3D vcMin, vcMax;							// box extreme points
	Vector3D vcCenter;								// centerpoint

	//---------------------------------------

	AABBox(void) { /* nothing to do */ }
	AABBox(Vector3D vcMin, Vector3D vcMax);

	void Construct(const OBBox *pObb);                  // build from obb
	int  Cull(const Plane *pPlanes, int nNumPlanes);  

	// normals pointing outwards
	void GetPlanes(Plane *pPlanes);

	bool Contains(const Ray &_Ray, float fL);
	bool Intersects(const Ray &_Ray, float *t);
	bool Intersects(const Ray &_Ray, float fL, float *t);
	bool Intersects(const AABBox &aabb);
	bool Intersects(const Vector3D &vc0);

	bool Intersects(const Frustum &_Frustum);


	inline void operator += (const Vector4D &vc)
	{
		if( vcMin.x == 0 && vcMin.y == 0 && vcMin.z == 0 &&
			vcMax.x == 0 && vcMax.y == 0 && vcMax.z == 0 )
		{
			vcMin.Set(vc.x, vc.y, vc.z);
			vcMax.Set(vc.x, vc.y, vc.z);
		}
		else
		{
			vcMin.x = jmin(vc.x, vcMin.x);
			vcMin.y = jmin(vc.y, vcMin.y);
			vcMin.z = jmin(vc.z, vcMin.z);
			vcMax.x = jmax(vc.x, vcMax.x);
			vcMax.y = jmax(vc.y, vcMax.y);
			vcMax.z = jmax(vc.z, vcMax.z);
		}
		vcCenter = (vcMax + vcMin) / 2.0f;
	}

	inline void operator += (const Vector3D &vc)
	{
		if( vcMin.x == 0 && vcMin.y == 0 && vcMin.z == 0 &&
			vcMax.x == 0 && vcMax.y == 0 && vcMax.z == 0 )
		{
			vcMin = vc;
			vcMax = vc;
		}
		else
		{
			vcMin.x = jmin(vc.x, vcMin.x);
			vcMin.y = jmin(vc.y, vcMin.y);
			vcMin.z = jmin(vc.z, vcMin.z);
			vcMax.x = jmax(vc.x, vcMax.x);
			vcMax.y = jmax(vc.y, vcMax.y);
			vcMax.z = jmax(vc.z, vcMax.z);
		}
		vcCenter = (vcMax + vcMin) / 2.0f;
	}

	inline void operator += (const AABBox &aabb)
	{
		if( vcMin.x == 0 && vcMin.y == 0 && vcMin.z == 0 &&
			vcMax.x == 0 && vcMax.y == 0 && vcMax.z == 0 )
		{
			vcMin = aabb.vcMin;
			vcMax = aabb.vcMax;
		}
		else
		{
			vcMin.x = jmin(aabb.vcMin.x, vcMin.x);
			vcMin.y = jmin(aabb.vcMin.y, vcMin.y);
			vcMin.z = jmin(aabb.vcMin.z, vcMin.z);
			vcMax.x = jmax(aabb.vcMax.x, vcMax.x);
			vcMax.y = jmax(aabb.vcMax.y, vcMax.y);
			vcMax.z = jmax(aabb.vcMax.z, vcMax.z);
		}
		vcCenter = (vcMax + vcMin) / 2.0f;
	}

	inline AABBox& operator + (const AABBox &aabb)
	{
		if( vcMin.x == 0 && vcMin.y == 0 && vcMin.z == 0 &&
			vcMax.x == 0 && vcMax.y == 0 && vcMax.z == 0 )
		{
			vcMin = aabb.vcMin;
			vcMax = aabb.vcMax;
		}
		else
		{
			vcMin.x = jmin(aabb.vcMin.x, vcMin.x);
			vcMin.y = jmin(aabb.vcMin.y, vcMin.y);
			vcMin.z = jmin(aabb.vcMin.z, vcMin.z);
			vcMax.x = jmax(aabb.vcMax.x, vcMax.x);
			vcMax.y = jmax(aabb.vcMax.y, vcMax.y);
			vcMax.z = jmax(aabb.vcMax.z, vcMax.z);
		}
		vcCenter = (vcMax + vcMin) / 2.0f;
		return *this;
	}
};

#endif