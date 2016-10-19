#ifndef __SGP_SPARKSTATICEMITTER_HEADER__
#define __SGP_SPARKSTATICEMITTER_HEADER__


// An Emitter that emits particles with no initial velocity

class StaticEmitter : public Emitter
{
	SPARK_IMPLEMENT_REGISTERABLE(StaticEmitter)

public:
	static StaticEmitter* create();

private:
	virtual void generateVelocity(Particle& particle, float speed) const;
};


inline StaticEmitter* StaticEmitter::create()
{
	StaticEmitter* obj = new StaticEmitter;
	registerObject(obj);
	return obj;
}

inline void StaticEmitter::generateVelocity(Particle& particle, float ) const
{
	particle.velocity().Set(0.0f,0.0f,0.0f); // no initial velocity
}

#endif		// __SGP_SPARKSTATICEMITTER_HEADER__