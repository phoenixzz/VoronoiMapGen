#ifndef __SGP_SPARKLINEARFORCE_HEADER__
#define __SGP_SPARKLINEARFORCE_HEADER__

// An enum defining the way a factor is applied to a force
enum ForceFactor
{
	FACTOR_NONE,	/**< No factor is applied */
	FACTOR_LINEAR,	/**< A linear factor is applied */
	FACTOR_SQUARE,	/**< A square factor is applied */
};

/**

* A Modifier applying a linear force on particles
*
* The force is identical from any points of the universe (only if the modifier is triggered).
* In that way, particles under the influence of a LinearForce can theorically reach an infinite speed if not under the influence of a friction.

* The force can be multiplied or not by a particle parameter either linearly or squared.

* Note that this Modifier can be used to set a global gravity that can be updated for all groups at a time.
* To do so, the LinearForce has to be used with the param : PARAM_MASS and the FactorType FACTOR_LINEAR.

*/
class SGP_API LinearForce : public Modifier
{
	SPARK_IMPLEMENT_REGISTERABLE(LinearForce)

public:



	// zone : the Zone of the LinearForce or NULL if no Zone
	// trigger : the trigger of the LinearForce
	// force : the force of the LinearForce
	// type : the type of multiplier for the factor
	// param : the parameter used as the factor (if type != FACTOR_NONE) 
	LinearForce(Zone* zone = NULL,
		ModifierTrigger trigger = INSIDE_ZONE,
		const Vector3D& force = Vector3D(),
		ForceFactor type = FACTOR_NONE,
		ModelParam param = PARAM_SIZE);


	static LinearForce* create(Zone* zone = NULL,
		ModifierTrigger trigger = INSIDE_ZONE,
		const Vector3D& force = Vector3D(),
		ForceFactor type = FACTOR_NONE,
		ModelParam param = PARAM_SIZE);



	void setForce(const Vector3D& force);

	/**
	* Sets the factor type to apply to the force
	*	type : the type of multiplier for the factor
	*	param : the parameter of the Particle to use as the factor
	*/
	void setFactor(ForceFactor type, ModelParam param = PARAM_SIZE);




	const Vector3D& getForce() const;
	const Vector3D& getTransformedForce() const;


	ForceFactor getFactorType() const;
	ModelParam getFactorParam() const;

protected:
	virtual void innerUpdateTransform();

private:

	Vector3D force;
	Vector3D tForce;

	ForceFactor factorType;
	ModelParam factorParam;

	virtual void modify(Particle& particle, float deltaTime) const;
};


inline LinearForce* LinearForce::create(Zone* _zone, ModifierTrigger _trigger, const Vector3D& _force, ForceFactor _type, ModelParam _param)
{
	LinearForce* obj = new LinearForce(_zone, _trigger, _force, _type, _param);
	registerObject(obj);
	return obj;
}
	
inline void LinearForce::setForce(const Vector3D& force)
{
	this->force = tForce = force;
	notifyForUpdate();
}

inline void LinearForce::setFactor(ForceFactor type, ModelParam param)
{
	factorType = type;
	factorParam = param;
}

inline const Vector3D& LinearForce::getForce() const
{
	return force;
}

inline const Vector3D& LinearForce::getTransformedForce() const
{
	return tForce;
}

inline ForceFactor LinearForce::getFactorType() const
{
	return factorType;
}

inline ModelParam LinearForce::getFactorParam() const
{
	return factorParam;
}

inline void LinearForce::innerUpdateTransform()
{
	Modifier::innerUpdateTransform();
	tForce = force;
	tForce.RotateWith(getWorldTransform());
}


#endif		// __SGP_SPARKLINEARFORCE_HEADER__