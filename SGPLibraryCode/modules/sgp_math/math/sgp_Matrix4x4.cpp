#if SGP_MSVC && SGP_WIN32
// naked helper function for matrix multiplication
__declspec(naked) void MatrixMult(float* /*src1*/, float* /*src2*/, float* /*dst*/) 
{
	__asm 
	{
		mov edx, dword ptr [esp+4] ; src1
		mov eax, dword ptr [esp+0Ch] ; dst
		mov ecx, dword ptr [esp+8] ; src2
		movss xmm0, dword ptr [edx]
		movups xmm1, xmmword ptr [ecx]
		shufps xmm0, xmm0, 0
		movss xmm2, dword ptr [edx+4]
		mulps xmm0, xmm1
		shufps xmm2, xmm2, 0
		movups xmm3, xmmword ptr [ecx+10h]
		movss xmm7, dword ptr [edx+8]
		mulps xmm2, xmm3
		shufps xmm7, xmm7, 0
		addps xmm0, xmm2
		movups xmm4, xmmword ptr [ecx+20h]
		movss xmm2, dword ptr [edx+0Ch]
		mulps xmm7, xmm4
		shufps xmm2, xmm2, 0
		addps xmm0, xmm7
		movups xmm5, xmmword ptr [ecx+30h]
		movss xmm6, dword ptr [edx+10h]
		mulps xmm2, xmm5
		movss xmm7, dword ptr [edx+14h]
		shufps xmm6, xmm6, 0
		addps xmm0, xmm2
		shufps xmm7, xmm7, 0
		movlps qword ptr [eax], xmm0
		movhps qword ptr [eax+8], xmm0
		mulps xmm7, xmm3
		movss xmm0, dword ptr [edx+18h]
		mulps xmm6, xmm1
		shufps xmm0, xmm0, 0
		addps xmm6, xmm7
		mulps xmm0, xmm4
		movss xmm2, dword ptr [edx+24h]
		addps xmm6, xmm0
		movss xmm0, dword ptr [edx+1Ch]
		movss xmm7, dword ptr [edx+20h]
		shufps xmm0, xmm0, 0
		shufps xmm7, xmm7, 0
		mulps xmm0, xmm5
		mulps xmm7, xmm1
		addps xmm6, xmm0
		shufps xmm2, xmm2, 0
		movlps qword ptr [eax+10h], xmm6
		movhps qword ptr [eax+18h], xmm6
		mulps xmm2, xmm3
		movss xmm6, dword ptr [edx+28h]
		addps xmm7, xmm2
		shufps xmm6, xmm6, 0
		movss xmm2, dword ptr [edx+2Ch]
		mulps xmm6, xmm4
		shufps xmm2, xmm2, 0
		addps xmm7, xmm6
		mulps xmm2, xmm5
		movss xmm0, dword ptr [edx+34h]
		addps xmm7, xmm2
		shufps xmm0, xmm0, 0
		movlps qword ptr [eax+20h], xmm7
		movss xmm2, dword ptr [edx+30h]
		movhps qword ptr [eax+28h], xmm7
		mulps xmm0, xmm3
		shufps xmm2, xmm2, 0
		movss xmm6, dword ptr [edx+38h]
		mulps xmm2, xmm1
		shufps xmm6, xmm6, 0
		addps xmm2, xmm0
		mulps xmm6, xmm4
		movss xmm7, dword ptr [edx+3Ch]
		shufps xmm7, xmm7, 0
		addps xmm2, xmm6
		mulps xmm7, xmm5
		addps xmm2, xmm7
		movups xmmword ptr [eax+30h], xmm2
		ret
	} // asm
}
#endif

// matrix multiplication
Matrix4x4 Matrix4x4::operator * (const Matrix4x4 &m) const
{
	Matrix4x4 mResult;
#if SGP_MSVC && SGP_WIN32
	if( !SystemStats::hasSSE() )
	{
		float *pA = (float*)this;
		float *pB = (float*)&m;
		float *pM = (float*)&mResult;

		memset(pM, 0, sizeof(Matrix4x4));

		for(unsigned char i=0; i<4; i++) 
		{
			for(unsigned char j=0; j<4; j++) 
			{
				pM[4*i+j] += pA[4*i]   * pB[j];
				pM[4*i+j] += pA[4*i+1] * pB[4+j];
				pM[4*i+j] += pA[4*i+2] * pB[8+j];
				pM[4*i+j] += pA[4*i+3] * pB[12+j];
			}
		}
	}
	else 
	{
		MatrixMult((float*)this, (float*)&m, (float*)&mResult);
	}
#else
	float *pA = (float*)this;
	float *pB = (float*)&m;
	float *pM = (float*)&mResult;

	memset(pM, 0, sizeof(Matrix4x4));

	for(unsigned char i=0; i<4; i++) 
	{
		for(unsigned char j=0; j<4; j++) 
		{
			pM[4*i+j] += pA[4*i]   * pB[j];
			pM[4*i+j] += pA[4*i+1] * pB[4+j];
			pM[4*i+j] += pA[4*i+2] * pB[8+j];
			pM[4*i+j] += pA[4*i+3] * pB[12+j];
		}
	}
#endif
	return mResult;
}



