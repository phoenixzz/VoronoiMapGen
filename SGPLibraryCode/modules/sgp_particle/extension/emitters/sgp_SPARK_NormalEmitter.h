#ifndef __SGP_SPARKNORMALEMITTER_HEADER__
#define __SGP_SPARKNORMALEMITTER_HEADER__

/**
* An Emitter that emits particles following a Zone normals
*
* The Zone used to derive the direction of emission can either be the Emitter Zone
* or another Zone that can be set with setNormalZone(Zone*).
* If the normal zone is NULL the emitter Zone is used.
*/
class SGP_API NormalEmitter : public Emitter
{
	SPARK_IMPLEMENT_REGISTERABLE(NormalEmitter)

public:



	/**
	* Constructor of NormalEmitter
	*	normalZone : the Zone used to compute normals (NULL to used the Emitter Zone)
	*	inverted : true to invert the normals, false otherwise
	*/
	NormalEmitter(Zone* normalZone = NULL, bool inverted = false);

	static NormalEmitter* create(Zone* normalZone = NULL, bool inverted = false);
		


	/**
	* Sets whether normals are inverted or not
	*/
	void setInverted(bool inverted);

	/**
	* the Zone used to compute normals
	*
	* Note that if the normal zone is NULL, the Emitter Zone is used.
	*/
	void setNormalZone(Zone* zone);


	bool isInverted() const;		

	Zone* getNormalZone() const;



	virtual Registerable* findByName(const String& name);

protected:
	virtual void registerChildren(bool registerAll);
	virtual void copyChildren(const Registerable& object, bool createBase);
	virtual void destroyChildren(bool keepChildren);

private:
	bool inverted;
	Zone* normalZone;

	virtual void generateVelocity(Particle& particle, float speed) const;
};


inline NormalEmitter* NormalEmitter::create(Zone* _normalZone, bool _inverted)
{
	NormalEmitter* obj = new NormalEmitter(_normalZone, _inverted);
	registerObject(obj);
	return obj;
}

inline void NormalEmitter::setInverted(bool _inverted)
{
	this->inverted = _inverted;
}

inline bool NormalEmitter::isInverted() const
{
	return inverted;
}

inline Zone* NormalEmitter::getNormalZone() const
{
	return normalZone;
}	

#endif		// __SGP_SPARKNORMALEMITTER_HEADER__