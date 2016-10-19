

Matrix4x4 COpenGLES2Camera::m_mOpenGLProjTrans;
Matrix4x4 COpenGLES2Camera::m_mOpenGLProjTransInv;

/**
 * Calculate orthogonal projection and view matrix to render
 * vertices who's world-coordinates are given in screen space. (2D MODE)
 */
void COpenGLES2Camera::Prepare2DMode() 
{
	// set matrices to identity
	memset(&m_mViewMatrix, 0, sizeof(Matrix4x4)); 
	memset(&m_mProjMatrix, 0, sizeof(Matrix4x4)); 
	

	// orthogonal projection matrix
	if( m_pRenderDevice->isViewportRotated() )
	{
		m_mProjMatrix._12 =  2.0f/(float)m_pRenderDevice->getViewPort().Height;
		m_mProjMatrix._21 = -2.0f/(float)m_pRenderDevice->getViewPort().Width;
	}
	else
	{
		m_mProjMatrix._11 =  2.0f/(float)m_pRenderDevice->getViewPort().Width;
		m_mProjMatrix._22 =  2.0f/(float)m_pRenderDevice->getViewPort().Height;
	}
	m_mProjMatrix._33 =  1.0f/(m_fFar-m_fNear);
	m_mProjMatrix._43 = -m_fNear/(m_fFar-m_fNear);
	m_mProjMatrix._44 =  1.0f;

	// OpenGL Z convert -1~1
	m_mProjMatrix = m_mProjMatrix * m_mOpenGLProjTrans;

	// 2D view matrix
	float tx, ty, tz;
	tx = -(float)m_pRenderDevice->getViewPort().Width + m_pRenderDevice->getViewPort().Width * 0.5f;
	ty =  (float)m_pRenderDevice->getViewPort().Height - m_pRenderDevice->getViewPort().Height * 0.5f;
	tz =  m_fNear + 0.1f;
	m_mViewMatrix._11 = m_mViewMatrix._33 = m_mViewMatrix._44 = 1.0f;
	m_mViewMatrix._22 = -1.0f;
	m_mViewMatrix._41 = tx; 
	m_mViewMatrix._42 = ty; 
	m_mViewMatrix._43 = tz;

	CalcViewProjMatrix();
}

void COpenGLES2Camera::SetClippingPlanes(float fNear, float fFar)
{
	m_fNear = fNear;
	m_fFar  = fFar;

	if(m_fNear <= 0.0f)
		m_fNear = 0.01f;

	if(m_fFar <= 1.0f)
		m_fFar = 1.00f;
   
	if(m_fNear >= m_fFar)
	{
		m_fNear = m_fFar;
		m_fFar  = m_fNear + 1.0f;
	}

	float Q = 1.0f / (m_fFar - m_fNear);
	float X = m_fNear / (m_fNear - m_fFar);

	// change orthogonal projection
	if( m_Mode == SGPCT_TWOD )
	{
		// change 2D projection and view
		Prepare2DMode();
	}
	else if( m_Mode == SGPCT_ORTHOGONAL )
	{
		//float z =  -2.0f / (m_fFar-m_fNear);
		//float tz = -(m_fFar+m_fNear) / (m_fFar-m_fNear);

		m_mProjMatrix._33 = Q;
		m_mProjMatrix._43 = X;
		// OpenGL Z convert -1~1
		m_mProjMatrix = m_mProjMatrix * m_mOpenGLProjTrans;
	}
	else if( m_Mode == SGPCT_PERSPECTIVE )
	{
		//float depth = m_fFar - m_fNear;
		//float q = -(m_fFar + m_fNear) / depth;
		//float qn = -2 * (m_fFar * m_fNear) / depth;
		Q *= m_fFar;
		X = -Q * m_fNear;
		m_mProjMatrix._33 = Q;
		m_mProjMatrix._43 = X;
		// OpenGL Z convert -1~1
		m_mProjMatrix = m_mProjMatrix * m_mOpenGLProjTrans;
	}
}
/**
 * Calculate perspective 3D projection matrix.
 * -> IN: float     - aspect ration (viewport height / width)
 */
