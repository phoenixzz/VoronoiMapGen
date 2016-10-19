#ifndef __SGP_RAY_HEADER__
#define __SGP_RAY_HEADER__

class Plane;
class AABBox;
class OBBox;

class Ray
{
public:
	Vector3D m_vcOrig;		// ray origin
	Vector3D m_vcDir;		// ray direction

	//---------------------------------------

	Ray(void) { /* nothing to do */ ; }

    inline void Set(Vector3D vcOrig, Vector3D vcDir)
	{ 
		m_vcOrig = vcOrig;   m_vcDir  = vcDir; 
	}
    void DeTransform(const Matrix4x4 &_m);	// transform this ray into matrix space

	// test for intersection with triangle
	// bCull->triangle's back face do not consider
	// t->diatance from ray origion to intersection 
    bool Intersects(const Vector3D &vc0, const Vector3D &vc1,
                    const Vector3D &vc2, bool bCull, float *t);
	// fL->line segment length
    bool Intersects(const Vector3D &vc0, const Vector3D &vc1,
                    const Vector3D &vc2, bool bCull, float fL, float *t);
	// test for intersection with Plane
    bool Intersects(const Plane &plane, bool bCull, float *t, Vector3D *vcHit);       
    bool Intersects(const Plane &plane, bool bCull, float fL, float *t, Vector3D *vcHit);
    // test for intersection with AABB
	bool Intersects(const AABBox &aabb, float *t);
    bool Intersects(const AABBox &aabb, float fL, float *t);
	// test for intersection with OBB
    bool Intersects(const OBBox &obb, float *t);
    bool Intersects(const OBBox &obb, float fL, float *t);
};


#endif