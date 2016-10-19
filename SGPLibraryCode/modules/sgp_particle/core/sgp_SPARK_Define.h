

#ifndef __SGP_SPARKDEFINE_HEADER__
#define __SGP_SPARKDEFINE_HEADER__



/**
* SGP Engine SPARK Particle Code
*
* SPARK has been designed to be :
* 
* user friendly and easy to implement
* very configurable and as complete as possible
* evolving and upgradable by users
* portable and library free (it only uses the standard library)

* in SPARK, a SPARK::System is a collection of Groups of Particles.
*  A SPARK::Group contains a SPARK::Pool of Particles and 
			defines an complete universe for Particle evolution.
* This universe holds 3 types of physical objects :

* the Emitters : an SPARK::Emitter is an object that generates Particles
* the Modifiers : a SPARK::Modifier is an object that will modify Particles behavior
* the Particles themselves : a SPARK::Particle is a point that will follow physical laws and
						will be given some parameters that will evolve from 
						their birth to their death.

* Those parameters are defined by a SPARK::Model of Particles.
* The Emitters and Modifiers are physical entities whose body is represented by a SPARK::Zone.
* Vector3D is the primitive object used in SPARK to represents triplets of coordinates in a universe 3D

* Finally a system/groups can be renderered using a SPARK::Renderer.
*/



/** the random seed for the pseudo random numbers generation (1 by default) */
extern uint32 randomSeed;

/**
* Returns a random number in the range [min,max[
*
* Note that the sequence of pseudo random number generated depends on the initial seed which can be set by setting randomSeed.
*
* @param min : the minimum value
* @param max : the maximum value
*
* @return a random number within [min,max[
*/
template<typename T>
T random(T min,T max)
{
	// optimized standard minimal
	int32 tmp0 = 16807 * (randomSeed & 0xFFFF);
    int32 tmp1 = 16807 * (randomSeed >> 16);
    int32 tmp2 = (tmp0 >> 16) + tmp1;
    tmp0 = ((tmp0 & 0xFFFF)|((tmp2 & 0x7FFF) << 16)) + (tmp2 >> 15);

	// correction of the error
    if ((tmp0 & 0x80000000) != 0)
		tmp0 = (tmp0 + 1) & 0x7FFFFFFF;

	randomSeed = tmp0;

	// find a random number in the interval
    return static_cast<T>(min + ((randomSeed - 1) / 2147483646.0) * (max - min));
}

/////////////////////////
// global enumerations //
/////////////////////////

/**
* Constants for the Model parameters
*/
enum ModelParam
{
	PARAM_RED = 0,				/**< The red component of the Particle */
	PARAM_GREEN = 1,			/**< The green component of the Particle */
	PARAM_BLUE = 2,				/**< The blue component of the Particle */
	PARAM_ALPHA = 3,			/**< The alpha component of the Particle */
	PARAM_SIZE = 4,				/**< The size of the Particle */
	PARAM_MASS = 5,				/**< The mass of the Particle */
	PARAM_ANGLE = 6,			/**< The angle of the texture of the Particle */
	PARAM_TEXTURE_INDEX = 7,	/**< the index of texture of the Particle */
	PARAM_ROTATION_SPEED = 8,	/**< the rotation speed of the particle (must be used with a rotator modifier) */
	PARAM_CUSTOM_0 = 9,			/**< Reserved for a user custom parameter. This is not used by SPARK */
	PARAM_CUSTOM_1 = 10,		/**< Reserved for a user custom parameter. This is not used by SPARK */
	PARAM_CUSTOM_2 = 11,		/**< Reserved for a user custom parameter. This is not used by SPARK */
};


#endif		// __SGP_SPARKDEFINE_HEADER__
