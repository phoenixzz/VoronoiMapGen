#ifndef __SGP_SPARKLINEZONE_HEADER__
#define __SGP_SPARKLINEZONE_HEADER__

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

/**
* A Zone defining a line in the universe
*
* As any Zone, a Line is defined by a position. The Line in itself is defined by two bounds.
* Moving the position results in moving the 2 bounds in the universe by the same vector.

* To conform with the Zone class (the position is defined as the center of the Zone), the position is always set to
* be the center of the line. Therefore, if a bound is modified, the position will be modified as well.
*/
class SGP_API LineZone : public Zone
{
	SPARK_IMPLEMENT_REGISTERABLE(LineZone)

public:
	LineZone(const Vector3D& p0 = Vector3D(0.0f,0.0f,0.0f), const Vector3D& p1 = Vector3D(0.0f,0.0f,0.0f));

	static LineZone* create(const Vector3D& p0 = Vector3D(0.0f,0.0f,0.0f), const Vector3D& p1 = Vector3D(0.0f,0.0f,0.0f));


	void setPosition(const Vector3D& v);

	void setBounds(const Vector3D& p0, const Vector3D& p1);



	/**
	* Gets the bound of index of this Line
	* index : the index of the bound (0 or 1)
	*/
	const Vector3D& getBound(int index) const;

	/**
	* Gets the transformed bound of index of this Line
	*  index : the index of the bound (0 or 1)
	* return the transformed bound of index of this Line
	*/
	const Vector3D& getTransformedBound(int index) const;



	/**
	* Pushes a new bound to this Line
	* This method replaces the first bound by the second bound and the second bound by the new bound.
	* It allows to follow the trajectory of a moving object over time with a single Line.
	*/
	void pushBound(const Vector3D& bound);

	virtual void setZoneScale( float scale );
	virtual void generatePosition(Particle& particle, bool full) const;
	virtual bool contains(const Vector3D& v) const;
	virtual bool intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection,Vector3D* normal) const;
	virtual void moveAtBorder(Vector3D& v, bool inside) const;
	virtual Vector3D computeNormal(const Vector3D& point) const;

protected:
	virtual void innerUpdateTransform();

private:
	Vector3D bounds[2];
	Vector3D tBounds[2];

	Vector3D tDist;

	void computeDist();
	void computePosition();
};


inline LineZone* LineZone::create(const Vector3D& p0,const Vector3D& p1)
{
	LineZone* obj = new LineZone(p0,p1);
	registerObject(obj);
	return obj;
}
		
inline const Vector3D& LineZone::getBound(int index) const
{
	return bounds[index];
}

inline const Vector3D& LineZone::getTransformedBound(int index) const
{
	return tBounds[index];
}

inline bool LineZone::contains(const Vector3D& v) const
{
	return false;
}

inline bool LineZone::intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const
{
	return false;
}

inline void LineZone::moveAtBorder(Vector3D& v, bool inside) const {}

inline void LineZone::computeDist()
{
	tDist = tBounds[1] - tBounds[0];		
}

inline void LineZone::computePosition()
{
	Zone::setPosition((bounds[0] + bounds[1]) * 0.5f);
}

inline void LineZone::setZoneScale( float scale )
{
	Vector3D v = Zone::getPosition();
	this->setBounds( v + (bounds[0] - v) * scale, v + (bounds[1] - v) * scale );
}

#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKLINEZONE_HEADER__