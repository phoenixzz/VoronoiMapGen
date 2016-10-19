

bool Group::bufferManagement = true;

Group::Group(Model* m, uint32 capacity) : Registerable(), Transformable(),
	model(m != NULL ? m : &getDefaultModel()),
	renderer(NULL),
	friction(0.0f),
	gravity(Vector3D(0,0,0)),
	pool(capacity),
	sortingEnabled(false),
	distanceComputationEnabled(false),
	NumOfBufferedParticles(0),
	fupdate(NULL),
	fbirth(NULL),
	fdeath(NULL),
	boundingBoxEnabled(false)
{
	particleData = new Particle::ParticleData[capacity];
	particleCurrentParams = new float[capacity * model->getSizeOfParticleCurrentArray()];
	particleExtendedParams = new float[capacity * model->getSizeOfParticleExtendedArray()];
}

Group::Group(const Group& group) : Registerable(group), Transformable(group),
	model(group.model),
	renderer(group.renderer),
	friction(group.friction),
	gravity(group.gravity),
	pool(group.pool),
	sortingEnabled(group.sortingEnabled),
	distanceComputationEnabled(group.distanceComputationEnabled),
	creationBuffer(group.creationBuffer),
	NumOfBufferedParticles(group.NumOfBufferedParticles),
	fupdate(group.fupdate),
	fbirth(group.fbirth),
	fdeath(group.fdeath),
	boundingBoxEnabled(group.boundingBoxEnabled),
	emitters(group.emitters),
	modifiers(group.modifiers)
{
	activeModifiers.insertMultiple(0, NULL, group.activeModifiers.size());

	particleData = new Particle::ParticleData[pool.getSizeOfReserved()];
	particleCurrentParams = new float[pool.getSizeOfReserved() * model->getSizeOfParticleCurrentArray()];
	particleExtendedParams = new float[pool.getSizeOfReserved() * model->getSizeOfParticleExtendedArray()];

	memcpy(particleData, group.particleData, pool.getSizeOfTotal() * sizeof(Particle::ParticleData));
	memcpy(particleCurrentParams, group.particleCurrentParams, pool.getSizeOfTotal() * sizeof(float) * model->getSizeOfParticleCurrentArray());
	memcpy(particleExtendedParams, group.particleExtendedParams, pool.getSizeOfTotal() * sizeof(float) * model->getSizeOfParticleExtendedArray());

	for( Particle* it = pool.begin(); it < pool.endInactive(); ++it )
	{
		it->group = this;
		it->data = particleData + it->index;
		it->currentParams = particleCurrentParams + it->index * model->getSizeOfParticleCurrentArray();
		it->extendedParams = particleExtendedParams + it->index * model->getSizeOfParticleExtendedArray();
	}
}

Group::~Group()
{
	delete[] particleData;
	particleData = NULL;
	delete[] particleCurrentParams;
	particleCurrentParams = NULL;
	delete[] particleExtendedParams;
	particleExtendedParams = NULL;

	// destroys additional buffers
	destroyAllBuffers();
}

void Group::registerChildren(bool registerAll)
{
	Registerable::registerChildren(registerAll);

	registerChild(model, registerAll);
	registerChild(renderer, registerAll);

	for( Emitter** it = emitters.begin(); it < emitters.end(); ++it )
		registerChild(*it, registerAll);
	for( Modifier** it = modifiers.begin(); it < modifiers.end(); ++it )
		registerChild(*it, registerAll);
}

void Group::copyChildren(const Registerable& object, bool createBase)
{
	const Group& group = dynamic_cast<const Group&>(object);
	Registerable::copyChildren(group, createBase);

	model = dynamic_cast<Model*>(copyChild(group.model, createBase));
	renderer = dynamic_cast<Renderer*>(copyChild(group.renderer, createBase));

	// we clear the copies of pointers pushed in the vectors by the copy constructor
	emitters.clear();
	modifiers.clear();

	for( Emitter** it = group.emitters.begin(); it < group.emitters.end(); ++it )
		emitters.add(dynamic_cast<Emitter*>(copyChild(*it, createBase)));
	for( Modifier** it = group.modifiers.begin(); it < group.modifiers.end(); ++it )
		modifiers.add(dynamic_cast<Modifier*>(copyChild(*it, createBase)));
}

