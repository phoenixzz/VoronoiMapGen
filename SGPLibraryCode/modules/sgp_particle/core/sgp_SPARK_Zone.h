#ifndef __SGP_SPARKZONE_HEADER__
#define __SGP_SPARKZONE_HEADER__

class Particle;

/**
* An abstract class that defines a zone in space
* A Zone is used in SPARK to :		
	define the area of an Emitter
	define the area of a Modifier
*/
class SGP_API Zone : public Registerable, public Transformable
{
public:
	Zone(const Vector3D& position = Vector3D());
	virtual ~Zone() {}

	virtual void setPosition(const Vector3D& v);

	inline const Vector3D& getPosition() const { return position; }

	inline const Vector3D& getTransformedPosition() const { return PositionTransformed; }

	/**
	* Randomly generates a position inside this Zone for a given Particle
	* IN -> particle : the Particle whose position will be generated
	* IN -> full : true to generate a position in the whole volume of this Zone, false to generate a position only at borders
	*/
	virtual void generatePosition(Particle& particle, bool full) const = 0;

	// Checks whether a point is within the Zone
	virtual bool contains(const Vector3D& point) const = 0;

	// Set zone scale
	virtual void setZoneScale( float scale ) = 0;

	/**
	* Checks whether a line intersects the Zone
	*
	* The intersection is computed only if the Vector3D* intersection is not NULL.
	* The normal is computed if the Vector3D* normal AND intersection are not NULL.

	*	v0 : start of the line
	*	v1 : end of the line
	*	intersection : the Vector3D where the intersection will be stored, NULL not to compute the intersection
	*	normal : the Vector3D where the normal will be stored, NULL not to compute the normal
	* return true if the line intersects with the Zone, false otherwise
	*/
	virtual bool intersects(const Vector3D& v0, const Vector3D& v1, Vector3D* intersection, Vector3D* normal) const = 0;

	/**
	* Moves a point at the border of the Zone
	*	point : the point that will be moved to the border of the Zone
	*	inside : true to move the point inside the Zone of APPROXIMATION_VALUE, false to move it outside of APPROXIMATION_VALUE
	*/
	virtual void moveAtBorder(Vector3D& point, bool inside) const = 0;

	/**
	* Computes the normal for the point
	* IN param point : the point from where the normal is computed
	*/
	virtual Vector3D computeNormal(const Vector3D& point) const = 0;


protected:
	/** Value used for approximation */
	static const float APPROXIMATION_VALUE;

	/**
	* A helper static method to normalize a Vector3D
	*
	* If the Vector3D is NULL, a random normal Vector3D is set.
	* The randomness is guaranteed to be uniformely distributed.
	*/
	static void normalizeOrRandomize(Vector3D& v);

	virtual void innerUpdateTransform();

private:
	Vector3D position;
	Vector3D PositionTransformed; // transformed position
};

inline void Zone::setPosition(const Vector3D& v)
{
	position = PositionTransformed = v;
	notifyForUpdate();
}

inline void Zone::normalizeOrRandomize(Vector3D& v)
{
	while( v.GetLengthSquared() == 0 )
	{
		do v = Vector3D(random(-1.0f,1.0f),random(-1.0f,1.0f),random(-1.0f,1.0f));
		while (v.GetLengthSquared() > 1.0f);
	}
}

inline void Zone::innerUpdateTransform()
{
	PositionTransformed = position * getWorldTransform();
}

#endif		// __SGP_SPARKZONE_HEADER__