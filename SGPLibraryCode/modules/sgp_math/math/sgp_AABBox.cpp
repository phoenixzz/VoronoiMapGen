
AABBox::AABBox(Vector3D _vcMin, Vector3D _vcMax)
{
	vcMin = _vcMin;
	vcMax = _vcMax;
	vcCenter = (vcMax + vcMin) / 2.0f;
}

// construct from obb
void AABBox::Construct(const OBBox *pObb)
{
	vcMax.Set(0, 0, 0);
	vcMin.Set(0, 0, 0);
	vcCenter.Set(0, 0, 0);

	Vector3D vcA0 = pObb->vcA0 * pObb->fA0;
	Vector3D vcA1 = pObb->vcA1 * pObb->fA1;
	Vector3D vcA2 = pObb->vcA2 * pObb->fA2;

	*this += pObb->vcCenter + vcA0 + vcA1 + vcA2;
	*this += pObb->vcCenter + vcA0 + vcA1 - vcA2;
	*this += pObb->vcCenter + vcA0 - vcA1 + vcA2;
	*this += pObb->vcCenter + vcA0 - vcA1 - vcA2;

	*this += pObb->vcCenter - vcA0 + vcA1 + vcA2;
	*this += pObb->vcCenter - vcA0 + vcA1 - vcA2;
	*this += pObb->vcCenter - vcA0 - vcA1 + vcA2;
	*this += pObb->vcCenter - vcA0 - vcA1 - vcA2;
}

/**
 * Culls AABB to n sided frustrum. Normals pointing outwards.
 * -> IN:  Plane       - array of planes building frustrum
 *         int         - number of planes in array
 *    OUT: SGP_VISIBLE - obb totally inside frustrum
 *         SGP_CLIPPED - obb clipped by frustrum
 *         SGP_CULLED  - obb totally outside frustrum
 */
int AABBox::Cull(const Plane *pPlanes, int nNumPlanes)
{
	Vector3D   _vcMin, _vcMax;
	bool       bIntersects = false;

	// find and test extreme points
	for(int i=0; i<nNumPlanes; i++)
	{
		// x coordinate
		if (pPlanes[i].m_vcNormal.x >= 0.0f)
		{
			_vcMin.x = this->vcMin.x;
			_vcMax.x = this->vcMax.x;
		}
		else
		{
			_vcMin.x = this->vcMax.x;
			_vcMax.x = this->vcMin.x;
		}
		// y coordinate
		if (pPlanes[i].m_vcNormal.y >= 0.0f)
		{
			_vcMin.y = this->vcMin.y;
			_vcMax.y = this->vcMax.y;
		}
		else 
		{
			_vcMin.y = this->vcMax.y;
			_vcMax.y = this->vcMin.y;
		}
		// z coordinate
		if (pPlanes[i].m_vcNormal.z >= 0.0f)
		{
			_vcMin.z = this->vcMin.z;
			_vcMax.z = this->vcMax.z;
		}
		else
		{
			_vcMin.z = this->vcMax.z;
			_vcMax.z = this->vcMin.z;
		}

		if( ((pPlanes[i].m_vcNormal*_vcMin) + pPlanes[i].m_fDistance) > 0.0f)
			return SGP_CULLED;

		if( ((pPlanes[i].m_vcNormal*_vcMax) + pPlanes[i].m_fDistance) >= 0.0f)
			bIntersects = true;
		} // for

	if (bIntersects) 
		return SGP_CLIPPED;
	return SGP_VISIBLE;
}

// test for intersection with aabb, original code by Andrew Woo, 
// from "Geometric Tools...", Morgan Kaufmann Publ., 2002
bool AABBox::Intersects(const Ray &_Ray, float *t)
{

	float t0, t1, tmp;
	float tNear = -999999.9f;
	float tFar  =  999999.9f;
	float epsilon = 0.00001f;


	// first pair of planes
	if(fabs(_Ray.m_vcDir.x) < epsilon)
	{
		if( (_Ray.m_vcOrig.x < vcMin.x) || (_Ray.m_vcOrig.x > vcMax.x) )
			return false;
	}
	t0 = (vcMin.x - _Ray.m_vcOrig.x) / _Ray.m_vcDir.x;
	t1 = (vcMax.x - _Ray.m_vcOrig.x) / _Ray.m_vcDir.x;
	if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
	if (t0 > tNear) tNear = t0;
	if (t1 < tFar)  tFar = t1;
	if (tNear > tFar) return false;
	if (tFar < 0) return false;

	// second pair of planes
	if(fabs(_Ray.m_vcDir.y) < epsilon) 
	{
		if( (_Ray.m_vcOrig.y < vcMin.y) || (_Ray.m_vcOrig.y > vcMax.y) )
			return false;
		}
	t0 = (vcMin.y - _Ray.m_vcOrig.y) / _Ray.m_vcDir.y;
	t1 = (vcMax.y - _Ray.m_vcOrig.y) / _Ray.m_vcDir.y;
	if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
	if (t0 > tNear) tNear = t0;
	if (t1 < tFar)  tFar = t1;
	if (tNear > tFar) return false;
	if (tFar < 0) return false;

	// third pair of planes
	if(fabs(_Ray.m_vcDir.z) < epsilon)
	{
		if( (_Ray.m_vcOrig.z < vcMin.z) || (_Ray.m_vcOrig.z > vcMax.z) )
			return false;
	}
	t0 = (vcMin.z - _Ray.m_vcOrig.z) / _Ray.m_vcDir.z;
	t1 = (vcMax.z - _Ray.m_vcOrig.z) / _Ray.m_vcDir.z;
	if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
	if (t0 > tNear) tNear = t0;
	if (t1 < tFar)  tFar = t1;
	if (tNear > tFar) return false;
	if (tFar < 0) return false;

	if(tNear > 0)
	{ 
		if(t) 
			*t = tNear;
	}
	else 
	{ 
		if(t) 
			*t = tFar;
	}
	return true;
}

