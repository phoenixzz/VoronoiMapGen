#ifndef __SGP_VECTOR4D_HEADER__
#define __SGP_VECTOR4D_HEADER__

//==============================================================================
/**
    A four-coordinate vector.
*/

class Vector4D
{
public:
	float x, y, z, w;

public:
	//! Default constructor (null vector)
	Vector4D() noexcept : x(0), y(0), z(0), w(1.0f) {}
	//! Constructor with two different values
	Vector4D(float nx, float ny, float nz) noexcept : x(nx), y(ny), z(nz), w(1.0f) {}
	Vector4D(float _x, float _y, float _z, float _w) noexcept : x(_x), y(_y), z(_z), w(_w) {}
	Vector4D(const Vector3D& other) noexcept : x(other.x), y(other.y), z(other.z), w(1.0f) {}
	//! Copy constructor
	Vector4D(const Vector4D& other) noexcept : x(other.x), y(other.y), z(other.z), w(other.w) {}
    Vector4D& operator = (const Vector4D& other) noexcept { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }

	inline void Set(float _x, float _y, float _z, float _w=1) noexcept { x=_x; y=_y; z=_z; w=_w; }
	inline void Set(const Vector4D &v) noexcept { x=v.x; y=v.y; z=v.z; w=v.w;}


	/**
	 * Add two vectors together. Note this is faster than '+' due to lack
	 * of additional constructor and return.
	 */
	inline void operator += (const Vector4D &v)	noexcept { x += v.x; y += v.y; z += v.z; }
	/**
	* Subtract two vectors together. Note this is faster than '-' due to lack
	* of additional constructor and return.
	*/
	inline void operator -= (const Vector4D &v) noexcept { x -= v.x; y -= v.y; z -= v.z; }
	// scale vector
	inline void operator *= (float f) noexcept { x *= f; y *= f; z *= f; }
	// scale down
	inline void operator /= (float f) noexcept { x /= f; y /= f; z /= f; }	
	// add scalar
	inline void operator += (float f) noexcept { x += f; y += f; z += f; }	
	// subtract scalar
	inline void operator -= (float f) noexcept { x -= f; y -= f; z -= f; }		
	// dot product
	inline float operator * (const Vector4D &v) const noexcept { return (v.x*x + v.y*y + v.z*z); }	
	// scale vector
	inline Vector4D		operator * (float f) const noexcept { return Vector4D(x*f, y*f, z*f); }
	// scalar divide
	inline Vector4D		operator / (float f) const noexcept { return Vector4D(x/f, y/f, z/f); }
	// add scalar
	inline Vector4D		operator + (float f) const noexcept { return Vector4D(x+f, y+f, z+f); }	
	// subtract scale
	inline Vector4D		operator - (float f) const noexcept { return Vector4D(x-f, y-f, z-f); }		
	// Negative
	inline Vector4D		operator - () const noexcept { return Vector4D(-x, -y, -z); }
	// vector-matrix product
	Vector4D			operator * (const Matrix4x4 &m) const;

	/**
	* Add two vectors together. Note this is slower than '+=' due to 
	* additional constructor and return.
	*/
	inline Vector4D operator + (const Vector4D &v) const noexcept
	{
		return Vector4D( x+v.x, y+v.y, z+v.z );
	}
	/**
	* Subtract two vectors together, use SSE if available. Note this is
	* slower than '-=' due to additional constructor and return.
	*/
	inline Vector4D operator - (const Vector4D &v) const noexcept
	{
		return Vector4D( x-v.x, y-v.y, z-v.z );
	}

