

void RandomEmitter::generateVelocity(Particle& particle, float speed) const
{
	float norm;
	do 
	{
		particle.velocity().Set(random(-1.0f,1.0f),random(-1.0f,1.0f),random(-1.0f,1.0f));
		norm = particle.velocity().GetLength();
	}
	while((norm > 1.0f) || (norm == 0.0f));
	particle.velocity() *= speed / norm;
}