void Group::destroyChildren(bool keepChildren)
{
	destroyChild(model, keepChildren);
	destroyChild(renderer, keepChildren);

	for( Emitter** it = emitters.begin(); it < emitters.end(); ++it )
		destroyChild(*it, keepChildren);
	for( Modifier** it = modifiers.begin(); it < modifiers.end(); ++it )
		destroyChild(*it, keepChildren);

	Registerable::destroyChildren(keepChildren);
}

Registerable* Group::findByName(const String& name)
{
	Registerable* object = Registerable::findByName(name);
	if(object != NULL)
		return object;

	object = model->findByName(name);
	if(object != NULL)
		return object;

	if(renderer != NULL)
	{
		object = renderer->findByName(name);
		if(object != NULL)
			return object;
	}

	for( Emitter** it = emitters.begin(); it < emitters.end(); ++it )
	{
		object = (*it)->findByName(name);
		if(object != NULL)
			return object;
	}

	for( Modifier** it = modifiers.begin(); it < modifiers.end(); ++it )
	{
		object = (*it)->findByName(name);
		if(object != NULL)
			return object;
	}

	return NULL;
}

void Group::setModel(Model* newmodel)
{
	if( !newmodel ) 
		newmodel = &getDefaultModel();

	if( model == newmodel ) 
		return;

	// empty and change model
	empty();

	decrementChildReference(model);
	incrementChildReference(newmodel);
	model = newmodel;

	// recreate data
	delete[] particleData;
	delete[] particleCurrentParams;
	delete[] particleExtendedParams;

	particleData = new Particle::ParticleData[pool.getSizeOfReserved()];
	particleCurrentParams = new float[pool.getSizeOfReserved() * model->getSizeOfParticleCurrentArray()];
	particleExtendedParams = new float[pool.getSizeOfReserved() * model->getSizeOfParticleExtendedArray()];

	pool.clear();

	// Destroys all the buffers
	destroyAllBuffers();
}

void Group::setRenderer(Renderer* _renderer)
{
	decrementChildReference(this->renderer);
	incrementChildReference(_renderer);

	if( bufferManagement && (_renderer != this->renderer) )
	{
		if( this->renderer != NULL ) 
			this->renderer->destroyBuffers(*this);
		if( _renderer != NULL ) 
			_renderer->createBuffers(*this);
	}

	this->renderer = _renderer;
}

void Group::addEmitter(Emitter* emitter)
{
	if( emitter == NULL )
		return;

	// Checks if the emitter is already in the group 
	if( emitters.contains(emitter) )
		return;

	incrementChildReference(emitter);
	emitters.add(emitter);
}

void Group::removeEmitter(Emitter* emitter)
{
	int FoundIdx = emitters.indexOf(emitter);
	if( FoundIdx != -1 )
	{
		decrementChildReference(emitter);
		emitters.remove(FoundIdx);
	}
}

void Group::addModifier(Modifier* modifier)
{
	if(modifier == NULL)
		return;

	incrementChildReference(modifier);

	if(bufferManagement)
		modifier->createBuffers(*this);

	modifiers.add(modifier);
}

void Group::removeModifier(Modifier* modifier)
{
	int FoundIdx = modifiers.indexOf(modifier);
	if( FoundIdx != -1 )
	{
		decrementChildReference(modifier);

		if(bufferManagement)
			modifier->destroyBuffers(*this);

		modifiers.remove(FoundIdx);
	}
}

