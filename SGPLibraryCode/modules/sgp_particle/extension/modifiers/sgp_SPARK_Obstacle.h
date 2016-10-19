#ifndef __SGP_SPARKOBSTACLE_HEADER__
#define __SGP_SPARKOBSTACLE_HEADER__

// A Modifier that acts as an obstacle on particles
class SGP_API Obstacle : public Modifier
{
	SPARK_IMPLEMENT_REGISTERABLE(Obstacle)

public:


	/**
	* Constructor of Obstacle
	*		 zone : the Zone of the Obstacle
	*		 trigger : the trigger of the Destructor
	*		 bouncingRatio : the bouncingRatio of the Obstacle
	*		 friction : the friction of the Obstacle
	*/
	Obstacle(Zone* zone = NULL, ModifierTrigger trigger = INTERSECT_ZONE, float bouncingRatio = 1.0f, float friction = 1.0f);


	static Obstacle* create(Zone* zone = NULL, ModifierTrigger trigger = INTERSECT_ZONE, float bouncingRatio = 1.0f, float friction = 1.0f);



	/**
	* Sets the bouncing ratio of this Obstacle
	*
	* The bouncing ratio is the multiplier applied to the normal component of the rebound.
	*/
	void setBouncingRatio(float bouncingRatio);

	/**
	* Sets the friction of this Obstacle
	*
	* The bouncing ratio is the multiplier applied to the tangent component of the rebound.
	*/
	void setFriction(float friction);


	float getBouncingRatio() const;
	float getFriction() const;

private:

	float bouncingRatio;
	float friction;

	virtual void modify(Particle& particle, float deltaTime) const;
	virtual void modifyWrongSide(Particle& particle, bool inside) const;
};


inline Obstacle* Obstacle::create(Zone* _zone, ModifierTrigger _trigger, float _bouncingRatio, float _friction)
{
	Obstacle* obj = new Obstacle(_zone, _trigger, _bouncingRatio, _friction);
	registerObject(obj);
	return obj;
}
		
inline void Obstacle::setBouncingRatio(float bouncingRatio)
{
	this->bouncingRatio = bouncingRatio < 0.0f ? 0.0f : bouncingRatio;
}

inline void Obstacle::setFriction(float friction)
{
	this->friction = friction;
}

inline float Obstacle::getBouncingRatio() const
{
	return bouncingRatio;
}

inline float Obstacle::getFriction() const
{
	return friction;
}

inline void Obstacle::modifyWrongSide(Particle& particle, bool inside) const
{
	if( isFullZone() )
		getZone()->moveAtBorder(particle.position(), inside);
}


#endif		// __SGP_SPARKOBSTACLE_HEADER__