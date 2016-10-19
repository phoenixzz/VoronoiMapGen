

RingZone::RingZone(const Vector3D& position, const Vector3D& normal, float minRadius, float maxRadius) :
	Zone(position)
{
	setNormal(normal);
	setRadius(minRadius, maxRadius);
}

void RingZone::setRadius(float _minRadius, float _maxRadius)
{
	if(_minRadius < 0.0f) _minRadius = -_minRadius;
	if(_maxRadius < 0.0f) _maxRadius = -_maxRadius;
	if(_minRadius > _maxRadius) 
		sgp::swapVariables(_minRadius, _maxRadius);
	this->minRadius = _minRadius;
	this->maxRadius = _maxRadius;
	sqrMinRadius = _minRadius * _minRadius;
	sqrMaxRadius = _maxRadius * _maxRadius;
}

void RingZone::generatePosition(Particle& particle, bool ) const
{
	Vector3D tmp;
	do 
		tmp = Vector3D(random(-1.0f,1.0f),random(-1.0f,1.0f),random(-1.0f,1.0f));
	while (tmp.GetLengthSquared() > 1.0f);
		
	particle.position().Cross(tNormal, tmp);
	normalizeOrRandomize(particle.position());

	particle.position() *= std::sqrt(random(sqrMinRadius, sqrMaxRadius)); // to have a uniform distribution
	particle.position() += getTransformedPosition();
}

bool RingZone::intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* pnormal) const
{
	float dist0 = tNormal * (v0 - getTransformedPosition());
	float dist1 = tNormal * (v1 - getTransformedPosition());

	if( (dist0 <= 0.0f) == (dist1 <= 0.0f) ) // both points are on the same side
		return false;

	if(dist0 <= 0.0f)
		dist0 = -dist0;
	else
		dist1 = -dist1;

	float ti = dist0 / (dist0 + dist1);

	Vector3D vDir(v1 - v0);
	float norm = vDir.GetLength();

	norm *= ti;
	ti = norm < APPROXIMATION_VALUE ? 0.0f : ti * (norm - APPROXIMATION_VALUE) / norm;

	vDir *= ti;
	Vector3D inter(v0 + vDir);

	float distFromCenter = (inter - getTransformedPosition()).GetLengthSquared();
	if( distFromCenter > sqrMaxRadius || distFromCenter < sqrMinRadius ) // intersection is not in the ring
		return false;

	if(intersection != NULL)
	{
		*intersection = inter;
		if (pnormal != NULL)
			*pnormal = tNormal;
	}

	return true;
}
	
void RingZone::moveAtBorder(Vector3D& v, bool ) const
{
	float dist = tNormal * (v - getTransformedPosition());	
	v += tNormal * -dist;

	float distFromCenter = (v - getTransformedPosition()).GetLengthSquared();

	if(distFromCenter > sqrMaxRadius)
	{
		distFromCenter = std::sqrt(distFromCenter);
		Vector3D vDir(v - getTransformedPosition());
		vDir *= maxRadius / distFromCenter;
		v = getTransformedPosition() + vDir;
	}
	else if(distFromCenter < sqrMinRadius)
	{
		distFromCenter = std::sqrt(distFromCenter);
		Vector3D vDir(v - getTransformedPosition());
		vDir *= minRadius / distFromCenter;
		v = getTransformedPosition() + vDir;
	}
}

void RingZone::innerUpdateTransform()
{
	Zone::innerUpdateTransform();
	tNormal = normal;
	tNormal.RotateWith(getWorldTransform());
	tNormal.Normalize();
}