

NormalEmitter::NormalEmitter(Zone* normalZone, bool inverted) :
	Emitter(),
	normalZone(normalZone),
	inverted(inverted)
{}

void NormalEmitter::registerChildren(bool registerAll)
{
	Emitter::registerChildren(registerAll);
	registerChild(normalZone,registerAll);
}

void NormalEmitter::copyChildren(const Registerable& object, bool createBase)
{
	const NormalEmitter& emitter = dynamic_cast<const NormalEmitter&>(object);
	Emitter::copyChildren(emitter,createBase);
	normalZone = dynamic_cast<Zone*>(copyChild(emitter.normalZone,createBase));	
}
	
void NormalEmitter::destroyChildren(bool keepChildren)
{
	destroyChild(normalZone,keepChildren);
	Emitter::destroyChildren(keepChildren);
}

Registerable* NormalEmitter::findByName(const String& name)
{
	Registerable* object = Emitter::findByName(name);
	if( (object != NULL) || (normalZone == NULL) )
		return object;

	return normalZone->findByName(name);
}

void NormalEmitter::setNormalZone(Zone* zone)
{
	decrementChildReference(normalZone);
	incrementChildReference(zone);

	normalZone = zone;
}

void NormalEmitter::generateVelocity(Particle& particle, float speed) const
{
	if( inverted ) 
		speed = -speed;
	const Zone* zone = (normalZone == NULL ? getZone() : normalZone);
	particle.velocity() = zone->computeNormal(particle.position()) * speed;
}