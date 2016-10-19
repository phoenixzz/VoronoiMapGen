
//! This constructor creates a view frustum based on a projection and/or view matrix.
void Frustum::setFrom(const Matrix4x4& viewprojmat)
{
	// left clipping plane
	planes[VF_LEFT_PLANE].m_vcNormal.x = viewprojmat._14 + viewprojmat._11;
	planes[VF_LEFT_PLANE].m_vcNormal.y = viewprojmat._24 + viewprojmat._21;
	planes[VF_LEFT_PLANE].m_vcNormal.z = viewprojmat._34 + viewprojmat._31;
	planes[VF_LEFT_PLANE].m_fDistance  = viewprojmat._44 + viewprojmat._41;

	// right clipping plane
	planes[VF_RIGHT_PLANE].m_vcNormal.x = viewprojmat._14 - viewprojmat._11;
	planes[VF_RIGHT_PLANE].m_vcNormal.y = viewprojmat._24 - viewprojmat._21;
	planes[VF_RIGHT_PLANE].m_vcNormal.z = viewprojmat._34 - viewprojmat._31;
	planes[VF_RIGHT_PLANE].m_fDistance  = viewprojmat._44 - viewprojmat._41;

	// top clipping plane
	planes[VF_TOP_PLANE].m_vcNormal.x = viewprojmat._14 - viewprojmat._12;
	planes[VF_TOP_PLANE].m_vcNormal.y = viewprojmat._24 - viewprojmat._22;
	planes[VF_TOP_PLANE].m_vcNormal.z = viewprojmat._34 - viewprojmat._32;
	planes[VF_TOP_PLANE].m_fDistance  = viewprojmat._44 - viewprojmat._42;

	// bottom clipping plane
	planes[VF_BOTTOM_PLANE].m_vcNormal.x = viewprojmat._14 + viewprojmat._12;
	planes[VF_BOTTOM_PLANE].m_vcNormal.y = viewprojmat._24 + viewprojmat._22;
	planes[VF_BOTTOM_PLANE].m_vcNormal.z = viewprojmat._34 + viewprojmat._32;
	planes[VF_BOTTOM_PLANE].m_fDistance  = viewprojmat._44 + viewprojmat._42;

	// far clipping plane
	planes[VF_FAR_PLANE].m_vcNormal.x = viewprojmat._14 - viewprojmat._13;
	planes[VF_FAR_PLANE].m_vcNormal.y = viewprojmat._24 - viewprojmat._23;
	planes[VF_FAR_PLANE].m_vcNormal.z = viewprojmat._34 - viewprojmat._33;
	planes[VF_FAR_PLANE].m_fDistance  = viewprojmat._44 - viewprojmat._43;

	// near clipping plane
	planes[VF_NEAR_PLANE].m_vcNormal.x = viewprojmat._13;
	planes[VF_NEAR_PLANE].m_vcNormal.y = viewprojmat._23;
	planes[VF_NEAR_PLANE].m_vcNormal.z = viewprojmat._33;
	planes[VF_NEAR_PLANE].m_fDistance  = viewprojmat._43;

	// normalize normals
	for( int i=0; i != VF_PLANE_COUNT; ++i )
	{
		float len = -1.0f / planes[i].m_vcNormal.GetLength();
		planes[i].m_vcNormal *= len;
		planes[i].m_fDistance *= len;
		planes[i].Set(planes[i].m_vcNormal, planes[i].m_fDistance); 
	}
}

void Frustum::transform(const Matrix4x4& mat)
{
	for(int i=0; i<VF_PLANE_COUNT; ++i)
		planes[i].Transform(mat);
}


Vector3D Frustum::getFarLeftUp() const
{	
	return planeIntersects(planes[VF_LEFT_PLANE], planes[VF_TOP_PLANE], planes[VF_FAR_PLANE]);
}

Vector3D Frustum::getFarLeftDown() const
{
	return planeIntersects(planes[VF_LEFT_PLANE], planes[VF_BOTTOM_PLANE], planes[VF_FAR_PLANE]);
}

Vector3D Frustum::getFarRightUp() const
{
	return planeIntersects(planes[VF_RIGHT_PLANE], planes[VF_TOP_PLANE], planes[VF_FAR_PLANE]);
}

Vector3D Frustum::getFarRightDown() const
{
	return planeIntersects(planes[VF_RIGHT_PLANE], planes[VF_BOTTOM_PLANE], planes[VF_FAR_PLANE]);
}

Vector3D Frustum::getNearLeftUp() const
{
	return planeIntersects(planes[VF_LEFT_PLANE], planes[VF_TOP_PLANE], planes[VF_NEAR_PLANE]);
}

Vector3D Frustum::getNearLeftDown() const
{
	return planeIntersects(planes[VF_LEFT_PLANE], planes[VF_BOTTOM_PLANE], planes[VF_NEAR_PLANE]);
}

Vector3D Frustum::getNearRightUp() const
{
	return planeIntersects(planes[VF_RIGHT_PLANE], planes[VF_TOP_PLANE], planes[VF_NEAR_PLANE]);
}

Vector3D Frustum::getNearRightDown() const
{
	return planeIntersects(planes[VF_RIGHT_PLANE], planes[VF_BOTTOM_PLANE], planes[VF_NEAR_PLANE]);
}

Vector3D Frustum::planeIntersects( const Plane& p1, const Plane& p2, const Plane& p3 ) const
{
	Vector3D c23, c31, c12;
	c23.Cross( p2.m_vcNormal, p3.m_vcNormal );
	c31.Cross( p3.m_vcNormal, p1.m_vcNormal );
	c12.Cross( p1.m_vcNormal, p2.m_vcNormal );
	return Vector3D( (c23 * p1.m_fDistance + c31 * p2.m_fDistance + c12 * p3.m_fDistance) / -(p1.m_vcNormal * c23) );
}