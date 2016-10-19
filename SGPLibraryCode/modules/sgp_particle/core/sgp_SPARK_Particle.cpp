
Particle::Particle(Group* group, uint32 index) :
	group(group),
	index(index),
	data(group->particleData + index),
	currentParams(group->particleCurrentParams + index * group->model->getSizeOfParticleCurrentArray()),
	extendedParams(group->particleExtendedParams + index * group->model->getSizeOfParticleExtendedArray())
{
	init();
}

void Particle::init()
{
	const Model* model = group->getModel();
	data->age = 0.0f;
	data->life = random(model->lifeTimeMin, model->lifeTimeMax);

	// creates pseudo-iterators to parse arrays
	float* particleCurrentIt = currentParams;
	float* particleMutableIt = extendedParams;
	float* particleInterpolatedIt = extendedParams + model->NumOfMutableParams;
	const uint32* paramIt = model->enableParams;

	// initializes params
	for(uint32 i = model->NumOfEnableParams; i != 0; --i)
	{
		ModelParam param = static_cast<ModelParam>(*paramIt);
		const float* templateIt = &model->params[model->indices[param]];

		if( model->isInterpolated(param) )
		{
			*particleCurrentIt++ = Model::DEFAULT_VALUES[param];
			*particleInterpolatedIt++ = random(0.0f, 1.0f); // ratioY

			Interpolator* interpolator = model->interpolators[param];
			float offsetVariation = interpolator->getOffsetXVariation();
			float scaleVariation = interpolator->getScaleXVariation();

			*particleInterpolatedIt++ = random(-offsetVariation, offsetVariation); // offsetX
			*particleInterpolatedIt++ = 1.0f + random(-scaleVariation, scaleVariation); // scaleX
		}
		else if( model->isRandom(param) )
		{
			*particleCurrentIt++ = random(*templateIt, *(templateIt + 1));
			if( model->isMutable(param) )
				*particleMutableIt++ = random(*(templateIt + 2), *(templateIt + 3));
		}
		else 
		{
			*particleCurrentIt++ = *templateIt;
			if( model->isMutable(param) )
				*particleMutableIt++ = *(templateIt + 1);
		}

		++paramIt;
	}
}

void Particle::interpolateParameters()
{
	const Model* model = group->getModel();

	float* interpolatedIt = extendedParams + model->NumOfMutableParams;
	for(uint32 i = 0; i < model->NumOfInterpolatedParams; ++i)
	{
		uint32 indexi = model->interpolatedParams[i];
		uint32 enableIndex = model->particleEnableIndices[indexi];
		currentParams[enableIndex] = model->interpolators[indexi]->interpolate(*this, static_cast<ModelParam>(indexi), interpolatedIt[0], interpolatedIt[1], interpolatedIt[2]);
		interpolatedIt += 3;
	}
}

bool Particle::update(float deltaTime)
{
	const Model* model = group->getModel();
	data->age += deltaTime;

	if( !model->immortal )
	{
		// computes the ratio between the life of the particle and its lifetime
		float ratio = sgp::jmin(1.0f, deltaTime / data->life);
		data->life -= deltaTime;
			
		// updates mutable parameters
		for(uint32 i = 0; i < model->NumOfMutableParams; ++i)
		{
			uint32 indexi = model->mutableParams[i];
			uint32 enableIndex = model->particleEnableIndices[indexi];
			currentParams[enableIndex] += (extendedParams[i] - currentParams[enableIndex]) * ratio;
		}
	}

	// updates interpolated parameters
	interpolateParameters();

	// updates position
	oldPosition() = position();
	position() += velocity() * deltaTime;

	// updates velocity
	velocity() += group->getGravity() * deltaTime;

	for( Modifier** it = group->activeModifiers.begin(); it < group->activeModifiers.end(); ++it )
		(*it)->process(*this, deltaTime);

	if( group->getFriction() != 0.0f )
		velocity() *= 1.0f - sgp::jmin( 1.0f, group->getFriction() * deltaTime / getParamCurrentValue(PARAM_MASS) );

	return data->life <= 0.0f;
}

bool Particle::setParamCurrentValue(ModelParam type, float value)
{
	const Model* const model = group->getModel();
	if( model->isEnabled(type) )
	{
		currentParams[model->particleEnableIndices[type]] = value;
		return true;
	}

	return false;
}

bool Particle::setParamFinalValue(ModelParam type, float value)
{
	const Model* const model = group->getModel();
	if( model->isMutable(type) )
	{
		extendedParams[model->particleMutableIndices[type]] = value;
		return true;
	}

	return false;
}

bool Particle::changeParamCurrentValue(ModelParam type, float delta)
{
	const Model* const model = group->getModel();
	if( model->isEnabled(type) )
	{
		currentParams[model->particleEnableIndices[type]] += delta;
		return true;
	}

	return false;
}

bool Particle::changeParamFinalValue(ModelParam type, float delta)
{
	const Model* const model = group->getModel();
	if( model->isMutable(type) )
	{
		extendedParams[model->particleMutableIndices[type]] += delta;
		return true;
	}

	return false;
}

float Particle::getParamCurrentValue(ModelParam type) const
{
	const Model* const model = group->getModel();
	if( model->isEnabled(type) )
		return currentParams[model->particleEnableIndices[type]];

	return Model::DEFAULT_VALUES[type];
}

float Particle::getParamFinalValue(ModelParam type) const
{
	const Model* const model = group->getModel();
	if( model->isEnabled(type) )
	{
		if( model->isMutable(type) )
			return extendedParams[model->particleMutableIndices[type] + 1];
		return currentParams[model->particleEnableIndices[type]];
	}

	return Model::DEFAULT_VALUES[type];
}

Model* Particle::getModel() const
{
	return group->getModel();
}

void Particle::computeSqrDist()
{
	data->sqrDist = (position() - System::getCameraPosition()).GetLengthSquared();
}

extern void swapParticles(Particle& a, Particle& b)
{
	sgp::swapVariables((*a.data), (*b.data));
	for(uint32 i = 0; i < a.getModel()->getSizeOfParticleCurrentArray(); ++i)
		sgp::swapVariables(a.currentParams[i], b.currentParams[i]);
	for(uint32 i = 0; i < a.getModel()->getSizeOfParticleExtendedArray(); ++i)
		sgp::swapVariables(a.extendedParams[i], b.extendedParams[i]);
		
	// swap additional data (groups are assumed to be the same)
	for( Buffer** it = a.group->swappableBuffers.begin(); it < a.group->swappableBuffers.end(); ++it )
		(*it)->swap(a.index, b.index);
}