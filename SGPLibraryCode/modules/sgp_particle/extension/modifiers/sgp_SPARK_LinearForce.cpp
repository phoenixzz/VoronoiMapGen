

LinearForce::LinearForce(Zone* zone, ModifierTrigger trigger, const Vector3D& force, ForceFactor type, ModelParam param) :
	Modifier(ALWAYS | INSIDE_ZONE | OUTSIDE_ZONE,ALWAYS, false, false, zone),
	force(force),
	tForce(force),
	factorType(type),
	factorParam(param)
{
	setTrigger(trigger);
}

void LinearForce::modify(Particle& particle, float deltaTime) const
{
	float factor = deltaTime / particle.getParamCurrentValue(PARAM_MASS);
		
	if(factorType != FACTOR_NONE)
	{
		float param = particle.getParamCurrentValue(factorParam);
		factor *= param; // linearity function of the parameter
		if(factorType == FACTOR_SQUARE)
			factor *= param; // linearity function of the square of the parameter
	}

	particle.velocity() += tForce * factor;
}