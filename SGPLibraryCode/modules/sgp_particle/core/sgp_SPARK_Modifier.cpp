

Vector3D Modifier::intersection = Vector3D(0,0,0);
Vector3D Modifier::normal = Vector3D(0,0,0);

Modifier::Modifier(uint32 availableTriggers, ModifierTrigger trigger, bool needsIntersection, bool needsNormal, Zone* zone) :
	Registerable(),	Transformable(), BufferHandler(),
	availableTriggers(availableTriggers),
	trigger(trigger),
	zone(zone),
	needsIntersection(needsIntersection),
	needsNormal(needsNormal),
	full(false),
	active(true),
	local(false)
{}

void Modifier::registerChildren(bool registerAll)
{
	Registerable::registerChildren(registerAll);
	registerChild(zone, registerAll);
}

void Modifier::copyChildren(const Registerable& object, bool createBase)
{
	const Modifier& modifier = dynamic_cast<const Modifier&>(object);
	Registerable::copyChildren(modifier, createBase);
	zone = dynamic_cast<Zone*>(copyChild(modifier.zone, createBase));
}

void Modifier::destroyChildren(bool keepChildren)
{
	destroyChild(zone, keepChildren);
	Registerable::destroyChildren(keepChildren);
}

Registerable* Modifier::findByName(const String& name)
{
	Registerable* object = Registerable::findByName(name);
	if((object != NULL) || (zone == NULL))
		return object;

	return zone->findByName(name);
}

void Modifier::setZone(Zone* _zone, bool full)
{
	decrementChildReference(this->zone);
	incrementChildReference(_zone);

	this->zone = _zone;
	this->full = full;
}

bool Modifier::setTrigger(ModifierTrigger _trigger)
{
	if( (_trigger & availableTriggers) != 0 )
	{
		this->trigger = _trigger;
		return true;
	}

	return false;
}

void Modifier::beginProcess(Group& group)
{
	savedActive = active;
		
	if( !active )
		return;
		
	if( !prepareBuffers(group) )
		active = false; // if buffers of the modifier in the group are not ready, the modifier is made incative for the frame
}