void COpenGLES2Camera::CalcPerspProjMatrix(float fAspect)
{
	if(fabs(m_fFar - m_fNear) < 0.01f)
	{
		m_fNear = m_fFar;
		m_fFar  = m_fNear + 1.0f;
	}

	float w = 1.0f / (tanf(m_fFov * pi_over_360) * fAspect);
	float h = 1.0f / tanf(m_fFov * pi_over_360);
	float Q = m_fFar / (m_fFar - m_fNear);

	memset(&m_mProjMatrix, 0, sizeof(Matrix4x4));
	if( m_pRenderDevice->isViewportRotated() )
	{
		m_mProjMatrix._12 = h;
		m_mProjMatrix._21 = -w;
		m_mProjMatrix._33 = Q;
		m_mProjMatrix._34 = 1.0f;
		m_mProjMatrix._43 = -Q*m_fNear;
	}
	else
	{
		m_mProjMatrix._11 = w;
		m_mProjMatrix._22 = h;
		m_mProjMatrix._33 = Q;
		m_mProjMatrix._34 = 1.0f;
		m_mProjMatrix._43 = -Q*m_fNear;
	}
	// OpenGL Z convert -1~1
	m_mProjMatrix = m_mProjMatrix * m_mOpenGLProjTrans;

	//float xymax = m_fNear * tanf(m_fFov * pi_over_360);
	//float ymin = -xymax;
	//float xmin = -xymax;

	//float width = xymax - xmin;
	//float height = xymax - ymin;

	//float depth = m_fFar - m_fNear;
	//float q = -(m_fFar + m_fNear) / depth;
	//float qn = -2 * (m_fFar * m_fNear) / depth;

	//float w = 2 * m_fNear / width;
	//w = w / fAspect;
	//float h = 2 * m_fNear / height;

	//memset(&m_mProjMatrix, 0, sizeof(Matrix4x4));

	//m_mProjMatrix._11 = w;
	//m_mProjMatrix._22 = h;
	//m_mProjMatrix._33 = q;
	//m_mProjMatrix._34 = -1;
	//m_mProjMatrix._43 = qn;

	return;
}

/**
 * Retrieve active frustrum planes, normals pointing outwards.
 * Construct frustum from view-projection transformation.
 * -> IN: Plane* - address to store 6 planes
 */
void COpenGLES2Camera::GetFrustrum(Plane *p)
{
	// left plane
	p[0].m_vcNormal.x = -(m_mViewProjMatrix._14 + m_mViewProjMatrix._11);
	p[0].m_vcNormal.y = -(m_mViewProjMatrix._24 + m_mViewProjMatrix._21);
	p[0].m_vcNormal.z = -(m_mViewProjMatrix._34 + m_mViewProjMatrix._31);
	p[0].m_fDistance  = -(m_mViewProjMatrix._44 + m_mViewProjMatrix._41);

	// right plane
	p[1].m_vcNormal.x = -(m_mViewProjMatrix._14 - m_mViewProjMatrix._11);
	p[1].m_vcNormal.y = -(m_mViewProjMatrix._24 - m_mViewProjMatrix._21);
	p[1].m_vcNormal.z = -(m_mViewProjMatrix._34 - m_mViewProjMatrix._31);
	p[1].m_fDistance  = -(m_mViewProjMatrix._44 - m_mViewProjMatrix._41);

	// top plane
	p[2].m_vcNormal.x = -(m_mViewProjMatrix._14 - m_mViewProjMatrix._12);
	p[2].m_vcNormal.y = -(m_mViewProjMatrix._24 - m_mViewProjMatrix._22);
	p[2].m_vcNormal.z = -(m_mViewProjMatrix._34 - m_mViewProjMatrix._32);
	p[2].m_fDistance  = -(m_mViewProjMatrix._44 - m_mViewProjMatrix._42);
   
	// bottom plane
	p[3].m_vcNormal.x = -(m_mViewProjMatrix._14 + m_mViewProjMatrix._12);
	p[3].m_vcNormal.y = -(m_mViewProjMatrix._24 + m_mViewProjMatrix._22);
	p[3].m_vcNormal.z = -(m_mViewProjMatrix._34 + m_mViewProjMatrix._32);
	p[3].m_fDistance  = -(m_mViewProjMatrix._44 + m_mViewProjMatrix._42);

	// near plane
	p[4].m_vcNormal.x = -m_mViewProjMatrix._13;
	p[4].m_vcNormal.y = -m_mViewProjMatrix._23;
	p[4].m_vcNormal.z = -m_mViewProjMatrix._33;
	p[4].m_fDistance  = -m_mViewProjMatrix._43;

	// far plane
	p[5].m_vcNormal.x = -(m_mViewProjMatrix._14 - m_mViewProjMatrix._13);
	p[5].m_vcNormal.y = -(m_mViewProjMatrix._24 - m_mViewProjMatrix._23);
	p[5].m_vcNormal.z = -(m_mViewProjMatrix._34 - m_mViewProjMatrix._33);
	p[5].m_fDistance  = -(m_mViewProjMatrix._44 - m_mViewProjMatrix._43);

	// normalize frustrum normals
	for(int i=0;i<6;i++)
	{
		float fL = p[i].m_vcNormal.GetLength();
		p[i].m_vcNormal /= fL;
		p[i].m_fDistance  /= fL;
	}
}

