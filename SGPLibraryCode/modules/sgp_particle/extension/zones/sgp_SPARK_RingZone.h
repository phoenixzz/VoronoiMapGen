#ifndef __SGP_SPARKRINGZONE_HEADER__
#define __SGP_SPARKRINGZONE_HEADER__

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

/**
* A Zone defining a flat ring in the universe

* A ring is defined by :	
	The position of its center
	The normal of the plane on which it lies
	A minimum and maximum radius
	
* Note that by having the minimum radius equal to 0, the ring becomes a disk in the universe.
* Note that the normal does not have to be normalized as it is normalized internally when set.
*/
class SGP_API RingZone : public Zone
{
	SPARK_IMPLEMENT_REGISTERABLE(RingZone)

public:
	/**
	* Constructor of ring
	*  position : the position of the ring
	*  normal : the normal of the plane on which lies the ring
	*  minRadius : the minimum radius of the ring
	*  maxRadius : the maximum radius of the ring
	*/
	RingZone(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), const Vector3D& normal = Vector3D(0.0f,1.0f,0.0f), float minRadius = 0.0f, float maxRadius = 1.0f);


	static RingZone* create(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), const Vector3D& normal = Vector3D(0.0f,1.0f,0.0f), float minRadius = 0.0f, float maxRadius = 1.0f);



	// the normal does not have to be normalized
	// Note that the normal is normalized internally
	void setNormal(const Vector3D& normal);

	/**
	* Sets the min and max radius of this ring
	* 
	* A radius cannot be negative.
	* Note that negative radius are inverted internally
	*/
	void setRadius(float minRadius, float maxRadius);


	const Vector3D& getNormal() const;
	const Vector3D& getTransformedNormal() const;


	float getMinRadius() const;
	float getMaxRadius() const;


	virtual void setZoneScale( float scale );
	virtual void generatePosition(Particle& particle, bool full) const;
	virtual bool contains(const Vector3D& v) const;
	virtual bool intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const;
	virtual void moveAtBorder(Vector3D& v, bool inside) const;
	virtual Vector3D computeNormal(const Vector3D& point) const;

protected:
	virtual void innerUpdateTransform();

private:
	Vector3D normal;
	Vector3D tNormal;

	float minRadius;
	float maxRadius;

	// Square of the radius (for optimization purpose)
	float sqrMinRadius;
	float sqrMaxRadius;
};

	
inline RingZone* RingZone::create(const Vector3D& _position, const Vector3D& _normal, float _minRadius, float _maxRadius)
{
	RingZone* obj = new RingZone(_position, _normal, _minRadius, _maxRadius);
	registerObject(obj);
	return obj;
}
		
inline void RingZone::setNormal(const Vector3D& normal)
{
	this->normal = normal;
	this->normal.Normalize();
	tNormal = this->normal;
	notifyForUpdate();
}

inline const Vector3D& RingZone::getNormal() const
{
	return normal;
}

inline const Vector3D& RingZone::getTransformedNormal() const
{
	return tNormal;
}

inline float RingZone::getMinRadius() const
{
	return minRadius;
}

inline float RingZone::getMaxRadius() const
{
	return maxRadius;
}

inline bool RingZone::contains(const Vector3D& v) const
{
	return false;
}

inline Vector3D RingZone::computeNormal(const Vector3D& point) const
{
	return tNormal;
}

inline void RingZone::setZoneScale( float scale )
{
	this->setRadius( this->getMinRadius() * scale, this->getMaxRadius() * scale );
}

#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKRINGZONE_HEADER__