#ifndef __SGP_PLANE_HEADER__
#define __SGP_PLANE_HEADER__

class Polygon;
class Ray;
class AABBox;
class OBBox;

class Plane 
{
public:
	Vector3D			m_vcNormal,		// plane normal vector
						m_vcPoint;		// point on plane
	float				m_fDistance;	// distance to origin

	//---------------------------------------

	Plane(void) : m_fDistance(0) { /* nothing to do */ ; }

	inline void  Set(const Vector3D &vcN, const Vector3D &vcP)
		{ m_fDistance = - ( vcN * vcP); m_vcNormal = vcN; m_vcPoint = vcP; }
	inline void  Set(const Vector3D &vcN, const Vector3D &vcP, float fD)
		{ m_vcNormal = vcN; m_fDistance = fD; m_vcPoint = vcP; }
	inline void  Set(const Vector3D &vcN, float fD)
	{ 
		m_vcNormal = vcN; m_fDistance = fD; 
		if( m_vcNormal.x != 0 )	
			m_vcPoint.Set(-m_fDistance/m_vcNormal.x, 0, 0);
		else if( m_vcNormal.y != 0 ) 
			m_vcPoint.Set(0, -m_fDistance/m_vcNormal.y, 0);
		else if( m_vcNormal.z != 0 ) 
			m_vcPoint.Set(0, 0, -m_fDistance/m_vcNormal.z);
	}

	void  Set(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2);

	inline float Distance(const Vector3D &vcPoint)	
		{ return fabs((m_vcNormal*vcPoint) - m_fDistance); }
	
	int   Classify(const Vector3D &vcPoint);
	int   Classify(const Polygon &Poly);

	// clips a ray into two segments if it intersects the plane
	bool  Clip(const Ray* _pRay, float fL, Ray* pFront, Ray* pBack);

	bool  Intersects(const Vector3D &vc0, const Vector3D &vc1, const Vector3D &vc2);
	// Intersection of two planes?
	bool  Intersects(const Plane &plane, Ray *pIntersection);
	bool  Intersects(const AABBox &aabb);
	bool  Intersects(const OBBox &obb);

	// Transforms the Plane by the matrix
	void Transform(const Matrix4x4& mat);

	// Normalizes the plane coefficients so that the plane normal has unit length
	// This function normalizes a plane so that |a,b,c| == 1
	void Normalize();
};

#endif