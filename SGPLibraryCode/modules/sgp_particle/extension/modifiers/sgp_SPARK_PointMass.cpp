

PointMass::PointMass(Zone* zone, ModifierTrigger trigger, float mass, float minDistance) :
	Modifier(ALWAYS | INSIDE_ZONE | OUTSIDE_ZONE,ALWAYS, false, false, zone),
	position(),
	tPosition(),
	mass(mass)
{
	setTrigger(trigger);
	setMinDistance(minDistance);
}

void PointMass::modify(Particle& particle, float deltaTime) const
{
	Vector3D force = tPosition;
	if( getZone() != NULL )
		force += getZone()->getTransformedPosition();

	force -= particle.position();
	force *= mass * deltaTime / jmax(sqrMinDistance, force.GetLengthSquared());
	particle.velocity() += force;
}