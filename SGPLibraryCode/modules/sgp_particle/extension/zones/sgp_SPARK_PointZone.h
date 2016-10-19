#ifndef __SGP_SPARKPOINTZONE_HEADER__
#define __SGP_SPARKPOINTZONE_HEADER__

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

/**
* A Zone defining a point in the universe
*/
class SGP_API PointZone : public Zone
{
	SPARK_IMPLEMENT_REGISTERABLE(PointZone)

public:

	PointZone(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f)) : Zone(position) {}

	static PointZone* create(const Vector3D& position = Vector3D(0.0f,0.0f,0.0f));

	// Interface
	virtual void setZoneScale( float scale );
	virtual void generatePosition(Particle& particle, bool full) const;
	virtual bool contains(const Vector3D& v) const;
	virtual bool intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const;
	virtual void moveAtBorder(Vector3D& v, bool inside) const;
	virtual Vector3D computeNormal(const Vector3D& point) const
	{
		Vector3D normal(point - getTransformedPosition());

		normalizeOrRandomize(normal);

		return normal;
	}
};

inline PointZone* PointZone::create(const Vector3D& position)
{
	PointZone* obj = new PointZone(position);
	registerObject(obj);
	return obj;
}
	
inline void PointZone::generatePosition(Particle& particle, bool full) const
{
	particle.position() = getTransformedPosition();
}

inline bool PointZone::contains(const Vector3D& v) const
{
	return false;
}

inline bool PointZone::intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const
{
	return false;
}

inline void PointZone::moveAtBorder(Vector3D& v, bool inside) const {}
inline void PointZone::setZoneScale( float scale ) {}

#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKPOINTZONE_HEADER__