// test for intersection with aabb, original code by Andrew Woo, 
// from "Geometric Tools...", Morgan Kaufmann Publ., 2002
bool AABBox::Intersects(const Ray &_Ray, float fL, float *t) 
{

	float t0, t1, tmp, tFinal;
	float tNear = -999999.9f;
	float tFar  =  999999.9f;
	float epsilon = 0.00001f;


	// first pair of planes
	if(fabs(_Ray.m_vcDir.x) < epsilon) 
	{
		if( (_Ray.m_vcOrig.x < vcMin.x) || (_Ray.m_vcOrig.x > vcMax.x) )
			return false;
	}
	t0 = (vcMin.x - _Ray.m_vcOrig.x) / _Ray.m_vcDir.x;
	t1 = (vcMax.x - _Ray.m_vcOrig.x) / _Ray.m_vcDir.x;
	if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
	if (t0 > tNear) tNear = t0;
	if (t1 < tFar)  tFar = t1;
	if (tNear > tFar) return false;
	if (tFar < 0) return false;

	// second pair of planes
	if(fabs(_Ray.m_vcDir.y) < epsilon)
	{
		if( (_Ray.m_vcOrig.y < vcMin.y) || (_Ray.m_vcOrig.y > vcMax.y) )
			return false;
	}
	t0 = (vcMin.y - _Ray.m_vcOrig.y) / _Ray.m_vcDir.y;
	t1 = (vcMax.y - _Ray.m_vcOrig.y) / _Ray.m_vcDir.y;
	if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
	if (t0 > tNear) tNear = t0;
	if (t1 < tFar)  tFar = t1;
	if (tNear > tFar) return false;
	if (tFar < 0) return false;

	// third pair of planes
	if(fabs(_Ray.m_vcDir.z) < epsilon)
	{
		if( (_Ray.m_vcOrig.z < vcMin.z) || (_Ray.m_vcOrig.z > vcMax.z) )
			return false;
	}
	t0 = (vcMin.z - _Ray.m_vcOrig.z) / _Ray.m_vcDir.z;
	t1 = (vcMax.z - _Ray.m_vcOrig.z) / _Ray.m_vcDir.z;
	if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
	if (t0 > tNear) tNear = t0;
	if (t1 < tFar)  tFar = t1;
	if (tNear > tFar) return false;
	if (tFar < 0) return false;


	if(tNear > 0) 
		tFinal = tNear;
	else 
		tFinal = tFar;

	if(tFinal > fL) 
		return false;
	if(t) 
		*t = tFinal;
	return true;
}

// intersection between two aabbs
bool AABBox::Intersects(const AABBox &aabb)
{
	if((vcMin.x > aabb.vcMax.x) || (aabb.vcMin.x > vcMax.x))
		return false;
	if((vcMin.y > aabb.vcMax.y) || (aabb.vcMin.y > vcMax.y))
		return false;
	if((vcMin.z > aabb.vcMax.z) || (aabb.vcMin.z > vcMax.z))
		return false;
	return true;
}

// does aabb contain the given point
bool AABBox::Intersects(const Vector3D &vc) 
{
	if( vc.x > vcMax.x ) return false;
	if( vc.y > vcMax.y ) return false;
	if( vc.z > vcMax.z ) return false;
	if( vc.x < vcMin.x ) return false;
	if( vc.y < vcMin.y ) return false;
	if( vc.z < vcMin.z ) return false;
	return true;
}

bool AABBox::Intersects(const Frustum &_Frustum)
{
	return ( Cull(_Frustum.planes, Frustum::VF_PLANE_COUNT) != SGP_CULLED );
}

// does aabb contain ray
bool AABBox::Contains(const Ray &_Ray, float fL)
{
	Vector3D vcEnd = _Ray.m_vcOrig + (_Ray.m_vcDir*fL);
	return ( Intersects(_Ray.m_vcOrig) && Intersects(vcEnd) );
}

// get the six planes, normals pointing outwards
void AABBox::GetPlanes(Plane *pPlanes)
{
	Vector3D vcN;
   
	if(!pPlanes) 
		return;

	// right side
	vcN.Set(1.0f, 0.0f, 0.0f);
	pPlanes[0].Set(vcN, vcMax);
   
	// left side
	vcN.Set(-1.0f, 0.0f, 0.0f);
	pPlanes[1].Set(vcN, vcMin);

	// front side
	vcN.Set(0.0f, 0.0f, -1.0f);
	pPlanes[2].Set(vcN, vcMin);

	// back side
	vcN.Set(0.0f, 0.0f, 1.0f);
	pPlanes[3].Set(vcN, vcMax);

	// top side
	vcN.Set(0.0f, 1.0f, 0.0f);
	pPlanes[4].Set(vcN, vcMax);

	// bottom side
	vcN.Set(0.0f, -1.0f, 0.0f);
	pPlanes[5].Set(vcN, vcMin);
}