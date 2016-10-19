

Vector3D System::cameraPosition = Vector3D(0,0,0);

StepMode System::stepMode = STEP_REAL;

float System::constantStep = 0.0f;
float System::minStep = 0.0f;
float System::maxStep = 0.0f;

bool System::clampStepEnabled = false;
float System::clampStep = 1.0f;

System::System() : Registerable(), Transformable(),
	NumberOfParticles(0),
	boundingBoxEnabled(false),
	boundingBox(Vector3D(0,0,0), Vector3D(0,0,0)),
	deltaStep(0.0f)
{}

void System::registerChildren(bool registerAll)
{
	Registerable::registerChildren(registerAll);

	for(Group** it = groups.begin(); it < groups.end(); ++it)
		registerChild(*it, registerAll);
}

void System::copyChildren(const Registerable& object, bool createBase)
{
	const System& system = dynamic_cast<const System&>(object);
	Registerable::copyChildren(system, createBase);

	// we clear the copies of pointers pushed in the vectors by the copy constructor
	groups.clear();

	for( Group** it = system.groups.begin(); it < system.groups.end(); ++it )
		groups.add(dynamic_cast<Group*>( copyChild(*it, createBase) ));
}

void System::destroyChildren(bool keepChildren)
{
	for(Group** it = groups.begin(); it < groups.end(); ++it )
		destroyChild(*it, keepChildren);

	Registerable::destroyChildren(keepChildren);
}

void System::addGroup(Group* group)
{
	incrementChildReference(group);
	groups.add(group);
	NumberOfParticles += group->getNumberOfParticles();
}

void System::removeGroup(Group* group)
{
	int FoundIdx = groups.indexOf(group);
	if( FoundIdx != -1 )
	{
		decrementChildReference(group);
		groups.remove(FoundIdx);
	}
}

uint32 System::computeNumOfParticles()
{
	NumberOfParticles = 0;
	for( Group** it = groups.begin(); it < groups.end(); ++it )
		NumberOfParticles += (*it)->getNumberOfParticles();
	return NumberOfParticles;
}

bool System::innerUpdate(float deltaTimeInSecond)
{
	NumberOfParticles = 0;
	bool isAlive = false;

	bool hasGroupsWithAABB = false;
	if( boundingBoxEnabled )
	{
		boundingBox.vcMin.Set(0.0f,0.0f,0.0f);
		boundingBox.vcMax.Set(0.0f,0.0f,0.0f);
		boundingBox.vcCenter.Set(0.0f,0.0f,0.0f);
	}

	for( Group** it = groups.begin(); it < groups.end(); ++it )
	{
		isAlive |= (*it)->update(deltaTimeInSecond);
		NumberOfParticles += (*it)->getNumberOfParticles();

		if( boundingBoxEnabled && (*it)->isAABBComputingEnabled() )
		{
			const AABBox groupAABB = (*it)->getAABBox();

			if( boundingBox.vcMin.x > groupAABB.vcMin.x )
				boundingBox.vcMin.x = groupAABB.vcMin.x;
			if( boundingBox.vcMin.y > groupAABB.vcMin.y )
				boundingBox.vcMin.y = groupAABB.vcMin.y;
			if( boundingBox.vcMin.z > groupAABB.vcMin.z )
				boundingBox.vcMin.z = groupAABB.vcMin.z;
			if( boundingBox.vcMax.x < groupAABB.vcMax.x )
				boundingBox.vcMax.x = groupAABB.vcMax.x;
			if( boundingBox.vcMax.y < groupAABB.vcMax.y )
				boundingBox.vcMax.y = groupAABB.vcMax.y;
			if( boundingBox.vcMax.z < groupAABB.vcMax.z )
				boundingBox.vcMax.z = groupAABB.vcMax.z;

			boundingBox.vcCenter = (boundingBox.vcMax + boundingBox.vcMin) / 2.0f;

			hasGroupsWithAABB = true;
		}
	}

	if( !boundingBoxEnabled || !hasGroupsWithAABB )
	{
		boundingBox.vcMin.Set(0.0f,0.0f,0.0f);
		boundingBox.vcMax.Set(0.0f,0.0f,0.0f);
		boundingBox.vcCenter.Set(0.0f,0.0f,0.0f);
	}

	return isAlive;
}

