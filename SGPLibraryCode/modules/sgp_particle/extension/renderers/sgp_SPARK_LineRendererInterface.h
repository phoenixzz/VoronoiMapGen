#ifndef __SGP_SPARKLINERENDERINTERFACE_HEADER__
#define __SGP_SPARKLINERENDERINTERFACE_HEADER__

class LineRendererInterface
{
public:
	/**
	* Constructor of LineRendererInterface
	*	 length : the length multiplier of this LineRendererInterface
	*	 width : the width of this LineRenderer
	*/
	LineRendererInterface(float _length = 1.0f, float _width = 1.0f);

	virtual ~LineRendererInterface() {}



	/**
	*  Sets the length multiplier of this LineRendererInterface
	*
	* The length multiplier is the value which will be multiplied by the Particle velocity to get the line length in the universe.
	* A positive length means the line will be drawn in advance to the Particle, as opposed to a negative length.
	*/
	void setLength(float _length);

	/**
	* Sets the width of this LineRendererInterface
	*/
	virtual void setWidth(float _width);




	float getLength() const;
	float getWidth() const;

protected:
	float length;
	float width;
};


inline LineRendererInterface::LineRendererInterface(float _length, float _width) :
	length(_length),
	width(_width)
{}

inline void LineRendererInterface::setLength(float _length)
{
	this->length = _length;
}

inline void LineRendererInterface::setWidth(float _width)
{
	this->width = _width;
}

inline float LineRendererInterface::getLength() const
{
	return length;
}

inline float LineRendererInterface::getWidth() const
{
	return width;
}

#endif		// __SGP_SPARKLINERENDERINTERFACE_HEADER__