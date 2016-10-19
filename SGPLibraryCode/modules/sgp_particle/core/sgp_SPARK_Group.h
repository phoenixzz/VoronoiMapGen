#ifndef __SGP_SPARKGROUP_HEADER__
#define __SGP_SPARKGROUP_HEADER__

/**
* A group of many particles
*
* A Group is the structure the user will interact with the most to build up a full Particle System.
* More than only storing many particles, a Group also defines and entire environment for Particle generation and Evolution.

*/
class SGP_API Group : public Registerable, public Transformable
{
	friend class Renderer;
	friend class Particle;
	friend void swapParticles(Particle& a, Particle& b);

	SPARK_IMPLEMENT_REGISTERABLE(Group)

public:
	/**
	* Constructor for a Group
	*
	* A Group is constructed with a Model (if NULL, the default Model will be used to generate particles).
	* This Model will be used to handle Particle's generation and evolution within the Group.

	* A Group also needs a maximum capacity which is the maximum number of particles the Group can handle. This is necessary to reserve some memory space.
	* Note that the capacity can be changed by calling reallocate(uint32).
	* By default, the capacity is set to Pool::DEFAULT_CAPACITY.
	*
	*/
	Group(Model* model = NULL, uint32 capacity = Pool<Particle>::DEFAULT_CAPACITY);

	Group(const Group& group);

	/**
	* Creates and registers a new Group
	*/
	static Group* create(Model* model = NULL, uint32 capacity = Pool<Particle>::DEFAULT_CAPACITY);

	~Group();

	/**
	* change the Model of this Group
	* If the model parameter is set to NULL, the default model will be used. No changes are done if the model parameter is equal
	* to the Model of this Group. Changing the Model of this Group will empty it.
	*/
	void setModel(Model* model);

	/**
	* Sets the Renderer of this Group
	*
	* If the Renderer is set to NULL, the particles of the Group will not be renderered with a call to render().
	* Note that if the bufferManagement is on (see enableBuffersManagement(bool)), setting the Renderer will first
	* destroys the buffers needed for the previous Renderer held by this Group and create buffers needed for the new Renderer.
	*/
	void setRenderer(Renderer* renderer);

	/**
	* Sets the friction of this Group
	* The friction defines the way particles are accelerated or decelerated in their environment.

		If the friction is 0.0f, particles in the Group are neither accelerated nor decelerated (it is the default setting).
		If the friction is positive, particles will be decelerated function of their speed.
		If the friction is negative, particles will be accelerated function of their speed.

	The friction is applied on each Particle as followed :
		velocity *= 1 - min(1, friction * deltaTime / weight)

	* Note that the lighter the Particle, the more effect has the friction on it.
	*/
	void setFriction(float friction);

	/**
	* Sets the gravity of this Group
	*
	* The gravity is a vector which defines an attractive force that will be applied to each Particle in the Group during the update.
	* By default the gravity is the null vector (i.e. a Vector3D equal to (0,0,0)) which means no gravity is applied.

	* The gravity is applied on each Particle as followed :
		velocity += gravity * deltaTime
	* param gravity : the Vector3D that will be used as the gravity for this Group
	*/
	void setGravity(const Vector3D& gravity);


	/**
	* Assigns a callback for the custom update

	* The user has the possibility to assign a callback function for update that will be called for each Particle right after the standard update.
	* The signature of the function must be of the form :
		bool customUpdate(Particle&,float)
	  with :
		Particle& being the Particle which is currently updated
		float being the time step
		the return bool being true if the Particle has to die at the end of the update, false otherwise

	* @param fupdate : A pointer to the callback function that will perform custom update for this Group
	*/
	void setCustomUpdate(bool (*fupdate)(Particle&,float));

	/**
	* Assigns a callback for custom birth

	* This method allows to assign a callback function that will be called each time a Particle borns.
	* The signature of the function must be of the form :
		void customUpdate(Particle&)
	  with Particle& being the Particle which is just born.
	* @param fbirth : A pointer to the callback function that will perform custom birth for this Group
	*/
	void setCustomBirth(void (*fbirth)(Particle&));

