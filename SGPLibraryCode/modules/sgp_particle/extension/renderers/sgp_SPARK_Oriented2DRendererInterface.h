#ifndef __SGP_SPARKORIENTED2DRENDERINTERFACE_HEADER__
#define __SGP_SPARKORIENTED2DRENDERINTERFACE_HEADER__


// Defines the orientation of a particle oriented in 2D
enum Orientation2D
{
	ORIENTATION2D_UP,			/**< Oriented towards the camera plane */
	ORIENTATION2D_DIRECTION,	/**< Oriented towards the direction of the particle */
	ORIENTATION2D_POINT,		/**< Oriented towards a point in the universe */
	ORIENTATION2D_AXIS			/**< The orientation vector is defined by an axis */		
};



class Oriented2DRendererInterface
{
public:
	/**
	* The orientation vector
	*
	* It is used in 2 orientation modes :

		ORIENTATION2D_AXIS : The orientation vector is used as the axis
		ORIENTATION2D_POINT : The orientation vector is the point particle look to

	* In other modes the orientation vector is not used
	*/
	Vector3D orientationVector;


	Oriented2DRendererInterface() : orientation(ORIENTATION2D_UP)
	{
		orientationVector.Set(0.0f, -1.0f, 0.0f);
	}

	virtual ~Oriented2DRendererInterface() {}


	//Sets the way quads are oriented in the universe
	void setOrientation(Orientation2D _orientation);
	Orientation2D getOrientation() const;

protected:
	Orientation2D orientation;

	bool hasGlobalOrientation();
	void computeGlobalOrientation2D();
	void computeSingleOrientation2D(const Particle& particle);

	void scaleQuadVectors(const Particle& particle, float scaleX, float scaleY) const;
	void rotateAndScaleQuadVectors(const Particle& particle, float scaleX, float scaleY) const;

	const Vector3D& quadUp() const;
	const Vector3D& quadSide() const;

private:
	// Used to store the orientation of quads before scaling
	mutable Vector3D up;
	mutable Vector3D side;

	// This is where are stored quad orientation info after computation
	mutable Vector3D sideQuad;
	mutable Vector3D upQuad;
};



		
inline void Oriented2DRendererInterface::setOrientation(Orientation2D _orientation)
{
	this->orientation = _orientation;
}
	
inline Orientation2D Oriented2DRendererInterface::getOrientation() const
{
	return orientation;
}

inline const Vector3D& Oriented2DRendererInterface::quadUp() const
{
	return upQuad;
}

inline const Vector3D& Oriented2DRendererInterface::quadSide() const
{
	return sideQuad;
}

inline bool Oriented2DRendererInterface::hasGlobalOrientation()
{
	return (orientation == ORIENTATION2D_UP) || (orientation == ORIENTATION2D_AXIS);
}
	
inline void Oriented2DRendererInterface::computeGlobalOrientation2D()
{
	if(orientation == ORIENTATION2D_UP)
		up.Set(0.0f, -0.5f, 0.0f);
	else if(orientation == ORIENTATION2D_AXIS)
	{
		up.Set(orientationVector.x, orientationVector.y, 0.0f);
		up.Normalize();
		up *= 0.5f;
	}
}
	
inline void Oriented2DRendererInterface::computeSingleOrientation2D(const Particle& particle)
{
	if(orientation == ORIENTATION2D_DIRECTION)
		up = particle.velocity();
	else if(orientation == ORIENTATION2D_POINT)
	{
		up = orientationVector;
		up -= particle.position();
	}
		
	up.z = 0.0f;
	up.Normalize();
	up *= 0.5f;
}

inline void Oriented2DRendererInterface::scaleQuadVectors(const Particle& particle, float scaleX, float scaleY) const
{
	float size = particle.getParamCurrentValue(PARAM_SIZE);

	upQuad.Set(up.x, up.y, 0.0f);
	upQuad *= size * scaleY;
		
	sideQuad.Set(-up.y, up.x, 0.0f);
	sideQuad *= size * scaleX;
}

inline void Oriented2DRendererInterface::rotateAndScaleQuadVectors(const Particle& particle, float scaleX, float scaleY) const
{
	float size = particle.getParamCurrentValue(PARAM_SIZE);

	float angleTexture = particle.getParamCurrentValue(PARAM_ANGLE);
	float cosA = std::cos(angleTexture);
	float sinA = std::sin(angleTexture);

	upQuad.x = cosA * up.x + sinA * up.y;
	upQuad.y = -sinA * up.x + cosA * up.y;
	upQuad.z = 0.0f;

	sideQuad.Set(-upQuad.y, upQuad.x, 0.0f);
		
	sideQuad *= size * scaleX;
	upQuad *= size * scaleY;
}

#endif		// __SGP_SPARKORIENTED2DRENDERINTERFACE_HEADER__