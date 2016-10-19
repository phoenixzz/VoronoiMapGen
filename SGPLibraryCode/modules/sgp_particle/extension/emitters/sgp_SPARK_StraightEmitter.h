#ifndef __SGP_SPARKSTRAIGHTEMITTER_HEADER__
#define __SGP_SPARKSTRAIGHTEMITTER_HEADER__


/**
* An Emitter that emits in a given direction
*/
class SGP_API StraightEmitter : public Emitter
{
	SPARK_IMPLEMENT_REGISTERABLE(StraightEmitter)

public:

	StraightEmitter(const Vector3D& direction = Vector3D(0.0f,0.0f,-1.0f)) : Emitter()
	{
		setDirection(direction);
	}


	static StraightEmitter* create(const Vector3D& direction = Vector3D(0.0f,0.0f,-1.0f));



	/**
	* Sets the direction of this StraightEmitter
	*
	* Note that it is not necessary to provide a normalized Vector3D.
	* This Vector3D only indicates a direction, its norm does not matter.
	*/
	void setDirection(const Vector3D& direction)
	{
		this->direction = direction;
		this->direction.Normalize();
		tDirection = this->direction;
		notifyForUpdate();
	}

	const Vector3D& getDirection() const;

	/**
	* Gets the transformed direction of this StraightEmitter
	*/
	const Vector3D& getTransformedDirection() const;

protected:
	virtual void innerUpdateTransform()
	{
		Emitter::innerUpdateTransform();
		tDirection = direction;
		tDirection.RotateWith(getWorldTransform());
		tDirection.Normalize();
	}

private:
	Vector3D direction;
	Vector3D tDirection;

	virtual void generateVelocity(Particle& particle, float speed) const;
};


inline StraightEmitter* StraightEmitter::create(const Vector3D& _direction)
{
	StraightEmitter* obj = new StraightEmitter(_direction);
	registerObject(obj);
	return obj;
}

inline const Vector3D& StraightEmitter::getDirection() const
{
	return direction;
}

inline const Vector3D& StraightEmitter::getTransformedDirection() const
{
	return tDirection;
}

inline void StraightEmitter::generateVelocity(Particle& particle, float speed) const
{
	particle.velocity() = tDirection;
	particle.velocity() *= speed;
}

#endif		// __SGP_SPARKSTRAIGHTEMITTER_HEADER__