// invert m, save result in this
void Matrix4x4::InverseOf(const Matrix4x4 &m)
{
	Matrix4x4 mTrans;
	float   fTemp[12],  // cofaktors
			fDet;

	// calculate transposed matrix
	mTrans.TransposeOf(m);


	fTemp[ 0]  = mTrans._33 * mTrans._44;
	fTemp[ 1]  = mTrans._34 * mTrans._43;
	fTemp[ 2]  = mTrans._32 * mTrans._44;
	fTemp[ 3]  = mTrans._34 * mTrans._42;
	fTemp[ 4]  = mTrans._32 * mTrans._43;
	fTemp[ 5]  = mTrans._33 * mTrans._42;
	fTemp[ 6]  = mTrans._31 * mTrans._44;
	fTemp[ 7]  = mTrans._34 * mTrans._41;
	fTemp[ 8]  = mTrans._31 * mTrans._43;
	fTemp[ 9]  = mTrans._33 * mTrans._41;
	fTemp[10]  = mTrans._31 * mTrans._42;
	fTemp[11]  = mTrans._32 * mTrans._41;


	this->_11  = fTemp[0]*mTrans._22 + fTemp[3]*mTrans._23 + fTemp[4] *mTrans._24;
	this->_11 -= fTemp[1]*mTrans._22 + fTemp[2]*mTrans._23 + fTemp[5] *mTrans._24;
	this->_12  = fTemp[1]*mTrans._21 + fTemp[6]*mTrans._23 + fTemp[9] *mTrans._24;
	this->_12 -= fTemp[0]*mTrans._21 + fTemp[7]*mTrans._23 + fTemp[8] *mTrans._24;
	this->_13  = fTemp[2]*mTrans._21 + fTemp[7]*mTrans._22 + fTemp[10]*mTrans._24;
	this->_13 -= fTemp[3]*mTrans._21 + fTemp[6]*mTrans._22 + fTemp[11]*mTrans._24;
	this->_14  = fTemp[5]*mTrans._21 + fTemp[8]*mTrans._22 + fTemp[11]*mTrans._23;
	this->_14 -= fTemp[4]*mTrans._21 + fTemp[9]*mTrans._22 + fTemp[10]*mTrans._23;
	this->_21  = fTemp[1]*mTrans._12 + fTemp[2]*mTrans._13 + fTemp[5] *mTrans._14;
	this->_21 -= fTemp[0]*mTrans._12 + fTemp[3]*mTrans._13 + fTemp[4] *mTrans._14;
	this->_22  = fTemp[0]*mTrans._11 + fTemp[7]*mTrans._13 + fTemp[8] *mTrans._14;
	this->_22 -= fTemp[1]*mTrans._11 + fTemp[6]*mTrans._13 + fTemp[9] *mTrans._14;
	this->_23  = fTemp[3]*mTrans._11 + fTemp[6]*mTrans._12 + fTemp[11]*mTrans._14;
	this->_23 -= fTemp[2]*mTrans._11 + fTemp[7]*mTrans._12 + fTemp[10]*mTrans._14;
	this->_24  = fTemp[4]*mTrans._11 + fTemp[9]*mTrans._12 + fTemp[10]*mTrans._13;
	this->_24 -= fTemp[5]*mTrans._11 + fTemp[8]*mTrans._12 + fTemp[11]*mTrans._13;


	fTemp[ 0]  = mTrans._13 * mTrans._24;
	fTemp[ 1]  = mTrans._14 * mTrans._23;
	fTemp[ 2]  = mTrans._12 * mTrans._24;
	fTemp[ 3]  = mTrans._14 * mTrans._22;
	fTemp[ 4]  = mTrans._12 * mTrans._23;
	fTemp[ 5]  = mTrans._13 * mTrans._22;
	fTemp[ 6]  = mTrans._11 * mTrans._24;
	fTemp[ 7]  = mTrans._14 * mTrans._21;
	fTemp[ 8]  = mTrans._11 * mTrans._23;
	fTemp[ 9]  = mTrans._13 * mTrans._21;
	fTemp[10]  = mTrans._11 * mTrans._22;
	fTemp[11]  = mTrans._12 * mTrans._21;


	this->_31  = fTemp[0] *mTrans._42 + fTemp[3] *mTrans._43 + fTemp[4] *mTrans._44;
	this->_31 -= fTemp[1] *mTrans._42 + fTemp[2] *mTrans._43 + fTemp[5] *mTrans._44;
	this->_32  = fTemp[1] *mTrans._41 + fTemp[6] *mTrans._43 + fTemp[9] *mTrans._44;
	this->_32 -= fTemp[0] *mTrans._41 + fTemp[7] *mTrans._43 + fTemp[8] *mTrans._44;
	this->_33  = fTemp[2] *mTrans._41 + fTemp[7] *mTrans._42 + fTemp[10]*mTrans._44;
	this->_33 -= fTemp[3] *mTrans._41 + fTemp[6] *mTrans._42 + fTemp[11]*mTrans._44;
	this->_34  = fTemp[5] *mTrans._41 + fTemp[8] *mTrans._42 + fTemp[11]*mTrans._43;
	this->_34 -= fTemp[4] *mTrans._41 + fTemp[9] *mTrans._42 + fTemp[10]*mTrans._43;
	this->_41  = fTemp[2] *mTrans._33 + fTemp[5] *mTrans._34 + fTemp[1] *mTrans._32;
	this->_41 -= fTemp[4] *mTrans._34 + fTemp[0] *mTrans._32 + fTemp[3] *mTrans._33;
	this->_42  = fTemp[8] *mTrans._34 + fTemp[0] *mTrans._31 + fTemp[7] *mTrans._33;
	this->_42 -= fTemp[6] *mTrans._33 + fTemp[9] *mTrans._34 + fTemp[1] *mTrans._31;
	this->_43  = fTemp[6] *mTrans._32 + fTemp[11]*mTrans._34 + fTemp[3] *mTrans._31;
	this->_43 -= fTemp[10]*mTrans._34 + fTemp[2] *mTrans._31 + fTemp[7] *mTrans._32;
	this->_44  = fTemp[10]*mTrans._33 + fTemp[4] *mTrans._31 + fTemp[9] *mTrans._32;
	this->_44 -= fTemp[8] *mTrans._32 + fTemp[11]*mTrans._33 + fTemp[5] *mTrans._31;

	fDet =	mTrans._11*this->_11 + 
			mTrans._12*this->_12 + 
			mTrans._13*this->_13 +
			mTrans._14*this->_14;

	fDet = 1/fDet;

	this->_11 *= fDet;  
	this->_12 *= fDet;  
	this->_13 *= fDet;  
	this->_14 *= fDet;

	this->_21 *= fDet;  
	this->_22 *= fDet;  
	this->_23 *= fDet;  
	this->_24 *= fDet;

	this->_31 *= fDet;  
	this->_32 *= fDet;  
	this->_33 *= fDet;  
	this->_34 *= fDet;

	this->_41 *= fDet;  
	this->_42 *= fDet;  
	this->_43 *= fDet;  
	this->_44 *= fDet;
}

