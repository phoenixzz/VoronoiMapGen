#ifndef __SGP_MATRIX4X4_HEADER__
#define __SGP_MATRIX4X4_HEADER__


//==============================================================================
/**
** 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
** The matrix is a D3D style matrix, row major with translations in the 4th row. 
*/
class Matrix4x4
{
public:
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;

public:
	Matrix4x4(void) noexcept { }
	Matrix4x4(const float* f) noexcept 
	{ 
		memcpy(this, f, sizeof(Matrix4x4)); 
	}
	Matrix4x4(const Matrix4x4& other) noexcept
    {
        memcpy( this, &other, sizeof(Matrix4x4) );
    }
	
    Matrix4x4& operator= (const Matrix4x4& other) noexcept
    {
        memcpy( this, &other, sizeof(Matrix4x4) );
        return *this;
    }
	
	// identity matrix
	inline void Identity(void)								
	{
		memset(this, 0, sizeof(Matrix4x4));
		_11 = _22 = _33 = _44 = 1.0f;
	}

	// Build rotation matrix around X axis ( Angle of rotation in radians )
	inline void RotationX(float a)
	{
		float fCos = cosf(a);
		float fSin = sinf(a);

		_22 =  fCos;
		_23 =  fSin;
		_32 = -fSin;
		_33 =  fCos;

		_11 = _44 = 1.0f;
		_12 = _13 = _14 = _21 = _24 = _31 = _34 = _41 = _42 = _43 = 0.0f;
	}

	// Build rotation matrix around Y axis ( Angle of rotation in radians )
	inline void RotationY(float a)
	{
		float fCos = cosf(a);
		float fSin = sinf(a);

		_11 =  fCos;
		_13 = -fSin;
		_31 =  fSin;
		_33 =  fCos;

		_22 = _44 = 1.0f;
		_12 = _23 = _14 = _21 = _24 = _32 = _34 = _41 = _42 = _43 = 0.0f;
	}

	// Build rotation matrix around Z axis ( Angle of rotation in radians )
	inline void RotationZ(float a)
	{
		float fCos = cosf(a);
		float fSin = sinf(a);

		_11  =  fCos;
		_12  =  fSin;
		_21  = -fSin;
		_22  =  fCos;

		_33 = _44 = 1.0f;
		_13 = _14 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
	}

	// x, y and z ( Angle of rotation in radians )
	inline void RotationXYZ(const Vector3D &vc)				
	{
		float sr, sp, sy, cr, cp, cy;

		Identity();

		sy = sinf( vc.z );
		cy = cosf( vc.z );
		sp = sinf( vc.y );
		cp = cosf( vc.y );
		sr = sinf( vc.x );
		cr = cosf( vc.x );
 
		_11 = cp*cy;
		_12 = cp*sy;
		_13 = -sp;
		_21 = sr*sp*cy+cr*-sy;
		_22 = sr*sp*sy+cr*cy;
		_23 = sr*cp;
		_31 = cr*sp*cy+-sr*-sy;
		_32 = cr*sp*sy+-sr*cy;
		_33 = cr*cp;
	}

	// x, y and z ( Angle of rotation in radians )
	inline void RotationXYZ(float x, float y, float z)			
	{
		RotationXYZ( Vector3D(x, y, z) );
	}

	// Build rotation matrix around arbitrary axis ( Angle of rotation in radians )
	inline void RotationAxis(const Vector4D &_vcAxis, float a)	
	{
		Vector4D vcAxis = _vcAxis;
		float fCos = cosf(a);
		float fSin = sinf(a);
		float fSum = 1.0f - fCos;
   
		if(vcAxis.GetLengthSquared() != 1.0f)
			vcAxis.Normalize();

		_11 = (vcAxis.x * vcAxis.x) * fSum + fCos;
		_12 = (vcAxis.x * vcAxis.y) * fSum - (vcAxis.z * fSin);
		_13 = (vcAxis.x * vcAxis.z) * fSum + (vcAxis.y * fSin);

		_21 = (vcAxis.y * vcAxis.x) * fSum + (vcAxis.z * fSin);
		_22 = (vcAxis.y * vcAxis.y) * fSum + fCos;
		_23 = (vcAxis.y * vcAxis.z) * fSum - (vcAxis.x * fSin);

		_31 = (vcAxis.z * vcAxis.x) * fSum - (vcAxis.y * fSin);
		_32 = (vcAxis.z * vcAxis.y) * fSum + (vcAxis.x * fSin);
		_33 = (vcAxis.z * vcAxis.z) * fSum + fCos;

		_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
		_44 = 1.0f;
	}

	inline void ApplyInverseRotation(Vector4D *pvc)
	{
		float x = pvc->x * _11 + pvc->y * _12 + pvc->z * _13;
		float y = pvc->x * _21 + pvc->y * _22 + pvc->z * _23;
		float z = pvc->x * _31 + pvc->y * _32 + pvc->z * _33;
		pvc->x = x;
		pvc->y = y;
		pvc->z = z;
		pvc->w = 1;
	}

	inline void ApplyInverseRotation(Vector3D *pvc)
	{
		float x = pvc->x * _11 + pvc->y * _12 + pvc->z * _13;
		float y = pvc->x * _21 + pvc->y * _22 + pvc->z * _23;
		float z = pvc->x * _31 + pvc->y * _32 + pvc->z * _33;
		pvc->x = x;
		pvc->y = y;
		pvc->z = z;
	}

	inline void Translate(float dx, float dy, float dz)
	{ 
		_41 = dx;   _42 = dy;   _43 = dz;	
	}

	inline void SetTranslation(Vector3D vc, bool EraseContent=false)
	{
		if(EraseContent) 
			Identity();
		_41 = vc.x;		_42 = vc.y;		_43 = vc.z;
	}

	inline Vector3D GetTranslation(void)	
	{ 
		return Vector3D(_41, _42, _43);
	}

	inline float GetScaleX(void)
	{
		return Vector3D(_11, _12, _13).GetLength();
	}
	inline float GetScaleY(void)
	{
		return Vector3D(_21, _22, _23).GetLength();
	}
	inline float GetScaleZ(void)
	{
		return Vector3D(_31, _32, _33).GetLength();
	}

	// transpose m, save result in this
	inline void TransposeOf(const Matrix4x4 &m)
	{
		_11 = m._11;
		_21 = m._12;
		_31 = m._13;
		_41 = m._14;

		_12 = m._21;
		_22 = m._22;
		_32 = m._23;
		_42 = m._24;

		_13 = m._31;
		_23 = m._32;
		_33 = m._33;
		_43 = m._34;

		_14 = m._41;
		_24 = m._42;
		_34 = m._43;
		_44 = m._44;
	}

	// invert m, save result in this
	void InverseOf(const Matrix4x4 &m);
	
	// matrix multiplication with matrix
	Matrix4x4 operator * (const Matrix4x4 &m) const;

	// linear matrix interpolation
	Matrix4x4& Lerp(const Matrix4x4 &m2, float t);

	// Builds a matrix that reflects the coordinate system about a plane, save result in this
	void ReflectPlane(const Plane &p);
};

#endif