	/**
	* Assigns a callback for custom death

	* This method allows to assign a callback function that will be called each time a Particle dies.
	* The signature of the function must be of the form :
		void customUpdate(Particle&)
	  with Particle& being the Particle which has just died.

	* @param fdeath : A pointer to the callback function that will perform custom death for this Group
	*/
	void setCustomDeath(void (*fdeath)(Particle&));


	/**
	* Enables or disables the sorting of particles

	* The sorting is performed from particles further to the camera to particles closer to the camera.
	* Sorting particles allows to well draw particles with alpha.

	* If the sorting is enabled/disabled, the distance computation is enabled/disabled as well.

	* Note that sorting a Group is a computationnaly expensive operation that should be avoided when possible.
	*/
	void enableSorting(bool sort);
	bool isSortingEnabled() const;

	/**
	* Enables or disables the computation of the distance of a Particle from the camera
	*
	* The distance computation happens at each call to update(float).
	* The distance of a Particle from the camera can be gotten with a call to Particle::getDistanceFromCamera() or Particle::getSqrDistanceFromCamera()

	* Note that the distance is defined by the difference vector between the Particle and the the camera set with System::setCameraPosition(Vector3D&).

	* If the distance computation is disabled, then the sorting of particles is disabled as well.
	*/
	void enableDistanceComputation(bool distanceComputation);
	bool isDistanceComputationEnabled() const;

	/**
	* Enables or disables the computation of the axis aligned bouding box of the Group
	*
	* if the computing of the AABB is enabled, after each call to update(float), AABBox is updated with the coordinates information of the AABB.

	* Knowing the AABB of a Group of particles can be useful in some case like frustum culling for instance.

	* Note that the bounding box does not use the size information of the particles which means when computing the bounding box, particles are considered to be points in the space.
	*/
	void enableAABBComputing(bool AABB);
	bool isAABBComputingEnabled() const;

	/**
	* Enables or not Renderer buffers management in a static way
	*
	* If the buffer management is enabled, then a call to setRenderer(Renderer*) will destroy the existing buffers of the previous Renderer
	* held by this Group, and create the needed buffer for the new Renderer.

	* By default, the renderer buffers management is enabled.
	*/
	static void enableBuffersManagement(bool manage);
	static bool isBuffersManagementEnabled();

	/**
	* Gets the Pool of particles of the Group
	* Note that the Pool returned is constant as the user is not allowed to modify the internal structure of particles.
	*/
	const Pool<Particle>& getParticles() const;

	/**
	* Gets a single Particle in the Group by its index
	*
	* Note that a given Particle in a Group is not ensured to keep the same index all over its life.
	* Particle index can be changed when inactivating particles.
	*/
	Particle& getParticle(uint32 index);

	/**
	* Gets a single Particle in the Group by its index
	* constant version of getParticle(uint32).
	*/
	const Particle& getParticle(uint32 index) const;

	
	// Gets the number of particles in the Group	
	uint32 getNumberOfParticles() const;


	// Gets the emitters of the Group
	const Array<Emitter*>& getEmitters() const;

	// Gets an Emitter of the Group by its index
	Emitter* getEmitter(uint32 index) const;
	// Gets the number of emitters in this Group
	uint32 getNumberOfEmitters() const;

	// Gets the modifiers of the Group
	const Array<Modifier*>& getModifiers() const;

	// Gets an Modifier of the Group by its index
	Modifier* getModifier(uint32 index) const;
	// Gets the number of modifiers in this Group
	uint32 getNumberOfModifiers() const;

	// Gets the Model of this Group
	Model* getModel() const;
	// Gets the Renderer of this Group
	Renderer* getRenderer() const;

	// Gets the friction coefficient of this Group
	float getFriction() const;
	// Gets the gravity Vector3D of this Group
	const Vector3D& getGravity() const;

	// Gets a AABBox holding the AABB of the Group.
	// Note that this method is only useful when the AABB computation is enabled (see enableAABBComputing(bool)).
	const AABBox& getAABBox() const;

	/**
	* Gets the start address of the given param
	*
	* This method is used by a Renderer to define the start position of an array to pass to the GPU.
	* You will not generally need it unless you re designing your own Renderer.

	* Note that if the parameter is not enabled, the return value will point to an enabled parameter starting address.
	*/
	const void* getParamAddress(ModelParam param) const;