bool Group::update(float deltaTimeInSeconds)
{
	uint32 nbManualBorn = NumOfBufferedParticles;
	uint32 nbAutoBorn = 0;

	bool hasActiveEmitters = false;

	// Updates emitters
	activeEmitters.clear();

	Emitter** endIt = emitters.end();
	for( Emitter** it = emitters.begin(); it < endIt; ++it )
	{
		if( (*it)->isActive() )
		{
			uint32 nb = (*it)->updateNumber(deltaTimeInSeconds);
			if( nb > 0 )
			{
				EmitterData data = {*it, nb};
				activeEmitters.add(data);
				nbAutoBorn += nb;
			}
		}

		hasActiveEmitters |= !((*it)->isSleeping());
	}

	EmitterData* emitterIt = activeEmitters.begin();

	uint32 nbBorn = nbAutoBorn + nbManualBorn;

	// Inits bounding box
	if(boundingBoxEnabled)
	{
		boundingBoxAABB.vcMin.Set(0, 0, 0);
		boundingBoxAABB.vcMax.Set(0, 0, 0);
		boundingBoxAABB.vcCenter.Set(0, 0, 0);
	}

	// Prepare modifiers for processing
	activeModifiers.clear();
	for( Modifier** it = modifiers.begin(); it < modifiers.end(); ++it )
	{
		(*it)->beginProcess(*this);
		if( (*it)->isActive() )
			activeModifiers.add(*it);
	}

	// Updates particles
	for( uint32 i = 0; i < pool.getSizeOfActive(); ++i )
	{
		if( pool[i].update(deltaTimeInSeconds) || ((fupdate != NULL) && ((*fupdate)(pool[i], deltaTimeInSeconds))) )
		{
			if( fdeath != NULL )
				(*fdeath)(pool[i]);

			if( nbBorn > 0 )
			{
				pool[i].init();
				launchParticle(pool[i], emitterIt, nbManualBorn);
				--nbBorn;
			}
			else
			{
				particleData[i].sqrDist = 0.0f;
				pool.makeInactive(i);
				--i;
			}
		}
		else
		{
			if( boundingBoxEnabled )
				updateAABB(pool[i]);

			if( distanceComputationEnabled )
				pool[i].computeSqrDist();
		}
	}

	// Terminates modifiers processing
	for( Modifier** it = modifiers.begin(); it < modifiers.end(); ++it )
		(*it)->endProcess(*this);

	// Emits new particles if some left
	for( uint32 i = nbBorn; i > 0; --i )
		pushParticle(emitterIt, nbManualBorn);

	// Sorts particles if enabled
	if( sortingEnabled && (pool.getSizeOfActive() > 1) )
		sortParticles(0, pool.getSizeOfActive() - 1);

	if( !boundingBoxEnabled || (pool.getSizeOfActive() == 0) )
	{
		boundingBoxAABB.vcMin.Set(0, 0, 0);
		boundingBoxAABB.vcMax.Set(0, 0, 0);
		boundingBoxAABB.vcCenter.Set(0, 0, 0);
	}

	return hasActiveEmitters || (pool.getSizeOfActive() > 0);
}

void Group::pushParticle(EmitterData* &emitterIt, uint32& nbManualBorn)
{
	Particle* ptr = pool.makeActive();
	if( ptr == NULL )
	{
		if(pool.getSizeOfEmpty() > 0)
		{
			Particle p(this, pool.getSizeOfActive());
			launchParticle(p, emitterIt, nbManualBorn);
			pool.pushActive(p);
		}
		else if(nbManualBorn > 0)
			popNextManualAdding(nbManualBorn);
	}
	else
	{
		ptr->init();
		launchParticle(*ptr, emitterIt, nbManualBorn);
	}
}

void Group::launchParticle(Particle& p, EmitterData* &emitterIt, uint32& nbManualBorn)
{
	if(nbManualBorn == 0)
	{
		emitterIt->emitter->emit(p);
		if(--emitterIt->NumOfParticles == 0)
			++emitterIt;
	}
	else
	{
		CreationData creationData = creationBuffer.getFirst();

		if(creationData.zone != NULL)
			creationData.zone->generatePosition(p, creationData.full);
		else
			p.position() = creationData.position;

		if(creationData.emitter != NULL)
			creationData.emitter->generateVelocity(p);
		else
			p.velocity() = creationData.velocity;

		popNextManualAdding(nbManualBorn);
	}

	// Resets old position
	p.oldPosition() = p.position();

	// first parameter interpolation
	// must be here so that the velocity has already been initialized
	p.interpolateParameters();

	if(fbirth != NULL)
		(*fbirth)(p);

	if(boundingBoxEnabled)
		updateAABB(p);

	if(distanceComputationEnabled)
		p.computeSqrDist();
}


void Group::render()
{
	if( (renderer == NULL) || (!renderer->isActive()) )
		return;

	renderer->render(*this);
}

void Group::empty()
{
	for(uint32 i = 0; i < pool.getSizeOfActive(); ++i)
		particleData[i].sqrDist = 0.0f;

	pool.makeAllInactive();
	creationBuffer.clear();
	NumOfBufferedParticles = 0;
}

void Group::flushAddedParticles()
{
	uint32 nbManualBorn = NumOfBufferedParticles;
	EmitterData* emitterIt = NULL; // dummy emitterIt because we dont care
	while(nbManualBorn > 0)
	{
		pushParticle(emitterIt, nbManualBorn);
	}
}

