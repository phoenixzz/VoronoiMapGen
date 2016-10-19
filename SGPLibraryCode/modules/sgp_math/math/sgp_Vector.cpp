
float Vector3D::GetLength() const noexcept
{
	return Vector4D(x,y,z).GetLength();
}

void Vector3D::Normalize() noexcept
{ 
	Vector4D Val4D(x,y,z);
	Val4D.Normalize();
	x = Val4D.x;
	y = Val4D.y;
	z = Val4D.z;
}

void Vector3D::Cross(const Vector3D &v1, const Vector3D &v2)
{
	Vector4D V14D(v1);
	Vector4D V24D(v2);
	Vector4D CrossResult;
	CrossResult.Cross(V14D, V24D);
	x = CrossResult.x;
	y = CrossResult.y;
	z = CrossResult.z;
}


// vector-matrix product
Vector4D Vector4D::operator * (const Matrix4x4 &m) const
{
	Vector4D vcResult;
#if SGP_MSVC
	if( !SystemStats::hasSSE() )
	{
		vcResult.x = x*m._11 + y*m._21 + z*m._31 + m._41;
		vcResult.y = x*m._12 + y*m._22 + z*m._32 + m._42;
		vcResult.z = x*m._13 + y*m._23 + z*m._33 + m._43;
		vcResult.w = x*m._14 + y*m._24 + z*m._34 + m._44;
   
		vcResult.x = vcResult.x/vcResult.w;
		vcResult.y = vcResult.y/vcResult.w;
		vcResult.z = vcResult.z/vcResult.w;
		vcResult.w = 1.0f;
	}
	else 
	{
		float *ptrRet = (float*)&vcResult;

		__asm 
		{
			mov    ecx,   this      ; vector
			mov    edx,   m         ; matrix

			movss  xmm0,  [ecx]
			mov    eax,   ptrRet    ; result vector

			shufps xmm0,  xmm0, 0
			movss  xmm1,  [ecx+4]

			movups xmm4, [edx]
			mulps  xmm0,  xmm4
			//mulps  xmm0,  [edx]

			shufps xmm1,  xmm1, 0
			movss  xmm2,  [ecx+8]

			movups xmm5, [edx+16]
			mulps  xmm1,  xmm5
			//mulps  xmm1,  [edx+16]

			shufps xmm2,  xmm2, 0
			movss  xmm3,  [ecx+12]

			movups xmm6, [edx+32]
			mulps  xmm2,  xmm6
			//mulps  xmm2,  [edx+32]

			shufps xmm3,  xmm3, 0
			addps  xmm0,  xmm1

			movups xmm7, [edx+48]
			mulps  xmm3,  xmm7
			//mulps  xmm3,  [edx+48]

			addps  xmm2,  xmm3
			addps  xmm0,  xmm2

			movups [eax], xmm0		; save as result
			//mov  [eax+3], 1			; w = 1
		}
	}
#else
	vcResult.x = x*m._11 + y*m._21 + z*m._31 + m._41;
	vcResult.y = x*m._12 + y*m._22 + z*m._32 + m._42;
	vcResult.z = x*m._13 + y*m._23 + z*m._33 + m._43;
	vcResult.w = x*m._14 + y*m._24 + z*m._34 + m._44;
   
	vcResult.x = vcResult.x/vcResult.w;
	vcResult.y = vcResult.y/vcResult.w;
	vcResult.z = vcResult.z/vcResult.w;
	vcResult.w = 1.0f;
#endif
	return vcResult;
}

/**
* rotate the vector with the given matrix, ignoring translation
* information in matrix if they are present
*/
void Vector4D::RotateWith(const Matrix4x4 &m)
{
	// applying rotational part of matrix only
	float _x = x * m._11 + y * m._21 + z * m._31;
	float _y = x * m._12 + y * m._22 + z * m._32;
	float _z = x * m._13 + y * m._23 + z * m._33;
	x = _x;   y = _y;   z = _z;
}

/**
* rotate the vector with the inverse rotation part of the given
* matrix, ignoring translation of the matrix if present.
*/
void Vector4D::InvRotateWith(const Matrix4x4 &m)
{
	// using transposed matrix
	float _x = x * m._11 + y * m._12 + z * m._13;
	float _y = x * m._21 + y * m._22 + z * m._23;
	float _z = x * m._31 + y * m._32 + z * m._33;
	x = _x;   y = _y;   z = _z;
}


/**
* rotate the vector with the given matrix, ignoring translation
* information in matrix if they are present
*/
void Vector3D::RotateWith(const Matrix4x4 &m)
{
	// applying rotational part of matrix only
	float _x = x * m._11 + y * m._21 + z * m._31;
	float _y = x * m._12 + y * m._22 + z * m._32;
	float _z = x * m._13 + y * m._23 + z * m._33;
	x = _x;   y = _y;   z = _z;
}

/**
* rotate the vector with the inverse rotation part of the given
* matrix, ignoring translation of the matrix if present.
*/
void Vector3D::InvRotateWith(const Matrix4x4 &m)
{
	// using transposed matrix
	float _x = x * m._11 + y * m._12 + z * m._13;
	float _y = x * m._21 + y * m._22 + z * m._23;
	float _z = x * m._31 + y * m._32 + z * m._33;
	x = _x;   y = _y;   z = _z;
}

Vector3D Vector3D::operator * (const Matrix4x4 &m) const
{
	Vector3D ResultVal3D;
	Vector4D ResultVal4D(*this);

	ResultVal4D = ResultVal4D * m;
	ResultVal3D.x = ResultVal4D.x;
	ResultVal3D.y = ResultVal4D.y;
	ResultVal3D.z = ResultVal4D.z;

	return ResultVal3D;
}