	/**
	* Gets the start address of the position
	*
	* This method is used by a Renderer to define the start position of an array to pass to the GPU.
	* You will not generally need it unless you re designing your own Renderer.
	*/
	const void* getPositionAddress() const;

	/**
	* Gets the stride for parameters
	*
	* This method is used by a Renderer to know the stride of an array to pass to the GPU.
	* You will not generally need it unless you re designing your own Renderer.
	*/
	uint32 getParamStride() const;

	/**
	* Gets the stride for positions
	*
	* This method is used by a Renderer to know the stride of an array to pass to the GPU.
	* You will not generally need it unless you re designing your own Renderer.
	*/
	uint32 getPositionStride() const;





	/**
	* Adds some Particles to this Group
	*
	* This method and all the methods of type addParticles will add a given number of Particles at the given position with the given velocity.
	* Note that even if a Zone and an Emitter is passed, the position and the velocity will be the same for all Particles.

	* In case a Zone is passed, Zone::generatePosition(Particle,bool) is used to generate the position.
	* In case an Emitter is passed, Emitter::generateVelocity(Particle) with a mass of 1 is used to generate the velocity.
	* The velocity will then be updated with the Particle's mass when the Particle will be generated.
	* In case a delta time is passed instead of a fixed number, the number will be computed thanks to the flow of the Emitter passed.

	* Particles will be added to the Group at the next call to update(float) or flushAddedParticles().
	* This is why any Emitter and Zone used to generate particles must be valid at the next call of either update(float) or flushAddedParticles().

	* On some methods addParticles, the full variable is present. This variable defines where to generate positions in a Zone :

		true to generate position somewhere within the whole Zone.
		false to generate position somewhere on the Zone border.


	* @param number : the number of Particles to add
	* @param position : the position where the Particles will be added
	* @param velocity : the velocity of the Particles
	*/
	void addParticles(uint32 number, const Vector3D& position, const Vector3D& velocity);

	/**
	* Adds some Particles to this Group
	
	* @param number : the number of Particles to add
	* @param zone : the Zone that will be used to generate the position
	* @param emitter : the Emitter that will be used to generate the velocity
	* @param full : true to generate a position within the whole Zonz, false only at its borders
	*/
	void addParticles(uint32 number, const Zone* zone, Emitter* emitter, bool full = true);

	/**
	* Adds some Particles to this Group
	
	* @param number : the number of Particles to add
	* @param zone : the Zone that will be used to generate the position
	* @param velocity : the velocity of the Particles
	* @param full : true to generate a position within the whole Zonz, false only at its borders
	*/
	void addParticles(uint32 number, const Zone* zone, const Vector3D& velocity, bool full = true);

	/**
	* Adds some Particles to this Group

	* @param number : the number of Particles to add
	* @param position : the position where the Particles will be added
	* @param emitter : the Emitter that will be used to generate the velocity
	*/
	void addParticles(uint32 number, const Vector3D& position, Emitter* emitter);

	/**
	* Adds some Particles to this Group
	* @param number : the number of Particles to add
	* @param emitter : the Emitter that will be used to generate the velocity and whose Zone will be used to generate the position
	*/
	void addParticles(uint32 number, Emitter* emitter);

	/**
	* Adds some Particles to this Group

	* @param zone : the Zone that will be used to generate the position
	* @param emitter : the Emitter that will be used to generate the velocity
	* @param deltaTime : the step time that will be used to determine how many particles to generate
	* @param full : true to generate a position within the whole Zonz, false only at its borders
	*/
	void addParticles(const Zone* zone, Emitter* emitter, float deltaTime, bool full = true);

	/**
	* Adds some Particles to this Group
	* @param position : the position where the Particles will be added
	* @param emitter : the Emitter that will be used to generate the velocity
	* @param deltaTime : the step time that will be used to determine how many particles to generate
	*/
	void addParticles(const Vector3D& position, Emitter* emitter, float deltaTime);