float Group::addParticles(const Vector3D& start, const Vector3D& end, Emitter* emitter, float step, float offset)
{
	if( (step <= 0.0f) || (offset < 0.0f) )
		return 0.0f;

	Vector3D displacement = end - start;
	float totalDist = displacement.GetLength();

	while(offset < totalDist)
	{
		Vector3D position = start;
		position += displacement * offset / totalDist;
		addParticles(1, position, Vector3D(0,0,0), NULL, emitter);
		offset += step;
	}

	return offset - totalDist;
}

float Group::addParticles(const Vector3D& start, const Vector3D& end, const Vector3D& velocity, float step, float offset)
{
	if( (step <= 0.0f) || (offset < 0.0f) )
		return 0.0f;

	Vector3D displacement = end - start;
	float totalDist = displacement.GetLength();

	while(offset < totalDist)
	{
		Vector3D position = start;
		position += displacement * (offset / totalDist);
		addParticles(1, position, velocity, NULL, NULL);
		offset += step;
	}

	return offset - totalDist;
}

void Group::addParticles(uint32 number, const Vector3D& position, const Vector3D& velocity, const Zone* zone, Emitter* emitter, bool full)
{
	if(number == 0)
		return;

	CreationData data = {number, position, velocity, zone, emitter, full};
	creationBuffer.add(data);
	NumOfBufferedParticles += number;
}

void Group::addParticles(uint32 number, Emitter* emitter)
{
	addParticles(number, Vector3D(0,0,0), Vector3D(0,0,0), emitter->getZone(), emitter, emitter->isFullZone());
}

void Group::addParticles(const Zone* zone, Emitter* emitter, float deltaTime, bool full)
{
	addParticles(emitter->updateNumber(deltaTime), Vector3D(0,0,0), Vector3D(0,0,0), zone, emitter, full);
}

void Group::addParticles(const Vector3D& position, Emitter* emitter, float deltaTime)
{
	addParticles(emitter->updateNumber(deltaTime), position, Vector3D(0,0,0), NULL, emitter);
}

void Group::addParticles(Emitter* emitter, float deltaTime)
{
	addParticles(emitter->updateNumber(deltaTime), Vector3D(0,0,0), Vector3D(0,0,0), emitter->getZone(), emitter, emitter->isFullZone());
}

void Group::sortParticles()
{
	computeDistances();

	if( sortingEnabled )
		sortParticles(0, pool.getSizeOfActive() - 1);
}

void Group::computeDistances()
{
	if( !distanceComputationEnabled )
		return;

	Particle* endIt = pool.end();
	for( Particle* it = pool.begin(); it < endIt; ++it )
		it->computeSqrDist();
}

void Group::computeAABB()
{

	boundingBoxAABB.vcMin.Set(0.0f,0.0f,0.0f);
	boundingBoxAABB.vcMax.Set(0.0f,0.0f,0.0f);
	boundingBoxAABB.vcCenter.Set(0.0f,0.0f,0.0f);

	if( !boundingBoxEnabled || (pool.getSizeOfActive() == 0) )
	{
		return;
	}


	Particle* endIt = pool.end();
	for( Particle* it = pool.begin(); it < endIt; ++it )
		updateAABB(*it);
}

void Group::reallocate(uint32 capacity)
{
	if( capacity > pool.getSizeOfReserved() )
	{
		pool.reallocate(capacity);

		Particle::ParticleData* newData = new Particle::ParticleData[pool.getSizeOfReserved()];
		float* newCurrentParams = new float[pool.getSizeOfReserved() * model->getSizeOfParticleCurrentArray()];
		float* newExtendedParams = new float[pool.getSizeOfReserved() * model->getSizeOfParticleExtendedArray()];

		memcpy(newData, particleData,pool.getSizeOfTotal() * sizeof(Particle::ParticleData));
		memcpy(newCurrentParams, particleCurrentParams, pool.getSizeOfTotal() * sizeof(float) * model->getSizeOfParticleCurrentArray());
		memcpy(newExtendedParams, particleExtendedParams, pool.getSizeOfTotal() * sizeof(float) * model->getSizeOfParticleExtendedArray());

		delete[] particleData;
		delete[] particleCurrentParams;
		delete[] particleExtendedParams;

		particleData = newData;
		particleCurrentParams = newCurrentParams;
		particleExtendedParams = newExtendedParams;

		for( Particle* it = pool.begin(); it < pool.endInactive(); ++it )
		{
			it->group = this;
			it->data = particleData + it->index;
			it->currentParams = particleCurrentParams + it->index * model->getSizeOfParticleCurrentArray();
			it->extendedParams = particleExtendedParams + it->index * model->getSizeOfParticleExtendedArray();
		}

		// Destroys all the buffers
		destroyAllBuffers();
	}
}

