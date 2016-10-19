
void Plane::Set(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2) 
{
	Vector3D vcEdge1 = v1 - v0;
	Vector3D vcEdge2 = v2 - v0;

	m_vcNormal.Cross(vcEdge1, vcEdge2);
	m_vcNormal.Normalize();
	m_fDistance = - (m_vcNormal * v0);
	m_vcPoint = v0;
}

// Classify point with respect to this plane (front? back? on this plane?)
int Plane::Classify(const Vector3D &vcPoint)
{
	float f = (vcPoint * m_vcNormal) + m_fDistance;
   
	if(f >  0.00001) return SGP_FRONT;
	if(f < -0.00001) return SGP_BACK;
	return SGP_PLANAR;
}


// Classify polygon with respect to this plane
int Plane::Classify(const Polygon &Poly)
{
	int NumFront=0, NumBack=0, NumPlanar=0;
	int nClass;

	// cast away const
	Polygon *pPoly = ((Polygon*)&Poly);

	int NumPoints = pPoly->GetNumPoints();

	// loop through all points
	for(int i=0; i < NumPoints; i++)
	{
		nClass = Classify( pPoly->m_pPoints[i] );
      
		if (nClass == SGP_FRONT)     NumFront++;
		else if (nClass == SGP_BACK) NumBack++;
		else 
		{
			NumFront++;
			NumBack++;
			NumPlanar++;
		}
	} // for
   
	// all points are planar
	if(NumPlanar == NumPoints)
		return SGP_PLANAR;
	// all points are in front of plane
	else if(NumFront == NumPoints)
		return SGP_FRONT;
	// all points are on backside of plane
	else if(NumBack == NumPoints)
		return SGP_BACK;
	// poly is intersecting the plane
	else
		return SGP_CLIPPED;
}

// clips a ray into two segments if it intersects the plane
bool Plane::Clip(const Ray *_pRay, float fL, Ray *pFront, Ray *pBack) 
{
	Vector3D vcHit(0.0f,0.0f,0.0f);
   
	Ray *pRay = (Ray*)_pRay;

	// ray intersects plane at all?
	if( !pRay->Intersects( *this, false, fL, NULL, &vcHit) ) 
		return false;

	int n = Classify( _pRay->m_vcOrig );

	// ray comes from planes backside
	if( n == SGP_BACK )
	{
		if(pBack) 
			pBack->Set(pRay->m_vcOrig, pRay->m_vcDir);
		if(pFront) 
			pFront->Set(vcHit, pRay->m_vcDir);
	}
	// ray comes from planes front side
	else if( n == SGP_FRONT )
	{
		if(pFront) 
			pFront->Set(pRay->m_vcOrig, pRay->m_vcDir);
		if(pBack) 
			pBack->Set(vcHit, pRay->m_vcDir);
	}

	return true;
}

// Intersection of a plane with a triangle. If all vertices of the
// triangle are on the same side of the plane, no intersection occured. 
bool Plane::Intersects(const Vector3D &vc0, const Vector3D &vc1, const Vector3D &vc2)
{
	int n = this->Classify(vc0);

	if( (n == this->Classify(vc1)) && (n == this->Classify(vc2)) )
		return false;
	return true;
}


// Intersection of two planes. If third parameter is given the line
// of intersection will be calculated. (www.magic-software.com)
bool Plane::Intersects(const Plane &plane, Ray *pIntersection) 
{
	Vector3D  vcCross;
	float     fSqrLength;
   
	// if crossproduct of normals 0 than planes parallel
	vcCross.Cross(this->m_vcNormal, plane.m_vcNormal);
	fSqrLength = vcCross.GetLengthSquared();

	if(fSqrLength < 1e-08f) 
		return false;

	// find line of intersection
	if(pIntersection) 
	{
		float fN00 = this->m_vcNormal.GetLengthSquared();
		float fN01 = this->m_vcNormal * plane.m_vcNormal;
		float fN11 = plane.m_vcNormal.GetLengthSquared();
		float fDet = fN00*fN11 - fN01*fN01;

		if(fabs(fDet) < 1e-08f) 
			return false;

		float fInvDet = 1.0f/fDet;
		float fC0 = (fN11*this->m_fDistance - fN01*plane.m_fDistance) * fInvDet;
		float fC1 = (fN00*plane.m_fDistance - fN01*this->m_fDistance) * fInvDet;

		(*pIntersection).m_vcDir  = vcCross;
		(*pIntersection).m_vcOrig = this->m_vcNormal*fC0 + plane.m_vcNormal*fC1;
	}

	return true;
}

// Intersection with AABB. Search for AABB diagonal that is most
// aligned to plane normal. Test its two vertices against plane.
// ("Real-Time Rendering")
bool Plane::Intersects(const AABBox &aabb)
{
	Vector3D Vmin, Vmax;

	// x component
	if (m_vcNormal.x >= 0.0f)
	{
		Vmin.x = aabb.vcMin.x;
		Vmax.x = aabb.vcMax.x;
	}
	else
	{
		Vmin.x = aabb.vcMax.x;
		Vmax.x = aabb.vcMin.x;
	}

	// y component
	if (m_vcNormal.y >= 0.0f)
	{
		Vmin.y = aabb.vcMin.y;
		Vmax.y = aabb.vcMax.y;
	}
	else 
	{
		Vmin.y = aabb.vcMax.y;
		Vmax.y = aabb.vcMin.y;
	}
   
	// z component
	if (m_vcNormal.z >= 0.0f)
	{
		Vmin.z = aabb.vcMin.z;
		Vmax.z = aabb.vcMax.z;
	}
	else 
	{
		Vmin.z = aabb.vcMax.z;
		Vmax.z = aabb.vcMin.z;
	}

	if( ((m_vcNormal * Vmin) + m_fDistance) > 0.0f)
		return false;
   
	if( ((m_vcNormal * Vmax) + m_fDistance) >= 0.0f)
		return true;
  
	return false;
}

// Intersection with OBB. Same as obb culling to frustrum planes.
bool Plane::Intersects(const OBBox &obb)
{
    float fRadius = fabs( obb.fA0 * (m_vcNormal * obb.vcA0) ) 
                  + fabs( obb.fA1 * (m_vcNormal * obb.vcA1) ) 
                  + fabs( obb.fA2 * (m_vcNormal * obb.vcA2) );

    float fDistance = this->Distance(obb.vcCenter);
    return (fDistance <= fRadius);
}

// Transforms the Plane by the matrix
void Plane::Transform(const Matrix4x4& mat)
{
	Vector4D newPoint = Vector4D(m_vcPoint) * mat;
	newPoint.x /= newPoint.w;
	newPoint.y /= newPoint.w;
	newPoint.z /= newPoint.w;

	Matrix4x4 invmat, invTmat;
	invmat.InverseOf(mat);
	invTmat.TransposeOf(invmat);
	m_vcNormal = m_vcNormal * invTmat;

	Set(m_vcNormal, Vector3D(newPoint.x, newPoint.y, newPoint.z));
}

// Normalizes the plane coefficients so that the plane normal has unit length
void Plane::Normalize()
{
	float norm = m_vcNormal.GetLength();	
	
	if( norm != 0 )
		Set( m_vcNormal/norm, m_fDistance/norm );
	else
		Set( Vector3D(0,0,0), Vector3D(0,0,0) );

}