    inline float GetLengthSquared() const noexcept { return (float)(x*x + y*y + z*z); }
    inline float GetLength() noexcept 
	{
		float f = 0.0f;
#if SGP_MSVC
		if( SystemStats::hasSSE() )
		{
			w = 0.0f;
			__asm 
			{
				mov    esi,   this            ; vector u
				movups xmm0,  [esi]           ; first vector in xmm0
				mulps  xmm0,  xmm0            ; mul with 2nd vector
				movaps xmm1,  xmm0            ; copy result
				shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
				addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
				movaps xmm1,  xmm0            ; copy results
				shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
				addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
				sqrtss xmm0,  xmm0            ; sqroot from least bit value
				movss     f,  xmm0			  ; move result from xmm0 to edi
			}
			w = 1.0f;
		}
		else
			f = std::sqrt(GetLengthSquared());
#else
		f = std::sqrt(GetLengthSquared()); 
#endif
		return f;
	}

    inline void Normalize() noexcept
	{ 
#if SGP_MSVC
		if( !SystemStats::hasSSE() ) 
		{
			float f = sqrt(x*x + y*y + z*z);
			if(f != 0)
			{
				x/=f; y/=f; z/=f;
			}
		}
		else 
		{
			w = 0.0f;
			__asm 
			{
				mov    esi,   this            ; vector u
				movups xmm0,  [esi]           ; first vector in xmm0
				movaps xmm2,  xmm0            ; copy original vector
				mulps  xmm0,  xmm0            ; mul with 2nd vector
				movaps xmm1,  xmm0            ; copy result
				shufps xmm1,  xmm1, 4Eh       ; shuffle: f1,f0,f3,f2
				addps  xmm0,  xmm1            ; add: f3+f1,f2+f0,f1+f3,f0+f2 
				movaps xmm1,  xmm0            ; copy results
				shufps xmm1,  xmm1, 11h       ; shuffle: f0+f2,f1+f3,f0+f2,f1+f3 
				addps  xmm0,  xmm1            ; add: x,x,f0+f1+f2+f3,f0+f1+f2+f3
         
				rsqrtps xmm0,  xmm0           ; recip. sqrt (faster than ss + shufps)
				mulps   xmm2,  xmm0           ; mul by reciprocal
				movups  [esi], xmm2           ; bring back result
			}
			w = 1.0f;
		}
#else
		float f = sqrt(x*x + y*y + z*z);
		if(f != 0)
		{
			x/=f; y/=f; z/=f;
		}
#endif
	}

	inline float AngleWith(Vector4D &v)
	{
		return (float)acos( (float)((*this) * v) / (this->GetLength() * v.GetLength()) );
	}

	inline void Cross(const Vector4D &v1, const Vector4D &v2) 
	{
#if SGP_MSVC
		if ( !SystemStats::hasSSE() )
		{
			x = v1.y * v2.z - v1.z * v2.y;
			y = v1.z * v2.x - v1.x * v2.z;
			z = v1.x * v2.y - v1.y * v2.x;
			w = 1;
		}
		else 
		{
			__asm 
			{
				mov esi, v1
				mov edi, v2

				movups xmm0, [esi]
				movups xmm1, [edi]
				movaps xmm2, xmm0
				movaps xmm3, xmm1

				shufps xmm0, xmm0, 0xC9
				shufps xmm1, xmm1, 0xD2
				mulps  xmm0, xmm1

				shufps xmm2, xmm2, 0xD2
				shufps xmm3, xmm3, 0xC9
				mulps  xmm2, xmm3

				subps  xmm0, xmm2

				mov    esi, this
				movups [esi], xmm0
			}
			w = 1;
		}
#else
		x = v1.y * v2.z - v1.z * v2.y;
		y = v1.z * v2.x - v1.x * v2.z;
		z = v1.x * v2.y - v1.y * v2.x;
#endif
	}

	/**
	* rotate the vector with the given matrix, ignoring translation
	* information in matrix if they are present
	*/
	void RotateWith(const Matrix4x4 &m);
	/**
	* rotate the vector with the inverse rotation part of the given
	* matrix, ignoring translation of the matrix if present.
	*/
	void InvRotateWith(const Matrix4x4 &m);
};

#endif