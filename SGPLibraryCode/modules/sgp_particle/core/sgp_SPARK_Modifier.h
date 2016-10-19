#ifndef __SGP_SPARKMODIFIER_HEADER__
#define __SGP_SPARKMODIFIER_HEADER__

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

class ModifierGroup;


// Constants defining the triggers for Modifier
enum ModifierTrigger
{
	ALWAYS = 1 << 0,			/**< No trigger, a Particle is always modified */
	INSIDE_ZONE = 1 << 1,		/**< Trigger defining a Particle inside the Zone */
	OUTSIDE_ZONE = 1 << 2,		/**< Trigger defining a Particle outside the Zone */
	INTERSECT_ZONE = 1 << 3,	/**< Trigger defining a Particle intersecting the Zone (in any direction) */
	ENTER_ZONE = 1 << 4,		/**< Trigger defining a Particle entering the Zone */
	EXIT_ZONE = 1 << 5,			/**< Trigger defining a Particle exiting the Zone */
};

/**
* A abstract class that defines a physical object acting on particles
*
* A Modifier is first defined by a Zone and a trigger to that Zone.
* If the Particle triggers the Modifier, the Modifier's action is applied to the Particle.
* An action can be anything that has effect on the Particle's parameters, position, velocity, life...

* If no Zone is attached to a Modifier the Zone is considered to be the entire universe.

* Like an Emitter, a Modifier can either be used automatically within a Group or manually directly by the user.
*/
class SGP_API Modifier : public Registerable,
						 public Transformable,
						 public BufferHandler
{
	friend class ModifierGroup;
	friend class Group;
	friend class Particle;

public:
	/**
	* Constructor of Modifier
	* @param availableTriggers : the available triggers for this Modifier (OR-ed)
	* @param trigger : the default trigger of the Modifier
	* @param needsIntersection : true if the computation of the intersection is needed, false otherwise
	* @param needsNormal : true if the computation of the normal is needed, false otherwise
	* @param zone : the Zone of the Modifier
	*/
	Modifier(uint32 availableTriggers = ALWAYS, ModifierTrigger trigger = ALWAYS, bool needsIntersection = false, bool needsNormal = false, Zone* zone = NULL);
	virtual ~Modifier() {}

	/**
	* Sets this Modifier active or not.
	*
	* An inactive Modifier will not affect its parent Group during update.
	* However it can still be used manually by the user.
	*/
	inline void setActive(bool active) { this->active = active; }
	inline bool isActive() const { return active; }

	/**
	* Sets the Zone of this Modifier
	* If the Zone is NULL, the Zone is considered to be the entire universe.
	* @param zone : the Zone of this Modifier
	* @param full : true to consider the Zone as a full object so that particles are not allowed to be within
	*/
	void setZone(Zone* zone, bool full = false);
	inline Zone* getZone() const { return zone; }
	// whether the Zone of this Modifier is considerered to be full or not
	inline bool isFullZone() const { return full; }

	/**
	* Sets the trigger of this Modifier
	*
	* if the trigger is not one of the available triggers (see getAvailableTriggers()) for this Modifier,
	* nothing happens and false is returned else the trigger is set and true is returned.
	*
	* @param trigger : the trigger of this Modifier
	* @return true if the trigger can be set, false otherwise
	*/
	bool setTrigger(ModifierTrigger trigger);
	inline ModifierTrigger getTrigger() const { return trigger; }
	// Gets a flag containing all the available triggers for this Modifier
	inline uint32 getAvailableTriggers() const { return availableTriggers; }

	/**
	* Sets whether to consider this modifier local to a system or not
	*
	* A local modifier is transformed when its system is transformed, a non local one will not.
	*/
	inline void setLocalToSystem(bool local) { this->local = local; }
	// whether this modifier is considered as being local to a system or not
	inline bool isLocalToSystem() const { return local; }

	virtual Registerable* findByName(const String& name);

protected:
	/** the Vector3D that holds the intersection coordinates */
	static Vector3D intersection;

	/** the Vector3D that holds the intersection coordinates */
	static Vector3D normal;

	/** true if the Modifier needs the intersection computation, false if not */
	bool needsIntersection;

	/** true if the Modifier needs the normal computation, false if not */
	bool needsNormal;

	/** the current trigger of this Modifier */
	ModifierTrigger trigger;

	/** a flag containing all the available triggers */
	const uint32 availableTriggers;

	virtual void registerChildren(bool registerAll);
	virtual void copyChildren(const Registerable& object, bool createBase);
	virtual void destroyChildren(bool keepChildren);

	virtual void propagateUpdateTransform();

private:
	Zone* zone;
	bool full;

	bool active;
	mutable bool savedActive;

	bool local;

	void beginProcess(Group& group);
	void endProcess(Group& group);
	void process(Particle& particle, float deltaTime) const;

	/**
	* A pure virtual method that modifies the Particle
	*
	* The Modifier on this Particle has already been triggered and the Particle must be modified by this method.
	*/
	virtual void modify(Particle& particle, float deltaTime) const = 0;
	
	/**
	* A pure virtual method that handles particles on the wrong side of this Modifier Zone.

	* This method can be called internally with all triggers except SPK::TRIGGER_INTERSECTS.

	* The method isFullZone() can be called to vary the behavior whether the Zone is full or not.
	* The boolean inside indicates whether the wrong side is inside (true) or outside (false) the Zone.
	*
	* @param particle : the Particle which is on the wrong side
	* @param inside : true if the wrong side is inside, false if it is oustside
	*/
	virtual void modifyWrongSide(Particle& particle, bool inside) const {}

};

inline void Modifier::propagateUpdateTransform()
{
	if( zone )
		zone->updateTransform(this);
}

inline void Modifier::endProcess(Group& group)
{
	active = savedActive; // Restores the active state of the modifier
}

inline void Modifier::process(Particle& particle, float deltaTime) const
{
	switch(trigger)
	{
	case ALWAYS :
		modify(particle, deltaTime);
		break;

	case INSIDE_ZONE :
		if((zone == NULL) || (zone->contains(particle.position())))
			modify(particle, deltaTime);
		else
			modifyWrongSide(particle, true);
		break;

	case OUTSIDE_ZONE :
		if( !zone )
			return;
		if( !zone->contains(particle.position()) )
			modify(particle, deltaTime);
		else
			modifyWrongSide(particle, false);
		break;

	case INTERSECT_ZONE :
		if( !zone )
			return;
		if( zone->intersects( 
								particle.oldPosition(),
								particle.position(),
								needsIntersection ? &intersection : NULL,
								needsNormal ? &normal : NULL) )
			modify(particle, deltaTime);
		break;

	case ENTER_ZONE :
		if( !zone )
			return;
		if( zone->contains(particle.oldPosition()) )
			modifyWrongSide(particle, true);
		else if( zone->intersects(
									particle.oldPosition(),
									particle.position(),
									needsIntersection ? &intersection : NULL,
									needsNormal ? &normal : NULL) )
			modify(particle, deltaTime);
		break;

	case EXIT_ZONE :
		if( !zone )
			return;
		if( !zone->contains(particle.oldPosition()) )
			modifyWrongSide(particle, false);
		else if( zone->intersects(
									particle.oldPosition(),
									particle.position(),
									needsIntersection ? &intersection : NULL,
									needsNormal ? &normal : NULL) )
			modify(particle, deltaTime);
		break;
	}
}

#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKMODIFIER_HEADER__