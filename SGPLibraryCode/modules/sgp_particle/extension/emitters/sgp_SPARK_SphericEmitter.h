#ifndef __SGP_SPARKSPHERICEMITTER_HEADER__
#define __SGP_SPARKSPHERICEMITTER_HEADER__


/**
* An Emitter that emits particles in a portion of sphere
*
* This Emitter can emit particles in a spheric way.
* To do that 2 angles and a direction Vector3D can be parametered :

	The direction Vector3D defines the direction of the emitter.
	The angles defines the area in between which wil be emitted the particles velocities.
	
* Here are a few examples :

	0 and 2 * PI		will define a complete sphere ie equivalent to a RandomEmitter
	0 and 0				will define a Emitter equivalent to a StraightEmitter
	PI and PI			will define a disk
	PI / 2 and PI / 2	will define a Cone of angle PI / 2
		...
*/
class SGP_API SphericEmitter : public Emitter
{
	SPARK_IMPLEMENT_REGISTERABLE(SphericEmitter)

public:


	/**
	* Constructor of SphericEmitter
	*	direction : the direction of the SphericEmitter
	*	angleA : the first angle in radians of the SphericEmitter
	*	angleB : the second angle in radians of the SphericEmitter
	*/
	SphericEmitter(const Vector3D& direction = Vector3D(0.0f,0.0f,-1.0f), float angleA = 0.0f, float angleB = 0.0f);


	static SphericEmitter* create(const Vector3D& direction = Vector3D(0.0f,0.0f,-1.0f), float angleA = 0.0f, float angleB = 0.0f);

	/////////////
	// Setters //
	/////////////

	/**
	* Sets the direction of this SphericEmitter

	* Note that it is not necessary to provide a normalized Vector3D.
	* This Vector3D only indicates a direction, its norm does not matter.
	*/
	void setDirection(const Vector3D& direction);

	/**
	* Sets the angles of this SphericEmitter
	*
	* Note that angles are clamped between 0 and 2 * PI
	* AngleA does not have to be inferior to angleB, it has no importance as angles are sorted within the method.
	*
	*	angleA : the first angle in radians of this SphericEmitter
	*	angleB : the second angle in radians of this SphericEmitter
	*/
	void setAngles(float angleA, float angleB);


	const Vector3D& getDirection() const;
	const Vector3D& getTransformedDirection() const;
	float getAngleMin() const;
	float getAngleMax() const;

protected:
	virtual void innerUpdateTransform();

private:
	Vector3D direction;
	Vector3D tDirection; // transformed direction

	float angleMin;
	float angleMax;

	float cosAngleMin;
	float cosAngleMax;

	float matrix[9];

	void computeMatrix();

	virtual void generateVelocity(Particle& particle, float speed) const;
};


inline SphericEmitter* SphericEmitter::create(const Vector3D& _direction, float _angleA, float _angleB)
{
	SphericEmitter* obj = new SphericEmitter(_direction, _angleA, _angleB);
	registerObject(obj);
	return obj;
}

inline const Vector3D& SphericEmitter::getDirection() const
{
	return direction;
}

inline const Vector3D& SphericEmitter::getTransformedDirection() const
{
	return tDirection;
}

inline float SphericEmitter::getAngleMin() const
{
	return angleMin;
}

inline float SphericEmitter::getAngleMax() const
{
	return angleMax;
}

#endif		// __SGP_SPARKSPHERICEMITTER_HEADER__