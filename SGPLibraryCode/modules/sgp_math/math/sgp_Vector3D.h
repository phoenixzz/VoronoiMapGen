#ifndef __SGP_VECTOR3D_HEADER__
#define __SGP_VECTOR3D_HEADER__

class Matrix4x4;
//==============================================================================
/**
    A three-coordinate vector.
*/
class Vector3D
{
public:
	float x, y, z;

public:
	//! Default constructor (null vector)
	Vector3D() noexcept : x(0), y(0), z(0) {}
	//! Constructor with two different values
	Vector3D(float nx, float ny, float nz) noexcept : x(nx), y(ny), z(nz) {}
	//! Copy constructor
	Vector3D(const Vector3D& other) noexcept : x(other.x), y(other.y), z(other.z) {}
    Vector3D& operator = (const Vector3D& other) noexcept { x = other.x; y = other.y; z = other.z; return *this; }

	inline void Set(float _x, float _y, float _z) noexcept { x=_x; y=_y; z=_z; }
	inline void Set(const Vector3D &v) noexcept { x=v.x; y=v.y; z=v.z; }

	/**
	 * Add two vectors together. Note this is faster than '+' due to lack
	 * of additional constructor and return.
	 */
	inline void operator += (const Vector3D &v)	noexcept { x += v.x; y += v.y; z += v.z; }
	/**
	* Subtract two vectors together. Note this is faster than '-' due to lack
	* of additional constructor and return.
	*/
	inline void operator -= (const Vector3D &v) noexcept { x -= v.x; y -= v.y; z -= v.z; }
	// scale vector
	inline void operator *= (float f) noexcept { x *= f; y *= f; z *= f; }
	// scale down
	inline void operator /= (float f) noexcept { x /= f; y /= f; z /= f; }	
	// add scalar
	inline void operator += (float f) noexcept { x += f; y += f; z += f; }	
	// subtract scalar
	inline void operator -= (float f) noexcept { x -= f; y -= f; z -= f; }		
	// dot product
	inline float operator * (const Vector3D &v) const noexcept { return (v.x*x + v.y*y + v.z*z); }	
	// scale vector
	inline Vector3D		operator * (float f) const noexcept { return Vector3D(x*f, y*f, z*f); }
	// scalar divide
	inline Vector3D		operator / (float f) const noexcept { return Vector3D(x/f, y/f, z/f); }
	// add scalar
	inline Vector3D		operator + (float f) const noexcept { return Vector3D(x+f, y+f, z+f); }	
	// subtract scale
	inline Vector3D		operator - (float f) const noexcept { return Vector3D(x-f, y-f, z-f); }		
	// Negative
	inline Vector3D		operator - () const noexcept { return Vector3D(-x, -y, -z); }

	/**
	* Add two vectors together. Note this is slower than '+=' due to 
	* additional constructor and return.
	*/
	inline Vector3D operator + (const Vector3D &v) const noexcept
	{
		return Vector3D( x+v.x, y+v.y, z+v.z );
	}
	/**
	* Subtract two vectors together, use SSE if available. Note this is
	* slower than '-=' due to additional constructor and return.
	*/
	inline Vector3D operator - (const Vector3D &v) const noexcept
	{
		return Vector3D( x-v.x, y-v.y, z-v.z );
	}

    inline float GetLengthSquared() const noexcept { return x*x + y*y + z*z; }

	inline float AngleWith(Vector3D &v)
	{
		return (float)acos( (float)((*this) * v) / (this->GetLength() * v.GetLength()) );
	}

	void Cross(const Vector3D &v1, const Vector3D &v2);
	float GetLength() const noexcept;
    void Normalize() noexcept;

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
	Vector3D operator * (const Matrix4x4 &m) const;

};

#endif