#ifndef __SGP_SPARKMODEL_HEADER__
#define __SGP_SPARKMODEL_HEADER__


// Constants used to set bits in Model flags
enum ModelParamFlag
{
	FLAG_NONE = 0,										/**< the flag bit for no parameter */
	FLAG_RED = 1 << PARAM_RED,							/**< the flag bit for PARAM_RED */
	FLAG_GREEN = 1 << PARAM_GREEN,						/**< the flag bit for PARAM_GREEN */
	FLAG_BLUE = 1 << PARAM_BLUE,						/**< the flag bit for PARAM_BLUE */
	FLAG_ALPHA = 1 << PARAM_ALPHA,						/**< the flag bit for PARAM_ALPHA */
	FLAG_SIZE = 1 << PARAM_SIZE,						/**< the flag bit for PARAM_SIZE */
	FLAG_MASS = 1 << PARAM_MASS,						/**< the flag bit for PARAM_MASS */
	FLAG_ANGLE = 1 << PARAM_ANGLE,						/**< the flag bit for PARAM_ANGLE */
	FLAG_TEXTURE_INDEX = 1 << PARAM_TEXTURE_INDEX,		/**< the flag bit for PARAM_TEXTURE_INDEX */
	FLAG_ROTATION_SPEED = 1 << PARAM_ROTATION_SPEED,	/**< the flag bit for PARAM_ROTATION_SPEED */
	FLAG_CUSTOM_0 = 1 << PARAM_CUSTOM_0,				/**< the flag bit for PARAM_CUSTOM_0 */
	FLAG_CUSTOM_1 = 1 << PARAM_CUSTOM_1,				/**< the flag bit for PARAM_CUSTOM_1 */
	FLAG_CUSTOM_2 = 1 << PARAM_CUSTOM_2,				/**< the flag bit for PARAM_CUSTOM_2 */
};

/**
A model of particles for particles' generation and evolution

* This class defines a behavior for particles generation and evolution over time.
* A Particle is generated under a Model that cannot be changed during its life time.
* A Particle Model can however be changed but it will reinitialize the Particle parameters and life.

* A Model defines which parameters to use/update for a Particle. To do that an Model holds 4 flags that are :

	Enabled : The parameter is enabled and can be set. Otherwise its default value is used.
	Mutable : The parameter will vary linearly over the life time of the Particle.
	Random : The parameter will be randomly generated for the Particle.
	Interpolated : The parameter will be interpolated by an object Interpolator to allow flexible variations.

* The flags are processed when building up a model so that they are consistent :

	All flags are masked so that they have not bit set at a higher position than the number of parameters.
	The mutable,random and interpolated flags are masked with the enabled flag : a parameter must be enabled to be either random, mutable or interpolated.
	An interpolated parameter cannot be mutable or/and random as well. The interpolated flag has the priority.

* The life time of a particle and immortality is also defined by the Model.

* The default values for the parameters are the following :

	PARAM_RED : 1.0
	PARAM_GREEN : 1.0
	PARAM_BLUE : 1.0
	PARAM_ALPHA : 1.0
	PARAM_SIZE : 1.0
	PARAM_MASS : 1.0
	PARAM_ANGLE : 0.0
	PARAM_TEXTURE_INDEX : 0.0
	PARAM_ROTATION_SPEED : 0.0
	PARAM_CUSTOM_0 : 0.0
	PARAM_CUSTOM_1 : 0.0
	PARAM_CUSTOM_2 : 0.0

*/

class SGP_API Model : public Registerable
{
	friend class Particle;

	SPARK_IMPLEMENT_REGISTERABLE(Model)	
	
public :
	/**
	* The user have to pass the Model flags that cannot be changed afterwards.
	* To set up flags enumerators from ModelParamFlag can be used in a OR way.
	* For instance, Model(FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_ALPHA,FLAG_NONE,FLAG_NONE,FLAG_NONE)
	* constructs a Model that will generate Particles with a defined color that will not change over time.

	* PARAM_RED, PARAM_GREEN and PARAM_BLUE are enabled by default. 
	* This is needed to pass the values to the GPU in an optimized way.
	* Note that even the enable flag passed by the user does not containt those params, they will be enabled anyway.
	*/
	Model(uint32 enableFlag = FLAG_RED | FLAG_GREEN | FLAG_BLUE, uint32 mutableFlag = FLAG_NONE, uint32 randomFlag = FLAG_NONE, uint32 interpolatedFlag = FLAG_NONE);
	Model(const Model& model);
	~Model();

