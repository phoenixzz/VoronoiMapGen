#ifndef __SGP_SPARKEMITTER_HEADER__
#define __SGP_SPARKEMITTER_HEADER__

class Group;

/**
* An abstract class that defines an emitter of particles

* An Emitter is an object that will launch particles by giving them a velocity and a position.
* the position is derived from the Zone of the Emitter.
* the velocity is derived from the Emitter itself.

* An Emitter has a flow and a tank of particles.
* The flow defines the rate at which particles are launched and the tank defines the total number of Particles the Emitter can launched.
* Note that the flow and the tank of an Emitter are only used when the Emitter emits automatically from a Group
* but the user can also emit manually outside a Group.
*/
class SGP_API Emitter : public Registerable, public Transformable
{
friend class Group;

public:
	Emitter();
	virtual ~Emitter() {}

	/**
	* Sets this Emitter active or not.

	* An inactive Emitter will not emit in its parent Group during update.
	* However it can still be used manually by the user.
	*/
	inline void setActive(bool active) { this->active = active; }

	inline bool isActive() const { return active; }

	/**
	* Sets the number of particles in this Emitter's tank

	* Each time the Emitter is updated, the number of particles emitted is deduced from the Emitter tank.
	* When the tank reaches 0, the Emitter will not emit any longer until it is refilled.

	* A number of -1 (or any negative number) means the Emitter has an infinite tank which will never be empty.
	*
	* IN param tank : the number of particles in this Emitters's tank
	*/
	inline void setTank(int32 tank) { this->tank = tank; }

	inline int32 getTank() const { return tank; }

	/**
	* Changes the number of particles in this Emitters's tank

	* The new number of particles in the tank is equal to : number of particles in the tank + n
	* This method has no effect for Emitters with infinite tank (a negative number of particles) and an Emitter cannot become infinite with this method (the new number is clamped to 0).

	* IN param deltaTank : the number to add to the current tank
	*/
	void changeTank(int32 deltaTank);


	/**
	* Sets the flow of this Emitter

	* The flow is in the unit : number of particle per step.
	* A flow of -1 (or any negative number) indicates an infinite flow which means all particles in the Emitters(s tank are generated instantly.
	* Note that if both the flow and the tank are infinite, nothing will happen.
	*/
	inline void setFlow(float flow) { this->flow = flow; }

	inline float getFlow() const { return flow; }

	/**
	* Changes the flow of particles of this Emitter

	* The new flow is equal to : flow of the Emitter + deltaFlow
	* This method has no effect for Emitters with infinite flow (a negative flow of particles) and an Emitter's flow cannot become infinite with this method (the new flow is clamped to 0).
	*/
	void changeFlow(float deltaFlow);

	/**
	* Sets the force of this Emitter
	*
	* The force of the Emitter vary for each launch of a Particle between a minimum and a maximum.
	* To have a fixed force for the Emitter, just have min = max

	* the speed at which a Particle will be launched is equal to : force / weight of the Particle.
	*
	*  min : the minimum force of the Emitter
	*  max : the maximum force of the Emitter
	*/
	inline void setForce(float min, float max)
	{
		forceMin = min;
		forceMax = max;
	}

	inline float getForceMin() const { return forceMin; }
	inline float getForceMax() const { return forceMax; }
	/**
	* Sets the Zone of this Emitter
	*
	* If the Zone is NULL, the default Zone will be used (A Point centered at the origin)
	*
	*  zone : the Zone of this Emitter
	*  full : true to generate particles in the whole Zone, false to generate particles only at the Zone borders.
	*/
	void setZone(Zone* zone, bool full = true);

	inline Zone* getZone() const { return zone; }

	// Tells whether this Emitter emits in the whole Zone or only at its borders
	inline bool isFullZone() const { return full; }

	/**
	* whether this Emitter is sleeping or not
	* An Emitter is considered as sleeping if his flow or his tank is equal to 0.
	*/
	inline bool isSleeping() const { return ((tank == 0)||(flow == 0.0f)); }


	/**
	* Emits a Particle from this Emitter
	*
	* The Particle's velocity is updated with a call to generateVelocity(Particle&).
	* The Particle's position is updated with a call to Zone::generatePosition(Particle&) of the Emitter's Zone.
	*
	* Note that this will not decrease the number of particles in the Emitter's tank.
	* To do it, the user has to manually make a call to changeNumber(-1) after this call.
	*
	*  particle : the Particle to emit from this Emitter
	*/
	inline void emit(Particle& particle) const
	{
		zone->generatePosition(particle, full);
		generateVelocity(particle);
	}

	/**
	* Generates the velocity of the Particle
	*
	* The velocity of the Particle is updated in function of the Emitter's nature and parameters.
	* Unlike emit() the position of the Particle remains unchanged.
	*
	*  particle : the Particle whose velocity has to be updated
	*/
	inline void generateVelocity(Particle& particle) const
	{
		generateVelocity(particle, random(forceMin, forceMax) / particle.getParamCurrentValue(PARAM_MASS));
	}

	virtual Registerable* findByName(const String& name);

protected:
	virtual void registerChildren(bool registerAll);
	virtual void copyChildren(const Registerable& object, bool createBase);
	virtual void destroyChildren(bool keepChildren);

	virtual void propagateUpdateTransform() 
	{
		zone->updateTransform(this);
	}

private:
	Zone* zone;
	bool full;

	bool active;

	int32 tank;
	float flow;

	float forceMin;
	float forceMax;

	mutable float fraction;

	static Zone& getDefaultZone();

	uint32 updateNumber(float deltaTime);

	/**
	* A pure virtual method that generates the velocity of the Particle in function of a speed
	*
	* This is a pure virtual method to be implemented by children.

	* the Particle velocity has to be set by this method.
	* the generated velocity of the Particle must have a norm equal to speed.
	*
	* @param particle : the Particle whose velocity has to be generated
	* @param speed : the speed that the velocity must have
	*/
	virtual void generateVelocity(Particle& particle, float speed) const = 0;

};

#endif		// __SGP_SPARKEMITTER_HEADER__