	/**
	* Adds some Particles to this Group
	* @param emitter : the Emitter that will be used to generate the velocity and whose Zone will be used to generate the position
	* @param deltaTime : the step time that will be used to determine how many particles to generate
	*/
	void addParticles(Emitter* emitter, float deltaTime);

	/**
	* Adds some Particles to this Group
	*
	* This method is slightly different to other addParticles methods as Particles are generated not at a point but on a line defined by start and end.
	* The step is the distance between each Particle on the line and the offset is the starting distance to compute the first Particle's position.
	* The offset is useful to generate equidistant particles on several lines. the returned offset has to be used as the passed offset for the next line.

	* This method is useful to generate equidistant particles on lines no matter the framerate.

	* @param start : the position of the start of the line
	* @param end : the position of the end of the line
	* @param emitter : the Emitter that will be used to generate the velocity
	* @param step : the distance between each generated Particle
	* @param offset : the starting distance of the beginning of the line
	* @return the new offset at the end of the line
	*/
	float addParticles(const Vector3D& start, const Vector3D& end, Emitter* emitter, float step, float offset = 0.0f);

	/**
	* Adds some Particles to this Group

	* @param start : the position of the start of the line
	* @param end : the position of the end of the line
	* @param velocity : the velocity of the Particles
	* @param step : the distance between each generated Particle
	* @param offset : the starting distance of the beginning of the line
	* @return the new offset at the end of the line
	*/
	float addParticles(const Vector3D& start, const Vector3D& end, const Vector3D& velocity, float step, float offset = 0.0f);

	/**
	* Removes a Particle from this Group
	* Note that the Particle object is not destroyed but only inactivated in the Pool.
	* @param index : the position of the Particle in this Group
	*/
	void removeParticle(uint32 index);

	// Adds an Emitter in this Group
	// if the emitter is already in the group, it will not be inserted again.
	void addEmitter(Emitter* emitter);
	// Removes an Emitter from this Group
	void removeEmitter(Emitter* emitter);

	// Adds / Removes an Modifier in this Group
	void addModifier(Modifier* modifier);
	void removeModifier(Modifier* modifier);




	/**
	* Updates this Group by a step time
	*
	* The update process performs those operations :

		Updates the mutable parameters of each Particle.
		Updates the velocity of each Particle function of the friction and the gravity of the Group.
		Applies each Modifier of the Group to each Particle.
		Removes all dead particles
		Adds particles generated by the emitters of the Group.
		Adds particles added manually by the user (with calls to addParticles).


	* Those operations are optimized to limit the swaps and shifts of particles in the Pool to its minimum.
	* This method tells whether the Group still has some Particles, or if some Particles will still be generated by the Emitters at the next updates by returning a boolean.
	*
	* @param deltaTime : the time step used to update the Group
	* @return true if the Group has still some current or future Particles to update, false otherwise
	*/
	bool update(float deltaTimeInSeconds);

	// Renders this Group
	// Note that if no Renderer is attached to the Group, nothing will happen.
	void render();

	// Empties this Group
	// Not that this method does not release resource of this Group.
	// Particles are only inactivated, not destroyed.
	void empty();

	/**
	* Adds all manually added particles to the Group
	*
	* This method adds all particles added with the addParticles methods immediatly to the Group without waiting the next call to update(float).
	* The Particles are added in FIFO order until there is no more or the capacity limit of the Group is reached.
	*/
	void flushAddedParticles();

	/**
	* Sorts the particles within this Group
	*
	* The particles are sorted only if the sorting of this Group is enabled. See enableSorting(bool).
	* Note that update(float) also sorts the particles.
	* This method is therefore only useful when the camera position changes several times between 2 updates.
	* This method also makes a call to computeDistances().
	*/
	void sortParticles();


	/**
	* Computes the distance between each Particle within this Group and the camera
	*
	* The distances are computed only if the computation of distances of this Group is enabled. See enableDistanceComputation(bool).
	* Note that update(float) also computes the distances between particles and camera.
	* This method is therefore only useful when the camera position changes several times between 2 updates.
	*/
	void computeDistances();

