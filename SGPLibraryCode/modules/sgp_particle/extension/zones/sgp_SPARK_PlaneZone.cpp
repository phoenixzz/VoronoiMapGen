

PlaneZone::PlaneZone(const Vector3D& position, const Vector3D& normal) :
	Zone(position)
{
	setNormal(normal);
}

bool PlaneZone::intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* pnormal) const
{
	float dist0 = tNormal * (v0 - getTransformedPosition());
	float dist1 = tNormal * (v1 - getTransformedPosition());

	if( (dist0 <= 0.0f) == (dist1 <= 0.0f) ) // both points are on the same side
		return false;

	if( intersection != NULL )
	{
		if(dist0 <= 0.0f)
			dist0 = -dist0;
		else
			dist1 = -dist1;

		if(pnormal != NULL)
			*pnormal = tNormal;

		float ti = dist0 / (dist0 + dist1);

		Vector3D vDir = v1 - v0;
		float norm = vDir.GetLength();

		norm *= ti;
		ti = norm < Zone::APPROXIMATION_VALUE ? 0.0f : ti * (norm - Zone::APPROXIMATION_VALUE) / norm;

		vDir *= ti;
		*intersection = v0 + vDir;
	}

	return true;
}

void PlaneZone::moveAtBorder(Vector3D& v, bool inside) const
{
	float dist = tNormal * (v - getTransformedPosition());

	if( (dist <= 0.0f) == inside )
		inside ? dist += Zone::APPROXIMATION_VALUE : dist -= Zone::APPROXIMATION_VALUE;
	else
		inside ? dist -= Zone::APPROXIMATION_VALUE : dist += Zone::APPROXIMATION_VALUE;

	v += tNormal * -dist;
}

void PlaneZone::innerUpdateTransform()
{
	Zone::innerUpdateTransform();
	tNormal = normal;
	tNormal.RotateWith(getWorldTransform());
	tNormal.Normalize();
}