	static Model* create(uint32 enableFlag = FLAG_RED | FLAG_GREEN | FLAG_BLUE, uint32 mutableFlag = FLAG_NONE, uint32 randomFlag = FLAG_NONE, uint32 interpolatedFlag = FLAG_NONE);

	/**
	* All particles generated under this Model will be given a life time randomly generated within [lifeTimeMin,lifeTimeMax[
	* To generate particles with a non random life time, simply use setLifeTime(a,a)
	*/
	inline void setLifeTime(float lifeTimeMin, float lifeTimeMax)
	{
		this->lifeTimeMin = lifeTimeMin;
		this->lifeTimeMax = lifeTimeMax;
	}

	/**
	* Defines whether to generate immortal particles or not
	*
	* Immortal particles will still be given a life time. However the age of immortal particles will not increase so that they cannot die.
	* An immortal Particle whose Model immortality is unset will grow older again.
	* The immortality is therefore dynamic and can be granted or removed by impacting all particles generated under this Model.
	*/
	inline void setImmortal(bool immortal)
	{
		this->immortal = immortal;
	}
	/**
	* Sets a given parameter with 4 values
	*
	* This method will only set parameters that are enabled, mutable and random.
	* If not nothing will happen and the method will return false.
	*
	* param type : the parameter to set
	* param startMin : the minimum value at the Particle birth
	* param startMax : the maximum value at the Particle birth
	* param endMin : the minimum value at the Particle death
	* param endMax : the maximum value at the Particle death
	* return true if the parameter can be set (right flags), false otherwise
	*/
	bool setParam(ModelParam type, float startMin, float startMax, float endMin, float endMax);
	
	/**
	* Sets a given parameter with 2 values
	*
	* This method will only set parameters that are enabled and either mutable or random.
	* If not nothing will happen and the method will return false.

	* In case of a mutable parameter : value0 defines the birth value and value0 defines the death value.
	* In case of a random parameter : value0 defines the minimum value and value1 defines the maximum value.
	*/
	bool setParam(ModelParam type, float value0, float value1);

	/**
	* Sets a given parameter with 1 value
	*
	* This method will only set parameters that are enabled and neither mutable or random.
	* If not nothing will happen and the method will return false.

	* Note that to set the value of an interpolated parameter cannot be set with this function,
	* you must used the interpolator object associated to the parameter to control its value.
	*/
	bool setParam(ModelParam type, float value);

	// Gets the minimum/maximum life time
	inline float getLifeTimeMin() const { return lifeTimeMin; }
	inline float getLifeTimeMax() const { return lifeTimeMax; }

	// whether immortility is set or not
	inline bool isImmortal() const { return immortal; }

	// Checks whether a parameter is enabled or not
	// return 0 is the parameter is not enabled, a flag with the parameter bit set otherwise
	inline uint32 isEnabled(ModelParam type) const
	{
		return enableFlag & (1 << type);
	}

	// Checks whether a parameter is mutable or not
	// return 0 is the parameter is not mutable, a flag with the parameter bit set otherwise
	inline uint32 isMutable(ModelParam type) const
	{
		return mutableFlag & (1 << type);
	}

	// Checks whether a parameter is random or not
	// return 0 is the parameter is not random, a flag with the parameter bit set otherwise
	inline uint32 isRandom(ModelParam type) const
	{
		return randomFlag & (1 << type);
	}

	// Checks whether a parameter is interpolated or not
	// return 0 is the parameter is not interpolated, a flag with the parameter bit set otherwise
	inline uint32 isInterpolated(ModelParam type) const
	{
		return interpolatedFlag & (1 << type);
	}


	/**
	* Gets a parameter value
	* If index is superior or equal to the number of values for the parameter, the default value of the parameter is returned.
	* Otherwise the value returned depending on the index is :
		Enabled parameters : 0 the value of the parameter
		Mutable parameters : 0 the birth value of the parameter
							 1 the death value of the parameter

		Random parameters : 0 the minimum value of the parameter
							1 the maximum value of the parameter

		Mutable and random parameters :
			0 : the minimum birth value of the parameter
			1 : the maximum birth value of the parameter
			2 : the minimum death value of the parameter
			3 : the maximum death value of the parameter

	* Note that in case of an interpolated parameter, the default value is always returned.
	* The user has to use the interpolator object associated to the parameter to get its values.
	*/
	float getParamValue(ModelParam type, uint32 index) const;