void Group::popNextManualAdding(uint32& nbManualBorn)
{
	--creationBuffer.getReference(0).number;
	--nbManualBorn;
	--NumOfBufferedParticles;
	if( creationBuffer.getReference(0).number <= 0 )
		creationBuffer.remove(0);
}

void Group::updateAABB(const Particle& particle)
{
	const Vector3D& position = particle.position();

	if( boundingBoxAABB.vcMin.x > position.x )
		boundingBoxAABB.vcMin.x = position.x;
	if( boundingBoxAABB.vcMin.y > position.y )
		boundingBoxAABB.vcMin.y = position.y;
	if( boundingBoxAABB.vcMin.z > position.z )
		boundingBoxAABB.vcMin.z = position.z;

	if( boundingBoxAABB.vcMax.x < position.x )
		boundingBoxAABB.vcMax.x = position.x;
	if( boundingBoxAABB.vcMax.y < position.y )
		boundingBoxAABB.vcMax.y = position.y;
	if( boundingBoxAABB.vcMax.z < position.z )
		boundingBoxAABB.vcMax.z = position.z;
}

const void* Group::getParamAddress(ModelParam param) const
{
	return particleCurrentParams + model->getParameterOffset(param);
}

uint32 Group::getParamStride() const
{
	return model->getSizeOfParticleCurrentArray() * sizeof(float);
}

Buffer* Group::createBuffer(uint32 ID, const BufferCreator& creator, uint32 flag, bool swapEnabled) const
{
	destroyBuffer(ID);

	Buffer* buffer = creator.createBuffer(pool.getSizeOfReserved(), *this);

	buffer->flag = flag;
	buffer->swapEnabled = swapEnabled;

	additionalBuffers.set(ID, buffer);
	if( swapEnabled )
		swappableBuffers.add(buffer);

	return buffer;
}

void Group::destroyBuffer(uint32 ID) const
{

	if( additionalBuffers.contains(ID) )
	{
		if( additionalBuffers[ID]->isSwapEnabled() )
			swappableBuffers.removeValue( additionalBuffers[ID] );
		delete additionalBuffers[ID];
		additionalBuffers.set(ID, NULL);

		additionalBuffers.remove(ID);
	}
}

void Group::destroyAllBuffers() const
{
	HashMap<uint32, Buffer*>::Iterator it (additionalBuffers);
	while(it.next())
	{
		delete it.getValue();
	}

	additionalBuffers.clear();
	swappableBuffers.clear();
}

Buffer* Group::getBuffer(uint32 ID, uint32 flag) const
{
	Buffer* buffer = getBuffer(ID);

	if( (buffer != NULL) && (buffer->flag == flag) )
		return buffer;

	return NULL;
}

Buffer* Group::getBuffer(uint32 ID) const
{
	if( additionalBuffers.contains(ID) )
		return additionalBuffers[ID];

	return NULL;
}

void Group::enableBuffersManagement(bool manage)
{
	bufferManagement = manage;
}

bool Group::isBuffersManagementEnabled()
{
	return bufferManagement;
}

void Group::sortParticles(int32 start, int32 end)
{
	if(start < end)
	{
		int32 i = start - 1;
		int32 j = end + 1;
		float pivot = particleData[(start + end) >> 1].sqrDist;
		while(true)
		{
			do ++i;
			while (particleData[i].sqrDist > pivot);
			do --j;
			while (particleData[j].sqrDist < pivot);
			if (i < j)
				swapParticles(pool[i],pool[j]);
			else break;
		}

		sortParticles(start, j);
		sortParticles(j + 1, end);
	}
}

void Group::propagateUpdateTransform()
{
	for( Emitter** emitterIt = emitters.begin(); emitterIt < emitters.end(); ++emitterIt )
		(*emitterIt)->updateTransform(this);
	for( Modifier** modifierIt = modifiers.begin(); modifierIt < modifiers.end(); ++modifierIt )
		if( (*modifierIt)->isLocalToSystem() )
			(*modifierIt)->updateTransform(this);
}

Model& Group::getDefaultModel()
{
	static Model defaultModel;
	return defaultModel;
}