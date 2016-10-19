#ifndef __SGP_SPARKAABBOXZONE_HEADER__
#define __SGP_SPARKAABBOXZONE_HEADER__

/**
*  A Zone defining an axis-aligned Box
* An AABoxZone is defined by its center and a dimension in each axis.
*/
class SGP_API AABoxZone : public Zone
{
	SPARK_IMPLEMENT_REGISTERABLE(AABoxZone)

public:

	AABoxZone(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), const Vector3D& dimension = Vector3D(0.0f,0.0f,0.0f));

	static AABoxZone* create(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), const Vector3D& dimension = Vector3D(0.0f,0.0f,0.0f));

	/**
	* Sets the dimensions of this AABox
	* The negative dimensions are clamped to 0.
	* An AABox with 0 as its 3 dimensions is equivalent to a Point
	*/
	void setDimension(const Vector3D& dimension);


	/**
	* Gets the dimensions of this AABox
	*/
	const Vector3D& getDimension() const;


	virtual void setZoneScale( float scale );
	virtual void generatePosition(Particle& particle, bool full) const;
	virtual bool contains(const Vector3D& v) const;
	virtual bool intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const;
	virtual void moveAtBorder(Vector3D& v, bool inside) const;
	virtual Vector3D computeNormal(const Vector3D& point) const;

private:
	Vector3D dimension;

	bool slabIntersects(float p0, float p1, float bMin, float bMax, float& tEnter, float& tExit, int& firstAxis, int axis) const;
};


inline AABoxZone* AABoxZone::create(const Vector3D& _position, const Vector3D& _dimension)
{
	AABoxZone* obj = new AABoxZone(_position, _dimension);
	registerObject(obj);
	return obj;
}

inline const Vector3D& AABoxZone::getDimension() const
{
	return dimension;
}

inline bool AABoxZone::slabIntersects(float p0, float p1, float bMin, float bMax, float& tEnter, float& tExit, int& firstAxis, int axis) const
{
	float dir = p1 - p0;

	if( dir == 0.0f )
	{
		if( (p0 < bMin) || (p0 > bMax) )
			return false;
		return true;
	}

	float t0 = (bMin - p0) / dir;
	float t1 = (bMax - p0) / dir;

	if(t0 > t1)
	{
		sgp::swapVariables(t0, t1);
		axis += 3;
	}

	if( (t1 < tEnter) || (t0 > tExit) )
		return false;

	if(t0 > tEnter)
	{
		tEnter = t0;
		firstAxis = (firstAxis & 0xF0) | (axis & 0x0F);
	}

	if(t1 < tExit)
	{
		tExit = t1;
		firstAxis = (firstAxis & 0x0F) | ((axis << 4) & 0xF0);
	}

	return true;
}

inline void AABoxZone::setZoneScale( float scale )
{
	setDimension( getDimension() * scale );
}

#endif		// __SGP_SPARKAABBOXZONE_HEADER__