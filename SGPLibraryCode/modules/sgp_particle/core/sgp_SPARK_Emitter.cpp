

Emitter::Emitter() : Registerable(), Transformable(),
	zone( &getDefaultZone() ),
	full(true),
	tank(-1),
	flow(0.0f),
	forceMin(0.0f),
	forceMax(0.0f),
	fraction( random(0.0f,1.0f) ),
	active(true)
{}

Zone& Emitter::getDefaultZone()
{
	static PointZone defaultZone;
	return defaultZone;
}

void Emitter::registerChildren(bool registerAll)
{
	Registerable::registerChildren(registerAll);
	registerChild(zone, registerAll);
}

void Emitter::copyChildren(const Registerable& object, bool createBase)
{
	const Emitter& emitter = dynamic_cast<const Emitter&>(object);
	Registerable::copyChildren(emitter, createBase);
	zone = dynamic_cast<Zone*>(copyChild(emitter.zone, createBase));	
}
	
void Emitter::destroyChildren(bool keepChildren)
{
	destroyChild(zone, keepChildren);
	Registerable::destroyChildren(keepChildren);
}

Registerable* Emitter::findByName(const String& name)
{
	Registerable* object = Registerable::findByName(name);
	if (object != NULL)
		return object;

	return zone->findByName(name);
}

void Emitter::changeTank(int32 deltaTank)
{
	if(tank >= 0)
	{
		tank += deltaTank;
		if(tank < 0)
			tank = 0;
	}
}

void Emitter::changeFlow(float deltaFlow)
{
	if(flow >= 0.0f)
	{
		flow += deltaFlow;
		if(flow < 0.0f)
			flow = 0.0f;
	}
}

void Emitter::setZone(Zone* _zone, bool full)
{
	decrementChildReference(this->zone);
	incrementChildReference(_zone);

	if(_zone == NULL)
		_zone = &getDefaultZone();

	this->zone = _zone;
	this->full = full;
}

uint32 Emitter::updateNumber(float deltaTime)
{
	int32 nbBorn;
	if(flow < 0.0f)
	{
		nbBorn = jmax(0, tank);
		tank = 0;
	}
	else if(tank != 0)
	{
		fraction += flow * deltaTime;
		nbBorn = static_cast<int>(fraction);
		if(tank >= 0)
		{
			nbBorn = jmin(tank, nbBorn);
			tank -= nbBorn;
		}
		fraction -= nbBorn;
	}
	else
		nbBorn = 0;

	return static_cast<uint32>(nbBorn);
}
