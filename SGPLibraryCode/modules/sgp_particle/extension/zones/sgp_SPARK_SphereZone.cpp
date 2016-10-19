

SphereZone::SphereZone(const Vector3D& position, float radius) : Zone(position)
{
	setRadius(radius);
}

void SphereZone::generatePosition(Particle& particle, bool full) const
{
	do 
		particle.position() = Vector3D(random(-radius,radius),random(-radius,radius),random(-radius,radius));
	while (particle.position().GetLengthSquared() > radius * radius);

	if( (!full) && (radius > 0.0f) )
		particle.position() *= radius / particle.position().GetLength();

	particle.position() += getTransformedPosition();
}

bool SphereZone::contains(const Vector3D& v) const
{
	return (getTransformedPosition()-v).GetLengthSquared() <= radius * radius;
}

bool SphereZone::intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const
{
	float r2 = radius * radius;
	float dist0 = (getTransformedPosition()-v0).GetLengthSquared();
	float dist1 = (getTransformedPosition()-v1).GetLengthSquared();

	if( (dist0 <= r2) == (dist1 <= r2) )
		return false;

	if(intersection != NULL)
	{
		Vector3D vDir = v1 - v0;
		float norm = vDir.GetLength();

		float d = (vDir * (getTransformedPosition() - v0)) / norm;
		float a = std::sqrt(r2 - dist0 + d * d);

		float ti;
		if(dist0 <= r2)
			ti = d - a;
		else
			ti = d + a;

		ti /= norm;

		if(ti < 0.0f) ti = 0.0f;
		if(ti > 1.0f) ti = 1.0f;

		norm *= ti;
		ti = norm < APPROXIMATION_VALUE ? 0.0f : ti * (norm - APPROXIMATION_VALUE) / norm;

		vDir *= ti;
		*intersection = v0 + vDir;

		if(normal != NULL)
		{
			if(dist0 <= r2)
				*normal = getTransformedPosition() - *intersection;
			else
				*normal = *intersection - getTransformedPosition();
			normal->Normalize();
		}
	}

	return true;
}

void SphereZone::moveAtBorder(Vector3D& v, bool inside) const
{
	Vector3D vDir = v - getTransformedPosition();
	float norm = vDir.GetLength();

	if(inside)
		vDir *= (radius + APPROXIMATION_VALUE) / norm;
	else
		vDir *= (radius - APPROXIMATION_VALUE) / norm;

	v = getTransformedPosition() + vDir;
}

Vector3D SphereZone::computeNormal(const Vector3D& point) const
{
	Vector3D normal(point - getTransformedPosition());
	normalizeOrRandomize(normal);
	return normal;
}