	/**
	* Computes the bounding box of this Group
	*
	* The bounding box is computed only if the computation of the bounding box of this Group is enabled. See enableAABBComputing(bool).
	* Note that update(float) also computes the bounding box.
	* This method is therefore only useful when the bounding boxe has to be recomputed between 2 updates.
	*/
	void computeAABB();

	/**
	* Increases the maximum number of particles this Group can hold
	*
	* Note that decreasing the capacity will have no effect.
	* A reallocation of the group capacity will destroy all its current buffers.
	*/
	void reallocate(uint32 capacity);

	/**
	* Creates a new additional buffer attached to the Group.
	*
	* Additional buffers are used to attach data to a particles. They are mainly used by renderers to store data to transfer to the GPU
	* but can be used by the user in any other way.

	* A buffer is defined by a ID which is a String.
	* A buffer also has a flag, which can give information about the way it is configured.

	* Note that ID starting with PARTICLE_ are reserved by the engine.
	* Note also that creating a buffer with an already existing ID will destroy the previous buffer.

	* A buffer needs a BufferCreator to allow the group to create it.
	*
	* @param ID : the ID of the additinal buffer
	* @param creator : the buffer creator object
	* @param flag : the flag of the buffer
	* @param swapData : true to swap data when particle are swapped, false not to (faster but dont keep right order)
	*/
	Buffer* createBuffer(uint32 ID, const BufferCreator& creator, uint32 flag = 0, bool swapData = true) const;

	/**
	* Destroys the buffer with the given ID
	*
	* If no buffer with this ID exists, nothing happens.
	* Note that destroying a buffer must never freeze the engine. Checks must be performed.
	*/
	void destroyBuffer(uint32 ID) const;

	// Destroys all the buffers held by this Group
	void destroyAllBuffers() const;

	/**
	* Gets the buffer with the given ID and checks its flag
	*
	* The flag is used as a check. the passed flag must be the same as the stored flag.
	* If not or if no buffer with the given ID exists, NULL is returned.
	* The method returns a pointer on the buffer.
	* return a pointer to the buffer, or NULL if not found or with an incorrect flag
	*/
	Buffer* getBuffer(uint32 ID, uint32 flag) const;

	/**
	* Gets the buffer with the given ID
	* NULL is returned if the buffer does not exist
	*/
	Buffer* getBuffer(uint32 ID) const;

	virtual Registerable* findByName(const String& name);


protected:

	virtual void registerChildren(bool registerAll);
	virtual void copyChildren(const Registerable& object, bool createBase);
	virtual void destroyChildren(bool keepChildren);

	virtual void propagateUpdateTransform();

private:
	struct CreationData
	{
		uint32 number;
		Vector3D position;
		Vector3D velocity;
		const Zone* zone;
		Emitter* emitter;
		bool full;
	};

	struct EmitterData
	{
		Emitter* emitter;
		uint32 NumOfParticles;
	};

	// statics
	static bool bufferManagement;
	static Model& getDefaultModel();

	// registerables
	Model* model;
	Renderer* renderer;
	Array<Emitter*> emitters;
	Array<Modifier*> modifiers;

	mutable Array<EmitterData> activeEmitters;
	mutable Array<Modifier*> activeModifiers; // Vector of active modifiers to optimise the parsing when updating

	// physics parameters
	float friction;
	Vector3D gravity;

	// particles data
	Pool<Particle> pool;
	Particle::ParticleData* particleData;
	float* particleCurrentParams;	// Stores the current parameters values of the particles
	float* particleExtendedParams;	// Stores the extended parameters values of the particles (final values and interpolated data)

	// sorting
	bool sortingEnabled;
	bool distanceComputationEnabled;

	// creation data
	Array<CreationData> creationBuffer;
	uint32 NumOfBufferedParticles;

	// callbacks
	bool (*fupdate)(Particle&, float);
	void (*fbirth)(Particle&);
	void (*fdeath)(Particle&);

	// bounding box
	bool boundingBoxEnabled;
	AABBox boundingBoxAABB;

	// additional buffers
	mutable HashMap<uint32, Buffer*> additionalBuffers;
	mutable SortedSet<Buffer*> swappableBuffers;

	// the assignment operator is private
	Group& operator=(const Group& group);

