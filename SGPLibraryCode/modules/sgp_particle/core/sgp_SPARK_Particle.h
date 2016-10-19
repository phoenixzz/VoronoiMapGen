#ifndef __SGP_SPARKPARTICLE_HEADER__
#define __SGP_SPARKPARTICLE_HEADER__

class Group;

/**
* A single particle (a point in space with a velocity and different parameters)

* A Particle is the primitive on which all the SPARK Particle Core relies.
* Particles are designed to be handled in large amounts (called Group).
* This is why the user cannot update or render directly a single Particle.

* Note that the Particle class is only a class that presents an interface to the user, particles data are stored in the groups.
* This is why copying a Particle will not copy its data.
*/
class SGP_API Particle
{
	friend bool isFurtherToCamera(const Particle&, const Particle&);
	friend void swapParticles(Particle& a, Particle& b);
	friend class Group;
	friend class Pool<Particle>;

public:
	/**
	* Sets the current value for the given parameter

	* Note that the method will have no effect if the parameter is not enabled in the Particle's Model.
	* In this case, the method returns false.

	* return true if the parameter can be set, false otherwise (if the parameter is not enabled)
	*/
	bool setParamCurrentValue(ModelParam type, float value);

	/**
	* Sets the final value for the given parameter

	* The final value is the value the parameter will have at the end of the Particle's life.
	* Note that the method will have no effect if the parameter is not mutable in the Particle's Model.
	* In this case, the method returns false and setParamCurrentValue(ModelParam,float) should be used.

	* return true if the parameter can be set, false otherwise (if the parameter is not mutable)
	*/
	bool setParamFinalValue(ModelParam type, float value);

	/**
	* Changes the current value for the given parameter
	*
	* The delta is added to the current value of the parameter.
	* For more information see setParamCurrentValue(ModelParam,float).

	* return true if the parameter can be changed, false otherwise (if the parameter is not enabled)
	*/
	bool changeParamCurrentValue(ModelParam type, float delta);

	/**
	* Changes the final value for the given parameter
	*
	* The delta is added to the final value of the parameter.
	* For more information see setParamFinalValue(ModelParam,float).
	
	* return true if the parameter can be changed, false otherwise (if the parameter is not mutable)
	*/
	bool changeParamFinalValue(ModelParam type, float delta);

	/**
	* Sets the life left of the Particle.
	* When the Particle's life reaches 0, the Particle is inactivated.
	*/
	inline void setLifeLeft(float life) { data->life = life; }
	
	// Gets the position of the Particle
	inline Vector3D& position() { return data->position; }
	inline const Vector3D& position() const { return data->position; }
	// Gets the velocity of the Particle
	inline Vector3D& velocity() { return data->velocity; }
	inline const Vector3D& velocity() const { return data->velocity; }
	// Gets the old position of the Particle
	inline Vector3D& oldPosition() { return data->oldPosition; }
	inline const Vector3D& oldPosition() const { return data->oldPosition; }

	/**
	* Gets the current value for the given parameter
	* Note that if the the parameter is not enabled in the Particle's Model, the default value for the parameter is returned.
	*/
	float getParamCurrentValue(ModelParam type) const;

	/**
	* Gets the final value for the given parameter
	* Note that if the the parameter is not enabled in the Particle's Model, the default value for the parameter is returned.
	* If the parameter is enabled but not mutable, the current value is returned.
	*/
	float getParamFinalValue(ModelParam type) const;

	// Gets the Model of this Particle
	Model* getModel() const;
	// Gets the group of this Particle
	inline Group* getGroup() const { return group; }

	// Gets the index of this Particle in its Group
	inline uint32 getIndex() const { return index; }

	// Gets the amount of life left of the Particle
	// The life left of the Particle is the time left before the Particle dies.
	// Note that in case of immortal particles, this value does not evolve.
	inline float getLifeLeft() const { return data->life; }

	/**
	* Gets the age of the Particle
	* The age of a Particle starts at zero when it is initialized and evolve at each update.
	* Note that even immortal particles gets older.
	*/
	inline float getAge() const { return data->age; }

	/**
	* Gets the distance of this Particle from the camera.
	* Note that the correct distance is only returned if the Group of this Particles has its distance computation enabled.
	*/
	inline float getDistanceFromCamera() const { return std::sqrt(data->sqrDist); }

	/**
	* Gets the square distance of this Particle from the camera.
	* Note that the correct distance is only returned if the Group of this Particles has its distance computation enabled.
	* This method is faster than getDistanceFromCamera() and should be used instead when possible.
	*/
	inline float getSqrDistanceFromCamera() const { return data->sqrDist; }

	// Tells whether this Particle was initialized at its latest update or not
	// A call to this method is equivalent to (getAge() == 0.0f)
	inline bool isNewBorn() const { return data->age == 0.0f; }

	// Tells whether this Particle is alive or not
	// A call to this method is equivalent to (getLifeLeft > 0.0f)
	inline bool isAlive() const { return data->life > 0.0f; }


	// Initializes the Particle
	// When a Particle is initialized, all its parameters are reinitialized as well as its life.
	void init();

	// Kills this Particle
	// This method is equivalent to a call to setLifeLeft(float) with life being 0.
	inline void kill() { data->life = 0.0f; }


	// the color component are always enabled, we optimizes it a bit for access
	inline float getR() const { return currentParams[PARAM_RED]; }
	inline float getG() const { return currentParams[PARAM_GREEN]; }
	inline float getB() const { return currentParams[PARAM_BLUE]; }

private:
	struct ParticleData
	{
		Vector3D oldPosition;
		Vector3D position;
		Vector3D velocity;
		float age;
		float life;
		float sqrDist;
	};

	Group* group;
	uint32 index;

	ParticleData* data;
	float* currentParams;
	float* extendedParams;

	Particle(Group* group, uint32 index);

	bool update(float timeDelta);
	void computeSqrDist();

	void interpolateParameters();
};

// specialization of the swap for particle
template<>
inline void Pool<Particle>::swapElements(Particle& a, Particle& b)
{
	swapParticles(a, b);
}


inline bool isFurtherToCamera(const Particle& a, const Particle& b)
{
	return a.getSqrDistanceFromCamera() > b.getSqrDistanceFromCamera();
}

// Swaps particle data. Used internally. Do not use with particles that are not from the same group !
extern void swapParticles(Particle& a, Particle& b);

#endif		// __SGP_SPARKPARTICLE_HEADER__