void COpenGLES2Camera::SetViewMatrix3D(
	const Vector4D& vcRight, const Vector4D& vcUp, const Vector4D& vcDir, const Vector4D& vcEyePos)
{
	m_CameraPos = vcEyePos;

	m_mViewMatrix._14 = m_mViewMatrix._24 = m_mViewMatrix._34 = 0.0f;
	m_mViewMatrix._44 = 1.0f;

	m_mViewMatrix._11 = vcRight.x;
	m_mViewMatrix._21 = vcRight.y;
	m_mViewMatrix._31 = vcRight.z;
	m_mViewMatrix._41 = - (vcRight*vcEyePos);

	m_mViewMatrix._12 = vcUp.x;
	m_mViewMatrix._22 = vcUp.y;
	m_mViewMatrix._32 = vcUp.z;
	m_mViewMatrix._42 = - (vcUp*vcEyePos);

	m_mViewMatrix._13 = vcDir.x;
	m_mViewMatrix._23 = vcDir.y;
	m_mViewMatrix._33 = vcDir.z;
	m_mViewMatrix._43 = - (vcDir*vcEyePos);

	CalcViewProjMatrix();
}

void COpenGLES2Camera::CalcOrthoProjMatrix(float l, float r, float b, float t, float fN, float fF)
{
	if(fabs(fF - fN) < 0.01f)
	{
		fN = fF;
		fF = fN + 1.0f;
	}

	float x =  2.0f / (r-l);
	float y =  2.0f / (t-b);
	float z =  1.0f / (fF-fN);
	float tx = -(r+l) / (r-l);
	float ty = -(t+b) / (t-b);
	float tz = -(fN) / (fF-fN);

	memset(&m_mProjMatrix, 0, sizeof(Matrix4x4));
	if( m_pRenderDevice->isViewportRotated() )
	{
		m_mProjMatrix._12 = y;
		m_mProjMatrix._21 = -x;
		m_mProjMatrix._33 = z;
	}
	else
	{
		m_mProjMatrix._11 = x;
		m_mProjMatrix._22 = y;
		m_mProjMatrix._33 = z;
	}

	m_mProjMatrix._41 = tx;
	m_mProjMatrix._42 = ty;
	m_mProjMatrix._43 = tz;
	m_mProjMatrix._44 = 1.0f;

	// OpenGL Z convert -1~1
	m_mProjMatrix = m_mProjMatrix * m_mOpenGLProjTrans;
}


void COpenGLES2Camera::CalcOrthoProjMatrix(float w, float h, float fN, float fF)
{
	if(fabs(fF - fN) < 0.01f)
	{
		fN = fF;
		fF = fN + 1.0f;
	}


	memset(&m_mProjMatrix, 0, sizeof(Matrix4x4));
	if( m_pRenderDevice->isViewportRotated() )
	{
		m_mProjMatrix._12 = 2.0f / h;
		m_mProjMatrix._21 = -2.0f / w;
		m_mProjMatrix._33 = 1.0f / (fF - fN);
	}
	else
	{
		m_mProjMatrix._11 = 2.0f / w;
		m_mProjMatrix._22 = 2.0f / h;
		m_mProjMatrix._33 = 1.0f / (fF - fN);
	}
	
	m_mProjMatrix._43 = -(fN) / (fF - fN);
	m_mProjMatrix._44 = 1.0f;

	// OpenGL Z convert -1~1
	m_mProjMatrix = m_mProjMatrix * m_mOpenGLProjTrans;
	return;
}