	void pushParticle(EmitterData* &emitterIt, uint32& NumberOfManualBorn);
	void launchParticle(Particle& p, EmitterData* &emitterIt, uint32& NumberOfManualBorn);

	void addParticles(uint32 number, const Vector3D& position, const Vector3D& velocity, const Zone* zone, Emitter* emitter, bool full = false);

	void popNextManualAdding(uint32& NumberOfManualBorn);

	void updateAABB(const Particle& particle);

	void sortParticles(int32 start, int32 end);
};



inline Group* Group::create(Model* _model, uint32 capacity)
{
	Group* obj = new Group(_model, capacity);
	registerObject(obj);
	return obj;
}

inline void Group::setFriction(float friction)
{
	this->friction = friction;
}

inline void Group::setGravity(const Vector3D& gravity)
{
	this->gravity = gravity;
}

inline void Group::setCustomUpdate(bool (*fupdate)(Particle&, float))
{
	this->fupdate = fupdate;
}

inline void Group::setCustomBirth(void (*fbirth)(Particle&))
{
	this->fbirth = fbirth;
}

inline void Group::setCustomDeath(void (*fdeath)(Particle&))
{
	this->fdeath = fdeath;
}

inline void Group::enableSorting(bool sort)
{
	sortingEnabled = sort;
	distanceComputationEnabled = sort;
}

inline void Group::enableDistanceComputation(bool distanceComputation)
{
	distanceComputationEnabled = distanceComputation;
	if( !distanceComputation ) 
		enableSorting(false);
}

inline void Group::enableAABBComputing(bool AABB)
{
	boundingBoxEnabled = AABB;
}

inline const Pool<Particle>& Group::getParticles() const
{
	return pool;
}

inline Particle& Group::getParticle(uint32 index)
{
	return pool[index];
}

inline const Particle& Group::getParticle(uint32 index) const
{
	return pool[index];
}

inline uint32 Group::getNumberOfParticles() const
{
	return pool.getSizeOfActive();
}

inline const Array<Emitter*>& Group::getEmitters() const
{
	return emitters;
}

inline Emitter* Group::getEmitter(uint32 index) const
{
	return emitters[index];
}

inline uint32 Group::getNumberOfEmitters() const
{
	return emitters.size();
}

inline const Array<Modifier*>& Group::getModifiers() const
{
	return modifiers;
}

inline Modifier* Group::getModifier(uint32 index) const
{
	return modifiers[index];
}

inline uint32 Group::getNumberOfModifiers() const
{
	return modifiers.size();
}

inline Model* Group::getModel() const
{
	return model;
}

inline Renderer* Group::getRenderer() const
{
	return renderer;
}

inline float Group::getFriction() const
{
	return friction;
}

inline const Vector3D& Group::getGravity() const
{
	return gravity;
}

inline bool Group::isSortingEnabled() const
{
	return sortingEnabled;
}

inline bool Group::isDistanceComputationEnabled() const
{
	return distanceComputationEnabled;
}

inline bool Group::isAABBComputingEnabled() const
{
	return boundingBoxEnabled;
}

inline const AABBox& Group::getAABBox() const
{
	return boundingBoxAABB;
}



inline void Group::addParticles(uint32 number, const Vector3D& position, const Vector3D& velocity)
{
	addParticles(number, position, velocity, NULL, NULL);
}

inline void Group::addParticles(uint32 number, const Zone* zone, Emitter* emitter, bool full)
{
	addParticles(number, Vector3D(), Vector3D(), zone, emitter, full);
}

inline void Group::addParticles(uint32 number, const Zone* zone, const Vector3D& velocity, bool full)
{
	addParticles(number, Vector3D(), velocity, zone, NULL, full);
}

inline void Group::addParticles(uint32 number, const Vector3D& position, Emitter* emitter)
{
	addParticles(number, position, Vector3D(), NULL, emitter);
}

inline void Group::removeParticle(uint32 index)
{
	pool.makeInactive(index);
}

inline const void* Group::getPositionAddress() const
{
	return &(particleData[0].position);
}

inline uint32 Group::getPositionStride() const
{
	return sizeof(Particle::ParticleData);
}


#endif		// __SGP_SPARKGROUP_HEADER__