bool System::update(float deltaTimeInSecond)
{
	if( clampStepEnabled && (deltaTimeInSecond > clampStep) )
		deltaTimeInSecond = clampStep;

	if( stepMode != STEP_REAL )
	{
		deltaTimeInSecond += deltaStep;

		float updateStep;
		if( stepMode == STEP_ADAPTIVE )
		{
			if( deltaTimeInSecond > maxStep )
				updateStep = maxStep;
			else if( deltaTimeInSecond < minStep )
				updateStep = minStep;
			else
			{
				deltaStep = 0.0f;
				return innerUpdate(deltaTimeInSecond);
			}
		}
		else
			updateStep = constantStep;

		bool isAlive = true;
		while( deltaTimeInSecond >= updateStep )
		{
			if( isAlive && !innerUpdate(updateStep) )
				isAlive = false;
			deltaTimeInSecond -= updateStep;
		}
		deltaStep = deltaTimeInSecond;
		return isAlive;

	}	
	else
		return innerUpdate(deltaTimeInSecond);
}


void System::render() const
{
	for(Group** it = groups.begin(); it < groups.end(); ++it )
		(*it)->render();
}

void System::grow(float time, float step)
{
	if( step <= 0.0f )
		step = time;

	while( time > 0.0f )
	{
		float currentStep = time > step ? step : time;
		update(currentStep);
		time -= currentStep;
	}
}

void System::empty()
{
	for( Group** it = groups.begin(); it < groups.end(); ++it )
		(*it)->empty();
	NumberOfParticles = 0;
}

void System::setCameraPosition(const Vector3D& _cameraPosition)
{
	System::cameraPosition = _cameraPosition;
}

void System::setClampStep(bool enableClampStep, float clamp)
{
	clampStepEnabled = enableClampStep;
	clampStep = clamp;
}

void System::useConstantStep(float _constantStep)
{
	stepMode = STEP_CONSTANT;
	System::constantStep = _constantStep;
}

void System::useAdaptiveStep(float _minStep, float _maxStep)
{
	stepMode = STEP_ADAPTIVE;
	System::minStep = _minStep;
	System::maxStep = _maxStep;
}

void System::useRealStep()
{
	stepMode = STEP_REAL;
}

const Vector3D& System::getCameraPosition()
{
	return cameraPosition;
}

StepMode System::getStepMode()
{
	return stepMode;
}

void System::sortParticles()
{
	for( Group** it = groups.begin(); it < groups.end(); ++it )
		(*it)->sortParticles();
}

void System::computeDistances()
{
	for( Group** it = groups.begin(); it < groups.end(); ++it )
		(*it)->computeDistances();
}

void System::computeAABB()
{
	if(boundingBoxEnabled)
	{
		boundingBox.vcMin.Set(0.0f,0.0f,0.0f);
		boundingBox.vcMax.Set(0.0f,0.0f,0.0f);
		boundingBox.vcCenter.Set(0.0f,0.0f,0.0f);
	}

	bool hasGroupsWithAABB = false;
	for( Group** it = groups.begin(); it < groups.end(); ++it )
	{
		(*it)->computeAABB();

		if( boundingBoxEnabled && (*it)->isAABBComputingEnabled() )
		{
			const AABBox groupAABB = (*it)->getAABBox();

			if( boundingBox.vcMin.x > groupAABB.vcMin.x )
				boundingBox.vcMin.x = groupAABB.vcMin.x;
			if( boundingBox.vcMin.y > groupAABB.vcMin.y )
				boundingBox.vcMin.y = groupAABB.vcMin.y;
			if( boundingBox.vcMin.z > groupAABB.vcMin.z )
				boundingBox.vcMin.z = groupAABB.vcMin.z;
			if( boundingBox.vcMax.x < groupAABB.vcMax.x )
				boundingBox.vcMax.x = groupAABB.vcMax.x;
			if( boundingBox.vcMax.y < groupAABB.vcMax.y )
				boundingBox.vcMax.y = groupAABB.vcMax.y;
			if( boundingBox.vcMax.z < groupAABB.vcMax.z )
				boundingBox.vcMax.z = groupAABB.vcMax.z;

			boundingBox.vcCenter = (boundingBox.vcMax + boundingBox.vcMin) / 2.0f;

			hasGroupsWithAABB = true;
		}
	}

	if( !boundingBoxEnabled || !hasGroupsWithAABB )
	{
		const Vector3D pos( getWorldTransform()._41, getWorldTransform()._42, getWorldTransform()._43 );
		boundingBox.vcMin = boundingBox.vcMax = boundingBox.vcCenter = pos;
	}
}

Registerable* System::findByName(const String& name)
{
	Registerable* object = Registerable::findByName(name);
	if( object != NULL )
		return object;

	for( Group** it = groups.begin(); it < groups.end(); ++it )
	{
		object = (*it)->findByName(name);
		if( object != NULL )
			return object;
	}

	return NULL;
}

void System::propagateUpdateTransform()
{
	for( Group** it = groups.begin(); it < groups.end(); ++it )
		(*it)->updateTransform(this);
}
