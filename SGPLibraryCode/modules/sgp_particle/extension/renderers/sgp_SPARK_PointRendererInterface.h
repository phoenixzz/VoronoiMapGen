#ifndef __SGP_SPARKPOINTRENDERINTERFACE_HEADER__
#define __SGP_SPARKPOINTRENDERINTERFACE_HEADER__


// Constants defining the type of points to render

enum PointType
{
	POINT_SQUARE,	/**< Points are renderered as squares */
	//POINT_CIRCLE,	/**< Points are renderered as circles */
	POINT_SPRITE,	/**< Points are renderered as point sprites (textured points) */
};


class PointRendererInterface
{
public:

	/**
	* Constructor of PointRendererInterface
	*	type : the initial type of this PointRendererInterface (must be supported by default by the platform)
	*	size : the width of this PointRendererInterface
	*/
	PointRendererInterface(PointType _type = POINT_SPRITE, float _size = 1.0f) : type(_type), size(_size) {}

	virtual ~PointRendererInterface() {}



	/**
	* Sets the type of points to use in this PointRendererInterface
	* If the type is not supported by the platform, false is returned and the type per default is set.
	*/
	virtual bool setType(PointType _type)
	{
		this->type = _type;
		return true;
	}

	/**
	* Sets the size of the points in this PointRendererInterface
	*/
	virtual void setSize(float _size)
	{
		this->size = _size;
	}

	PointType getType() const { return type; }
	float getSize() const { return size; }

protected:
	PointType type;
	float size;
};


#endif		// __SGP_SPARKPOINTRENDERINTERFACE_HEADER__