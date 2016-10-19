#ifndef __SGP_VECTOR2D_HEADER__
#define __SGP_VECTOR2D_HEADER__

//==============================================================================
/**
    A two-coordinate vector.
*/

class Vector2D
{
public:
	float x, y;

public:
	//! Default constructor (null vector)
	Vector2D() noexcept : x(0), y(0) {}
	//! Constructor with two different values
	Vector2D(float nx, float ny) noexcept : x(nx), y(ny) {}
	//! Copy constructor
	Vector2D(const Vector2D& other) noexcept : x(other.x), y(other.y) {}
    Vector2D& operator = (const Vector2D& other) noexcept { x = other.x; y = other.y; return *this; }

	inline void Set(float _x, float _y) noexcept { x=_x; y=_y; }
	inline void Set(const Vector2D &v) noexcept { x=v.x; y=v.y; }


	/**
	 * Add two vectors together. Note this is faster than '+' due to lack
	 * of additional constructor and return.
	 */
	inline void operator += (const Vector2D &v)	noexcept { x += v.x; y += v.y; }
	/**
	* Subtract two vectors together. Note this is faster than '-' due to lack
	* of additional constructor and return.
	*/
	inline void operator -= (const Vector2D &v) noexcept { x -= v.x; y -= v.y; }
	// scale vector
	inline void operator *= (float f) noexcept {	x *= f; y *= f; }
	// scale down
	inline void operator /= (float f) noexcept { x /= f; y /= f; }	
	// add scalar
	inline void operator += (float f) noexcept { x += f; y += f; }	
	// subtract scalar
	inline void operator -= (float f) noexcept { x -= f; y -= f; }		
	// dot product
	inline float operator * (const Vector2D &v) const noexcept { return (v.x*x + v.y*y); }	
	// scale vector
	inline Vector2D		operator * (float f) const noexcept { return Vector2D(x*f, y*f); }
	// scalar divide
	inline Vector2D		operator / (float f) const noexcept { return Vector2D(x/f, y/f); }
	// add scalar
	inline Vector2D		operator + (float f) const noexcept { return Vector2D(x+f, y+f); }	
	// subtract scale
	inline Vector2D		operator - (float f) const noexcept { return Vector2D(x-f, y-f); }		
	// Negative
	inline Vector2D		operator - () const noexcept { return Vector2D(-x, -y); }

	/**
	* Add two vectors together. Note this is slower than '+=' due to 
	* additional constructor and return.
	*/
	inline Vector2D operator + (const Vector2D &v) const noexcept
	{
		return Vector2D( x+v.x, y+v.y );
	}
	/**
	* Subtract two vectors together, use SSE if available. Note this is
	* slower than '-=' due to additional constructor and return.
	*/
	inline Vector2D operator - (const Vector2D &v) const noexcept
	{
		return Vector2D( x-v.x, y-v.y );
	}

    inline float GetLengthSquared() const noexcept { return x * x + y * y; }
    inline float GetLength() const noexcept { return std::sqrt(GetLengthSquared()); }

    inline void Normalize() noexcept
	{ 
		float f = GetLength();
		if( f != 0 ) { x /= f; y /=f; }
	}
};

#endif