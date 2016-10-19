
Interpolator::computeXFn Interpolator::COMPUTE_X_FN[4] =
{
	&Interpolator::computeXLifeTime,
	&Interpolator::computeXAge,
	&Interpolator::computeXParam,
	&Interpolator::computeXVelocity,
};

Interpolator::Interpolator() :
	type(INTERPOLATOR_LIFETIME),
	param(PARAM_SIZE),
	scaleXVariation(0.0f),
	offsetXVariation(0.0f),
	loopingEnabled(false)
{}

float Interpolator::computeXLifeTime(const Particle& particle) const
{
	return particle.getAge() / (particle.getAge() + particle.getLifeLeft());
}

float Interpolator::computeXAge(const Particle& particle) const
{
	return particle.getAge();
}

float Interpolator::computeXParam(const Particle& particle) const
{
	return particle.getParamCurrentValue(param);
}

float Interpolator::computeXVelocity(const Particle& particle) const
{
	return particle.velocity().GetLengthSquared();
}

float Interpolator::interpolate(const Particle& particle, ModelParam interpolatedParam, float ratioY, float offsetX, float scaleX)
{
	// First finds the current X of the particle
	InterpolatorEntry currentKey((this->*Interpolator::COMPUTE_X_FN[type])(particle));
	currentKey.x += offsetX; // Offsets it
	currentKey.x *= scaleX;  // Scales it

	if(loopingEnabled)
	{
		// If the graph has less than 2 entries, we cannot loop
		if(graph.size() < 2)
		{
			if(graph.size() == 0)
				return Model::getDefaultValue(interpolatedParam);
			else
				return interpolateY(*(graph.begin()), ratioY);
		}

		// Else finds the current X in the range
		const float beginX = graph.begin()->x;
		const float rangeX = graph.getLast().x - beginX;
		float newX = (currentKey.x - beginX) / rangeX;
		newX -= static_cast<int>(newX);
		if (newX < 0.0f)
			newX = 1.0f + newX;
		currentKey.x = beginX + newX * rangeX;
	}


	// If the graph has no entry, sets the default value
	if( graph.size() == 0 )	
		return Model::getDefaultValue(interpolatedParam);
	// If the current X is lower than the first entry, sets the value to the first entry
	if( currentKey <= graph.getFirst() )
		return interpolateY(graph.getFirst(), ratioY);
	// sets the value of the last entry
	if( currentKey >= graph.getLast() )
		return interpolateY(graph.getLast(), ratioY);

	// interpolated between the entries before and after the current X
	for( InterpolatorEntry* nextIt = graph.begin()+1; nextIt < graph.end(); ++nextIt )
	{
		if( (*(nextIt-1) <= currentKey) && (currentKey < *nextIt) )
		{
			const InterpolatorEntry& nextEntry = *nextIt;
			const InterpolatorEntry& previousEntry = *(--nextIt);
			float y0 = interpolateY(previousEntry, ratioY);
			float y1 = interpolateY(nextEntry, ratioY);

			float ratioX = (currentKey.x - previousEntry.x) / (nextEntry.x - previousEntry.x);
			return y0 + ratioX * (y1 - y0);
		}
	}
	return Model::getDefaultValue(interpolatedParam);
}

void Interpolator::generateSinCurve(float period, float amplitudeMin, float amplitudeMax, float offsetX, float offsetY, float startX, uint32 length, uint32 nbSamples)
{
	// First clear any previous entry
	clearGraph();

	for(uint32 i = 0; i < nbSamples; ++i)
	{
		float x = startX + i * period * length / (nbSamples - 1);
		float sin = std::sin((x + offsetX) * 2 * float_Pi / period);
		addEntry(x, amplitudeMin * sin + offsetY, amplitudeMax * sin + offsetY);
	}
}

void Interpolator::generatePolyCurve(float constant, float linear, float quadratic, float cubic, float startX, float endX, uint32 nbSamples)
{
	// First clear any previous entry
	clearGraph();

	for(uint32 i = 0; i < nbSamples; ++i)
	{
		float x = startX + i * (endX - startX) / (nbSamples - 1);
		float x2 = x * x;
		float x3 = x2 * x;
		addEntry(x, constant + x * linear + x2 * quadratic + x3 * cubic);
	}
}