#ifndef __SGP_SPARKSYSTEM_HEADER__
#define __SGP_SPARKSYSTEM_HEADER__

// Enumeration defining how to handle the step time of particle systems
enum StepMode
{
	STEP_REAL,			/**< The step time is the deltatime passed by the user */
	STEP_CONSTANT,		/**< The step time is a constant time therefore 0 to many updates may occur in a call */
	STEP_ADAPTIVE,		/**< The step time is a range between 2 values therefore 0 to many updates may occur in a call */
};

/**
* This class defines a whole system of particles. It contains particle groups.
* It offers a way to handle a system very easily.
* Basically a particle system is updated by calling update(uint32) and renderered with render() at each frame.

* Note that a System is only a helper class to manage many Groups. However it can be omitted and groups can be updated and rendered alone.

* A System is transformable. If the system is transformed, all its sub emitters will be transformed as well. However, its modifiers will not
* (modifiers can be described in the world coordinates already). If you wish to transform some of its modifiers as well, you will have to do it aside.
*/
class SGP_API System : public Registerable, public Transformable
{
	SPARK_IMPLEMENT_REGISTERABLE(System)

public:
	System();
	virtual ~System() {}
	static System* create();

	/**
	* Sets the camera position

	* Note that the camera position is only useful if a group has to be sorted.
	* In that case this vector will be used as the camera position to derive the distance between the particle and the camera position.
	* The camera position has to be updated before an update of the sorted group.
	*/
	static void setCameraPosition(const Vector3D& cameraPosition);
	static const Vector3D& getCameraPosition();

	/**
	* Enables or not the clamping on the deltaTime when updating systems

	* This allows to limit too big deltaTime which may spoil your particle systems.
	* Basically if the deltaTime is higher than the clamp value, the clamp calue is used as the deltaTime.

	* It allows in real step mode to avoid having too big deltaTimes and in the other 2 modes to avoid having too
	* many updates that may slow down the application.
	* Note that setting the clamp value too low may slow down your systems
	*
	* useClampStep : true to use a clamp value on the step, false not to
	* clamp : the clamp value
	*/
	static void setClampStep(bool useClampStep, float clamp = 1.0f);

	/**
	* Uses a constant step to update the systems

	* This tells the system to be updated with a constant time.
	* Depending on the deltaTime passed for the update, 0 to many updates can occur
	* For example if the delta time is 1.0 and the constant step is 0.1 then 10 updates of time 0.1 will occur.

	* This mode is useful when the update must be constant (accurate collisions...) but be aware it can be very computationnaly intensive.
	* 
	*/
	static void useConstantStep(float constantStep);

	/**
	* Uses an adaptive step to update the systems

	* This tells the system to be updated with a time between min and max.
	* If the deltaTime passed is higher than maxStep or lower than minStep then this mode operates like the constant step mode with
	* either constant time being maxStep or minStep (respectivally).
	* If the deltaTime lies between minStep and maxStep then this mode performs like the real step mode.

	* This mode is a good alternative between the other two.
	* Combined with the clamp step, it allows to correctly handle the step time without being to much frame rate dependant.

	*/
	static void useAdaptiveStep(float minStep, float maxStep);

	/**
	* Uses the real step to update the systems

	* This is the basic mode (and the mode per default) to update the systems.
	* One call to update means one update of time deltaTime.

	* This mode is the simpler and the one that allows best performance on low end systems.
	* However the update may be inaccurate (due to too big deltaTime) and it performs badly with frame rate variation.
	*
	*/
	static void useRealStep();

	// Gets the current step mode
	static StepMode getStepMode();


	/**
	* Enables or disables the computation of the axis aligned Vector for this System

	* Enabling the computation of the AABB for the System only takes the AABB of all AABB of the Groups within the System where AABB computation is enabled.
	* see Group::enableAABBComputing(bool) for more details.
	*/
	void enableAABBComputing(bool AABB);
	bool isAABBComputingEnabled() const;
	const AABBox& getBoundingBox() const;

	/**
	* Gets the number of active particles in this system
	*
	* The number of active particles in the system is the addition of the number of active particles in each group of the system.
	* Note that the number of active particle of the system is updated after each update of the system.
	* This means if the user changes manually the number of particles in a group and call this method before an update, the number returned will not be up to date.
	* To compute and get the real number of active particles in the System, see computeNumOfParticles().
	*/
	uint32 getNumOfParticles() const;

