#ifndef __SGP_SPARKRANDOMEMITTER_HEADER__
#define __SGP_SPARKRANDOMEMITTER_HEADER__



// An Emitter that emits in a random direction

class SGP_API RandomEmitter : public Emitter
{
	SPARK_IMPLEMENT_REGISTERABLE(RandomEmitter)

public:
	static RandomEmitter* create();

private:
	virtual void generateVelocity(Particle& particle, float speed) const;
};


inline RandomEmitter* RandomEmitter::create()
{
	RandomEmitter* obj = new RandomEmitter;
	registerObject(obj);
	return obj;
}

#endif		// __SGP_SPARKRANDOMEMITTER_HEADER__