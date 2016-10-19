

Obstacle::Obstacle(Zone* zone, ModifierTrigger trigger, float bouncingRatio, float friction) :
	Modifier(INTERSECT_ZONE | ENTER_ZONE | EXIT_ZONE, INTERSECT_ZONE, true, true, zone),
	bouncingRatio(bouncingRatio),
	friction(friction)
{
	setTrigger(trigger);
}

void Obstacle::modify(Particle& particle, float deltaTime) const
{
	Vector3D& velocity = particle.velocity();
	velocity = particle.position();
	velocity -= particle.oldPosition();

	if(deltaTime != 0.0f)
		velocity *= 1.0f / deltaTime;
	else 
		velocity.Set(0.0f,0.0f,0.0f);

	float dist = velocity * normal;

	normal *= dist;
	velocity -= normal;			// tangent component
	velocity *= friction;
	normal *= bouncingRatio;	// normal component
	velocity -= normal;

	particle.position() = intersection;
}