	/**
	* Computes the number of active particles in this System and returns it
	*
	* Unlike getNumOfParticles() which returns the last number of particles computed (after a call to update(float) or empty()),
	* this method recomputes the current number of active particles by parsing all the groups of this System.
	* In that way, this method must not be used as an accessor but call once when necesseray between 2 updates.

	* Note that this method updates the inner number of particles of this System, which means a call to getNumOfParticles() will
	* then return the computed number.
	*
	*/
	uint32 computeNumOfParticles();

	/**
	* Gets the number of groups in the System
	*/
	uint32 getNumOfGroups() const;

	/**
	* Gets the vector of the groups (pointers) in this System
	*/
	const Array<Group*>& getGroups() const;
	Group* getGroup(int index);



	/* Adds a Group to the System */
	void addGroup(Group* group);

	/* Removes a Group from the System */
	void removeGroup(Group* group);

	/**
	* Updates the System of the current time step
	* Note that this method updates all groups in the System from first to last.

	* return true if the System is still active (has active groups)
	*/
	virtual bool update(float deltaTimeInSecond);

	/**
	* Renders particles in the System
	* this method renders all groups in the System from first to last.
	*/
	virtual void render() const;

	/**
	* Makes this System grow to the given time
	*
	* This method is useful to get a newly created System to a mature state.
	* This method only calls update(float) with the step until the total update time reaches the time.
	*
	* @param time : the total time of which to update this System
	* @param step : the time the System is updated at each call to update(float)
	*/
	void grow(float time, float step);

	/**
	* Empties the System
	*
	* This method will make all particles in the System inactive.
	* However all connections are kept which means groups are still in theSystem.
	*/
	void empty();

	/**
	* Sorts the particles in all the group of this System where the sorting is enabled
	* Note that the sorting is also performed during the update.
	* This method is therefore only useful when the camera position changes several times between 2 updates.
	* This method calls the Group::sortParticles() of each Group in this System.
	*/
	void sortParticles();

	/**
	* Computes the distances between each Particle in each Group of this System

	* Note that the distances computation is also performed during the update.
	* This method is therefore only useful when the camera position changes several times between 2 updates.

	* This methods calls the Group::computeDistances() of each Group in this System.
	*/
	void computeDistances();

	/**
	* Computes the bounding box of this System and of all groups in the System
	*
	* The bounding box of the System is only computed if the System has its bounding box computing enabled.
	* In the same way, the bounding box of a Group within the System is only computed if the Group has its bounding box computing enabled.

	* Note that the computation of bounding boxes is also performed during the update.
	* This method is therefore only useful when the bounding boxes have to be recomputed between 2 updates.

	* This methods calls the Group::computeAABB() of each Group in this System.
	*/
	void computeAABB();

	virtual Registerable* findByName(const String& name);

protected:
	Array<Group*> groups;

	virtual void registerChildren(bool registerAll);
	virtual void copyChildren(const Registerable& object, bool keepChildren);
	virtual void destroyChildren(bool createBase);

	virtual void propagateUpdateTransform();

private:
	static Vector3D cameraPosition;

	static StepMode stepMode;

	static float constantStep;
	static float minStep;
	static float maxStep;

	static bool clampStepEnabled;
	static float clampStep;

	float deltaStep;

	uint32 NumberOfParticles;

	bool boundingBoxEnabled;

	AABBox boundingBox;

	bool innerUpdate(float deltaTimeInSecond);

};

inline System* System::create()
{
	System* obj = new System;
	registerObject(obj);
	return obj;
}
	
inline void System::enableAABBComputing(bool bAABBComputing)
{
	boundingBoxEnabled = bAABBComputing;
}

inline uint32 System::getNumOfParticles() const
{
	return NumberOfParticles;
}

inline uint32 System::getNumOfGroups() const
{
	return groups.size();
}

inline const Array<Group*>& System::getGroups() const
{
	return groups;
}

inline Group* System::getGroup(int index)
{
	return groups[index];
}

inline bool System::isAABBComputingEnabled() const
{
	return boundingBoxEnabled;
}

inline const AABBox& System::getBoundingBox() const
{
	return boundingBox;
}


#endif		// __SGP_SPARKSYSTEM_HEADER__