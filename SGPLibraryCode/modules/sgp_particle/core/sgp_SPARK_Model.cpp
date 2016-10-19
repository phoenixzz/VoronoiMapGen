
const float Model::DEFAULT_VALUES[MAX_PARAMS] =
{
	1.0f,	// RED
	1.0f,	// GREEN
	1.0f,	// BLUE
	1.0f,	// ALPHA
	1.0f,	// SIZE
	1.0f,	// MASS
	0.0f,	// ANGLE
	0.0f,	// TEXTURE_INDEX
	0.0f,	// ROTATION_SPEED
	0.0f,	// CUSTOM_0
	0.0f,	// CUSTOM_1
	0.0f,	// CUSTOM_2
};

Model::Model(uint32 enableFlag, uint32 mutableFlag, uint32 randomFlag, uint32 interpolatedFlag) :
	Registerable(),
	lifeTimeMin(1.0f),
	lifeTimeMax(1.0f),
	immortal(false),
	paramsSize(0),
	NumOfEnableParams(0),
	NumOfMutableParams(0),
	NumOfRandomParams(0),
	NumOfInterpolatedParams(0)
{
	enableFlag |= FLAG_RED | FLAG_GREEN | FLAG_BLUE; // Adds the color parameters to the enable flag
	this->enableFlag = enableFlag & ((1 << (MAX_PARAMS + 1)) - 1); // masks the enable flag with the existing parameters
	this->interpolatedFlag = interpolatedFlag & this->enableFlag; // masks the interpolated flag with the enable flag
	this->mutableFlag = mutableFlag & this->enableFlag; // masks the mutable flag with the enable flag
	this->mutableFlag &= ~this->interpolatedFlag; // a param cannot be both interpolated and mutable
	this->randomFlag = randomFlag & this->enableFlag; // masks the random flag with the enable flag
	this->randomFlag &= ~this->interpolatedFlag; // a param cannot be both interpolated and random

	uint32 particleEnableParamsSize = 0;
	uint32 particleMutableParamsSize = 0;
	for(uint32 i = 0; i < MAX_PARAMS; ++i)
	{
		ModelParam param = static_cast<ModelParam>(i);

		uint32 paramSize = 0;
		if( isEnabled(param) )
		{
			++NumOfEnableParams;
			if( !isInterpolated(param) )
			{	
				interpolators[i] = NULL;
				paramSize = 1;
				if( isMutable(param) )
				{
					paramSize = 2;
					++NumOfMutableParams;
				}
				if( isRandom(param) )
				{
					paramSize <<= 1;
					++NumOfRandomParams;
				}
			}
			else
			{
				interpolators[i] = new Interpolator(); // Creates the interpolator
				++NumOfInterpolatedParams;
			}
		}
		else
			interpolators[i] = NULL;

		particleEnableIndices[i] = particleEnableParamsSize;
		particleMutableIndices[i] = particleMutableParamsSize;
		particleEnableParamsSize += isEnabled(param) >> i;
		particleMutableParamsSize += isMutable(param) >> i;

		indices[i] = paramsSize;
		paramsSize += paramSize;
	}

	// creates the array of params for this model
	if( paramsSize > 0 )
	{
		params = new float[paramsSize];
		uint32 currentParamIndex = 0;
		uint32 currentIndex = 0;
		while(currentIndex < paramsSize)
		{
			uint32 nbValues = getNumberOfValues(static_cast<ModelParam>(currentParamIndex));
			for(uint32 i = 0; i < nbValues; ++i)
				params[currentIndex + i] = DEFAULT_VALUES[currentParamIndex];
			++currentParamIndex;
			currentIndex += nbValues;
		}
	}
	else
		params = NULL;

	if( NumOfEnableParams > 0 )
	{
		enableParams = new uint32[NumOfEnableParams];
		uint32 index = 0;
		for(uint32 i = 0; i < MAX_PARAMS; ++i)
			if( isEnabled(static_cast<ModelParam>(i)) )
				enableParams[index++] = i;
	}
	else
		enableParams = NULL;

	if(NumOfMutableParams > 0)
	{
		mutableParams = new uint32[NumOfMutableParams];
		uint32 index = 0;
		for(uint32 i = 0; i < MAX_PARAMS; ++i)
			if( isMutable(static_cast<ModelParam>(i)) )
				mutableParams[index++] = i;
	}
	else
		mutableParams = NULL;

	if(NumOfInterpolatedParams > 0)
	{
		interpolatedParams = new uint32[NumOfInterpolatedParams];
		uint32 index = 0;
		for(uint32 i = 0; i < MAX_PARAMS; ++i)
			if(isInterpolated(static_cast<ModelParam>(i)))
				interpolatedParams[index++] = i;
	}
	else
		interpolatedParams = NULL;
}

