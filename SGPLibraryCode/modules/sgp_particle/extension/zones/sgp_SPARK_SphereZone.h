#ifndef __SGP_SPARKSPHEREZONE_HEADER__
#define __SGP_SPARKSPHEREZONE_HEADER__

// A Zone defining a sphere in the universe

class SGP_API SphereZone : public Zone
{
	SPARK_IMPLEMENT_REGISTERABLE(SphereZone)

public:

	/**
	*  position : position of the center of the Sphere
	*  radius : radius of the Sphere
	*/
	SphereZone(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), float radius = 0.0f);


	static SphereZone* create(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), float radius = 0.0f);



	/**
	* Sets the radius of this Sphere
	* A negative radius will be clamped to 0.0f and the Sphere will therefore acts as a Point.
	*/
	void setRadius(float radius);
	float getRadius() const;

	virtual void setZoneScale( float scale );
	virtual void generatePosition(Particle& particle, bool full) const;
	virtual bool contains(const Vector3D& v) const;
	virtual bool intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const;
	virtual void moveAtBorder(Vector3D& v, bool inside) const;
	virtual Vector3D computeNormal(const Vector3D& point) const;

private:
	float radius;
};


inline SphereZone* SphereZone::create(const Vector3D& _position, float _radius)
{
	SphereZone* obj = new SphereZone(_position, _radius);
	registerObject(obj);
	return obj;
}
		
inline void SphereZone::setRadius(float radius)
{
	this->radius = radius >= 0.0f ? radius : 0.0f;
}

inline float SphereZone::getRadius() const
{
	return radius;
}

inline void SphereZone::setZoneScale( float scale )
{
	this->setRadius( this->getRadius() * scale );
}

#endif		// __SGP_SPARKSPHEREZONE_HEADER__