	/**
	* Gets the number of values for a parameter
	* The number of values depends on the Model flags :

		Enabled parameter : 1
		Mutable or random parameter : 2
		Mutable and random parameter : 4
		Interpolated parameter : 0
		Not Enabled : 0
	*/
	uint8 getNumberOfValues(ModelParam type) const;

	/**
	* Gets the number of enabled parameters
	* return the number of enabled parameters in the model
	*/
	inline uint8 getNumberOfEnabled() const { return NumOfEnableParams; }

	/**
	* Gets the number of mutable parameters
	* return the number of mutable parameters in the model
	*/
	inline uint8 getNumberOfMutable() const { return NumOfMutableParams; }

	/**
	* Gets the number of random parameters
	* return the number of random parameters in the model
	*/
	inline uint8 getNumberOfRandom() const { return NumOfRandomParams; }

	/**
	* Gets the number of interpolated parameters
	* return the number of interpolated parameters in the model
	*/
	inline uint8 getNumberOfInterpolated() const { return NumOfInterpolatedParams; }

	/**
	* Gets the interpolator for the given parameter
	* IN - > param : the parameter whose intepolator must be retrieved
	* return a pointer to the interpolator of the given parameter or NULL if the parameter is not interpolated
	*/
	inline Interpolator* getInterpolator(ModelParam param) { return interpolators[param]; }

	/**
	* Gets the number of float values in the particle current array
	* The particle current array holds the current values of enabled parameters for a particle.
	* This is used internally and should not be needed by the user.
	*/
	inline uint32 getSizeOfParticleCurrentArray() const { return NumOfEnableParams; }

	/**
	* Gets the number of float values in the particle extended array
	*
	* The particle extended array holds the extended values needed for parameters interpolation for a particle.
	* This is used internally and should not be needed by the user.
	*/
	inline uint32 getSizeOfParticleExtendedArray() const
	{
		return NumOfMutableParams + (NumOfInterpolatedParams << 1) + NumOfInterpolatedParams; // nbMutable + nbInterpolated * 3
	}

	/**
	* Gets the number of float values in the model array
	*
	* This is used internally and should not be needed by the user.
	*/
	inline uint32 getSizeOfModelArray() const
	{
		return paramsSize;
	}


	/**
	* Gets the offset of the given parameter in the current array
	* This methods is used internally by the engine
	*/
	inline uint32 getParameterOffset(ModelParam param) const
	{
		return particleEnableIndices[param];
	}

	/**
	* Gets the default value of the parameter
	*/
	static float getDefaultValue(ModelParam param);

private:
	// total number of parameters
	static const int MAX_PARAMS = 12;
	// default values for the parameters
	static const float DEFAULT_VALUES[MAX_PARAMS];
	
	// arrays storing the values of parameters for the model following that form :
	// enable : 1 value -> value
	// mutable : 2 values -> start value | end value 
	// random : 2 values -> min value | max value
	// mutable and random : 4 values -> start min value | start max value | end min value | end max value
	// interpolated : 0 value
	float* params;
	uint32 paramsSize;

	// array storing the parameters that are enabled
	uint8 NumOfEnableParams;
	uint32* enableParams;

	// array storing the parameters that are mutable
	uint8 NumOfMutableParams;
	uint32* mutableParams;

	// array storing the parameters that are interpolated
	uint8 NumOfInterpolatedParams;
	uint32* interpolatedParams;

	// Number of random parameters
	uint8 NumOfRandomParams;

	// array of interpolators
	Interpolator* interpolators[MAX_PARAMS]; 

	// the flags of the model
	uint32 enableFlag;
	uint32 mutableFlag;
	uint32 randomFlag;
	uint32 interpolatedFlag;

	uint32 particleEnableIndices[MAX_PARAMS];	// array storing the index of a parameter in the enableParams array
	uint32 particleMutableIndices[MAX_PARAMS];	// array storing the index of a parameter in the mutableParams array
	uint32 indices[MAX_PARAMS];					// array storing the index of a parameter in the model param array

	float lifeTimeMin;
	float lifeTimeMax;
	bool immortal;

	void initParamArrays(const Model& model);
	// the assignment operator is private
	Model& operator=(const Model& model);
};

inline Model* Model::create(uint32 _enableFlag, uint32 _mutableFlag, uint32 _randomFlag, uint32 _interpolatedFlag)
{
	Model* obj = new Model(_enableFlag, _mutableFlag, _randomFlag, _interpolatedFlag);
	registerObject(obj);
	return obj;
}

#endif		// __SGP_SPARKMODEL_HEADER__