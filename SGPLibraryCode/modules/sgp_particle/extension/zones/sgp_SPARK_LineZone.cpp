

LineZone::LineZone(const Vector3D& p0, const Vector3D& p1) :
	Zone(Vector3D())
{
	setBounds(p0, p1);
}

void LineZone::setPosition(const Vector3D& v)
{
	Vector3D displacement = v - getPosition();
	tBounds[0] += displacement;
	bounds[0] = tBounds[0];
	tBounds[1] += displacement;
	bounds[1] = tBounds[1];
	computeDist();
	Zone::setPosition(v);
}

void LineZone::setBounds(const Vector3D& p0, const Vector3D& p1)
{
	bounds[0] = tBounds[0] = p0;
	bounds[1] = tBounds[1] = p1;
	computeDist();
	computePosition();
}

void LineZone::pushBound(const Vector3D& bound)
{
	bounds[0] = tBounds[0] = bounds[1];
	bounds[1] = tBounds[1] = bound;
	computeDist();
	computePosition();
}

void LineZone::generatePosition(Particle& particle, bool ) const
{
	float ratio = random(0.0f, 1.0f);
	particle.position() = tBounds[0] + tDist * ratio;
}

Vector3D LineZone::computeNormal(const Vector3D& point) const
{
	float d = -(tDist * point);
	float sqrNorm = tDist.GetLengthSquared();
	float t = 0.0f;
	if(sqrNorm > 0.0f)
	{
		t = -((tDist * tBounds[0]) + d) / sqrNorm;
		// t is clamped to the segment
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;
	}

	Vector3D normal = point;
	normal -= tBounds[0] + tDist * t;

	normalizeOrRandomize(normal);
	return normal;
}

void LineZone::innerUpdateTransform()
{
	Zone::innerUpdateTransform();
	tBounds[0] = bounds[0] * getWorldTransform();
	tBounds[1] = bounds[1] * getWorldTransform();
	computeDist();
}