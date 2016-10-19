#ifndef __SGP_SPARKPOINTMASS_HEADER__
#define __SGP_SPARKPOINTMASS_HEADER__


/**

* A Modifier defining a point with a mass that attracts or repulses particles
*
* A PointMass triggered on a Particle will affect its velocity as followed :
		dist = pointMassPosition - particlePosition
		particleVelocity += dist * mass * step / max(minDistance,|dist|)
*/
class SGP_API PointMass : public Modifier
{
	SPARK_IMPLEMENT_REGISTERABLE(PointMass)

public:


	/**
	* Constructor of PointMass
	*	zone : the Zone of the PointMass
	*	trigger : the trigger of the PointMass
	*	mass : the mass of the PointMass
	*	minDistance : the minimum distance of the PointMass
	*/
	PointMass(Zone* zone = NULL, ModifierTrigger trigger = INSIDE_ZONE, float mass = 1.0f, float minDistance = 0.05f);

	static PointMass* create(Zone* zone = NULL, ModifierTrigger trigger = INSIDE_ZONE, float mass = 1.0f, float minDistance = 0.05f);

	/////////////
	// Setters //
	/////////////


	// Sets the delta position from the position of the zone (or origin if no zone set)
	void setPosition(const Vector3D& pos);

	/**
	* Sets the mass of this PointMass
	*
	* The mass defines the strenght of the attraction. The more the mass, the stronger the attraction.
	* A position mass will result into an attraction while a negative mass will result into a repulsion.
	* Moreover a mass equal to 0 make the PointMass have no effect.
	*/
	void setMass(float mass);

	/**
	*  Sets the minimum distance of this PointMass
	*
	* The minimum distance of the PointMass is the minimum distance that can be considered to compute the force implied by the PointMass.
	* If a distance between a Particle and a PointMass is inferior to the minimum distance of the PointMass, the distance is clamped to the minimum distance.
	* This avoids forces that approaching the infinity with Particle getting very close to the PointMass.
	*/
	void setMinDistance(float minDistance);


	const Vector3D& getPosition() const;
	const Vector3D& getTransformedPosition() const;


	float getMass() const;
	float getMinDistance() const;

protected:
	virtual void innerUpdateTransform();

private:
	Vector3D position;
	Vector3D tPosition;

	float mass;
	float minDistance;
	float sqrMinDistance;

	virtual void modify(Particle& particle, float deltaTime) const;
};


inline PointMass* PointMass::create(Zone* _zone, ModifierTrigger _trigger, float _mass, float _minDistance)
{
	PointMass* obj = new PointMass(_zone, _trigger, _mass, _minDistance);
	registerObject(obj);
	return obj;
}
		
inline void PointMass::setPosition(const Vector3D& pos)
{
	position = tPosition = pos;
	notifyForUpdate();
}

inline void PointMass::setMass(float _mass)
{
	this->mass = _mass;
}

inline void PointMass::setMinDistance(float _minDistance)
{
	this->minDistance = _minDistance;
	sqrMinDistance = _minDistance * _minDistance;
}

inline const Vector3D& PointMass::getPosition() const
{
	return position;
}

inline const Vector3D& PointMass::getTransformedPosition() const
{
	return tPosition;
}

inline float PointMass::getMass() const
{
	return mass;
}

inline float PointMass::getMinDistance() const
{
	return minDistance;
}

inline void PointMass::innerUpdateTransform()
{
	Modifier::innerUpdateTransform();
	// the delta position is actually a direction not a position
	tPosition = position;
	tPosition.RotateWith(getWorldTransform());
}

#endif		// __SGP_SPARKPOINTMASS_HEADER__