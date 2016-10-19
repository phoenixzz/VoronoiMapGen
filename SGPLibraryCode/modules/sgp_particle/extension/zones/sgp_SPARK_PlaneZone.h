#ifndef __SGP_SPARKPLANEZONE_HEADER__
#define __SGP_SPARKPLANEZONE_HEADER__

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

/**
* A Zone defining a plane in the universe
*
* A Plane is defined by 2 parameters :
	A Position is the universe
	A Normal

* The defined Plane is the Plane having the normal and passing by the position.
* The direction of the normal defines the sens of the plane. The normal goes from inside the Plane to outside.
* 
* Note that the normal does not have to be normalized as it is normalized internally when set.
*/
class SGP_API PlaneZone : public Zone
{
	SPARK_IMPLEMENT_REGISTERABLE(PlaneZone)

public:



	/**
	* Constructor of Plane
	*  position : the position of the Plane
	*  normal : the normal of the Plane (0,1,0) stand for Y Axis is UP
	*/
	PlaneZone(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), const Vector3D& normal = Vector3D(0.0f,1.0f,0.0f));
	
	static PlaneZone* create(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f), const Vector3D& normal = Vector3D(0.0f,1.0f,0.0f));

	// the normal does not have to be normalized
	// because the normal is normalized internally.
	void setNormal(const Vector3D& normal);
	const Vector3D& getNormal() const;

	// Gets the transformed normal of this Plane
	const Vector3D& getTransformedNormal() const;


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
};


inline PlaneZone* PlaneZone::create(const Vector3D& _position, const Vector3D& _normal)
{
	PlaneZone* obj = new PlaneZone(_position, _normal);
	registerObject(obj);
	return obj;
}
		
inline void PlaneZone::setNormal(const Vector3D& _normal)
{
	this->normal = _normal;
	this->normal.Normalize();
	tNormal = this->normal;
	notifyForUpdate();
}

inline const Vector3D& PlaneZone::getNormal() const
{
	return normal;
}

inline const Vector3D& PlaneZone::getTransformedNormal() const
{
	return tNormal;
}

inline void PlaneZone::generatePosition(Particle& particle, bool full) const
{
	particle.position() = getTransformedPosition();
}

inline bool PlaneZone::contains(const Vector3D& v) const
{
	return (normal * (v - getTransformedPosition())) <= 0.0f;
}

inline Vector3D PlaneZone::computeNormal(const Vector3D& point) const
{
	return tNormal;
}

inline void PlaneZone::setZoneScale( float scale ) {}


#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKPLANEZONE_HEADER__