Matrix4x4& Matrix4x4::Lerp(const Matrix4x4 &m2, float t)
{
	this->_11 = this->_11 * (1-t) + m2._11 * t; 
	this->_12 = this->_12 * (1-t) + m2._12 * t; 
	this->_13 = this->_13 * (1-t) + m2._13 * t; 
	this->_14 = this->_14 * (1-t) + m2._14 * t;

	this->_21 = this->_21 * (1-t) + m2._21 * t; 
	this->_22 = this->_22 * (1-t) + m2._22 * t; 
	this->_23 = this->_23 * (1-t) + m2._23 * t; 
	this->_24 = this->_24 * (1-t) + m2._24 * t;

	this->_31 = this->_31 * (1-t) + m2._31 * t; 
	this->_32 = this->_32 * (1-t) + m2._32 * t; 
	this->_33 = this->_33 * (1-t) + m2._33 * t; 
	this->_34 = this->_34 * (1-t) + m2._34 * t;

	this->_41 = this->_41 * (1-t) + m2._41 * t; 
	this->_42 = this->_42 * (1-t) + m2._42 * t; 
	this->_43 = this->_43 * (1-t) + m2._43 * t; 
	this->_44 = this->_44 * (1-t) + m2._44 * t;

	return *this;
}

// Builds a matrix that reflects the coordinate system about a plane
void Matrix4x4::ReflectPlane(const Plane &p)
{
	Plane Nplane = p;
	Nplane.Normalize();

	Identity();

	_11 = -2.0f * Nplane.m_vcNormal.x * Nplane.m_vcNormal.x + 1.0f;
	_12 = -2.0f * Nplane.m_vcNormal.x * Nplane.m_vcNormal.y;
	_13 = -2.0f * Nplane.m_vcNormal.x * Nplane.m_vcNormal.z;
	_21 = -2.0f * Nplane.m_vcNormal.y * Nplane.m_vcNormal.x;
	_22 = -2.0f * Nplane.m_vcNormal.y * Nplane.m_vcNormal.y + 1.0f;
	_23 = -2.0f * Nplane.m_vcNormal.y * Nplane.m_vcNormal.z;
	_31 = -2.0f * Nplane.m_vcNormal.z * Nplane.m_vcNormal.x;
	_32 = -2.0f * Nplane.m_vcNormal.z * Nplane.m_vcNormal.y;
	_33 = -2.0f * Nplane.m_vcNormal.z * Nplane.m_vcNormal.z + 1.0f;
	_41 = -2.0f * Nplane.m_fDistance * Nplane.m_vcNormal.x;
	_42 = -2.0f * Nplane.m_fDistance * Nplane.m_vcNormal.y;
	_43 = -2.0f * Nplane.m_fDistance * Nplane.m_vcNormal.z;

	return;
}