Model::Model(const Model& model) :
	Registerable(model),
	lifeTimeMin(model.lifeTimeMin),
	lifeTimeMax(model.lifeTimeMax),
	immortal(model.immortal),
	paramsSize(model.paramsSize),
	NumOfEnableParams(model.NumOfEnableParams),
	NumOfMutableParams(model.NumOfMutableParams),
	NumOfRandomParams(model.NumOfRandomParams),
	NumOfInterpolatedParams(model.NumOfInterpolatedParams),
	enableFlag(model.enableFlag),
	mutableFlag(model.mutableFlag),
	randomFlag(model.randomFlag),
	interpolatedFlag(model.interpolatedFlag),
	params(NULL),
	enableParams(NULL),
	mutableParams(NULL),
	interpolatedParams(NULL)
{
	if(paramsSize > 0)
	{
		params = new float[paramsSize];
		for (uint32 i = 0; i < paramsSize; ++i)
			params[i] = model.params[i];
	}

	if(NumOfEnableParams > 0)
	{
		enableParams = new uint32[NumOfEnableParams];
		for(uint32 i = 0; i < NumOfEnableParams; ++i)
			enableParams[i] = model.enableParams[i];
	}

	if(NumOfMutableParams > 0)
	{
		mutableParams = new uint32[NumOfMutableParams];
		for(uint32 i = 0; i < NumOfMutableParams; ++i)
			mutableParams[i] = model.mutableParams[i];
	}

	if(NumOfInterpolatedParams > 0)
	{
		interpolatedParams = new uint32[NumOfInterpolatedParams];
		for(uint32 i = 0; i < NumOfInterpolatedParams; ++i)
			interpolatedParams[i] = model.interpolatedParams[i];
	}

	for(uint32 i = 0; i < MAX_PARAMS; ++i)
	{
		indices[i] = model.indices[i];
		particleEnableIndices[i] = model.particleEnableIndices[i];
		particleMutableIndices[i] = model.particleMutableIndices[i];
		if(model.interpolators[i] != NULL)
			interpolators[i] = new Interpolator(*model.interpolators[i]);
		else
			interpolators[i] = NULL;
	}
}

Model::~Model()
{
	delete[] enableParams;			enableParams = NULL;
	delete[] mutableParams;			mutableParams = NULL;
	delete[] interpolatedParams;	interpolatedParams = NULL;
	delete[] params;				params = NULL;

	for(uint32 i = 0; i < MAX_PARAMS; ++i)
	{
		delete interpolators[i];
		interpolators[i] = NULL;
	}
}

bool Model::setParam(ModelParam type, float startMin, float startMax, float endMin, float endMax)
{
	// if the given param doesnt have 4 values, return
	if( getNumberOfValues(type) != 4 )
		return false;

	// Sets the values at the right position in params
	float* ptr = params + indices[type];
	*ptr++ = startMin;
	*ptr++ = startMax;
	*ptr++ = endMin;
	*ptr = endMax;

	return true;
}

bool Model::setParam(ModelParam type, float value0, float value1)
{
	// if the given param doesnt have 2 values, return
	if(getNumberOfValues(type) != 2)
		return false;

	// Sets the values at the right position in params
	float* ptr = params + indices[type];
	*ptr++ = value0;
	*ptr = value1;

	return true;
}

bool Model::setParam(ModelParam type, float value)
{
	// if the given param doesnt have 1 value, return
	if(getNumberOfValues(type) != 1)
		return false;

	// Sets the value at the right position in params
	params[indices[type]] = value;

	return true;
}

float Model::getParamValue(ModelParam type, uint32 index) const
{
	uint32 nbValues = getNumberOfValues(type);

	if( index < nbValues )
		return params[indices[type] + index];

	return DEFAULT_VALUES[type];
}

uint8 Model::getNumberOfValues(ModelParam type) const
{
	uint32 value = 1 << type;
	if( !(enableFlag & value) || (interpolatedFlag & value) )
		return 0;

	if( !(mutableFlag & value) && !(randomFlag & value) )
		return 1;

	if( (mutableFlag & value) && (randomFlag & value) )
		return 4;

	return 2;
}

float Model::getDefaultValue(ModelParam param)
{
	return DEFAULT_VALUES[param];
}