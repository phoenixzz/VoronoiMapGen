#ifndef __SGP_SPARKQUADRENDERINTERFACE_HEADER__
#define __SGP_SPARKQUADRENDERINTERFACE_HEADER__

/**
* Constants defining the way to apply texture over the particles
*/
enum TexturingMode
{
	TEXTURE_NONE,		/**< Constant telling no texturing is used */
	TEXTURE_2D,			/**< Constant telling a 2D texture is used */
	TEXTURE_3D,			/**< Constant telling a 3D texture is used */
};



class SGP_API QuadRendererInterface
{
public:
	/**
	* Constructor of QuadRendererInterface
	*	scaleX the scale of the width of the quad
	*	scaleY the scale of the height of the quad
	*/
	QuadRendererInterface(float _scaleX = 1.0f, float _scaleY = 1.0f) :
		scaleX(_scaleX),
		scaleY(_scaleY),
		texturingMode(TEXTURE_NONE),
		textureAtlasNbX(1),
		textureAtlasNbY(1),
		textureAtlasW(1.0f),
		textureAtlasH(1.0f)
	{}

	virtual ~QuadRendererInterface() {}



	/**
	* Sets the texturing mode for this QuadRendererInterface
	*
	* The texturing mode defines whether or not to apply a texture
	* and if so which type of texture to apply (2D,3D or atlas).

	* Note that the validity of the texturing mode depends on the rendering API below.
	* The method returns true if the rendering mode can be set, false if it cannot
	
	* return true if the rendering mode can be set, false if it cannot
	*/
	virtual bool setTexturingMode(TexturingMode mode);

	/**
	* Sets the cut of the texture
	*
	* This is available only if PARAM_TEXTURE_INDEX is enabled and texturing mode is TEXTURE_2D.

	* Particles can be rendered only with a part of the texture depending on their texture index value.
	* The cut can only be constant.
	* The user defines in how many parts the texture must be divided in X and Y.
	* The first index is located at the top left cut, the it goes from left to right and from top to bottom.
	
	* For instance a cut with nbX = 3 and nbY = 2 will be as followed :

		 -------------
		 | 0 | 1 | 2 |
		 -------------
		 | 3 | 4 | 5 |
		 -------------

	* By default nbX and nbY are equal to 1.
	*/
	void setAtlasDimensions(int nbX, int nbY);

	/**
	* Sets the size ratio of this QuadRenderer
	*
	* These values defines how quads are scaled.
	* The height and width of a quad in the universe is defined as followed :
			width = size * scaleX
			height = size * scaleY
	*/
	void setScale(float _scaleX, float _scaleY);



	TexturingMode getTexturingMode() const;


	int getAtlasDimensionX() const;
	int getAtlasDimensionY() const;


	float getScaleX() const;
	float getScaleY() const;

protected:
	TexturingMode texturingMode;

	float scaleX;
	float scaleY;

	// texture atlas info
	int textureAtlasNbX;
	int textureAtlasNbY;
	float textureAtlasW;
	float textureAtlasH;

	void computeAtlasCoordinates(const Particle& particle) const;

	float textureAtlasU0() const;
	float textureAtlasU1() const;
	float textureAtlasV0() const;
	float textureAtlasV1() const;

private:
	// this is where textureAtlas are stored after computation
	mutable float atlasU0;
	mutable float atlasU1;
	mutable float atlasV0;
	mutable float atlasV1;
};


inline bool QuadRendererInterface::setTexturingMode(TexturingMode mode)
{
	texturingMode = mode;
	return true;
}

inline void QuadRendererInterface::setScale(float _scaleX, float _scaleY)
{
	this->scaleX = _scaleX;
	this->scaleY = _scaleY;
}

inline TexturingMode QuadRendererInterface::getTexturingMode() const
{
	return texturingMode;
}

inline int QuadRendererInterface::getAtlasDimensionX() const
{
	return textureAtlasNbX;
}

inline int QuadRendererInterface::getAtlasDimensionY() const
{
	return textureAtlasNbY;
}

inline float QuadRendererInterface::getScaleX() const
{
	return scaleX;
}

inline float QuadRendererInterface::getScaleY() const
{
	return scaleY;
}

inline float QuadRendererInterface::textureAtlasU0() const
{
	return atlasU0;
}

inline float QuadRendererInterface::textureAtlasU1() const
{
	return atlasU1;
}

inline float QuadRendererInterface::textureAtlasV0() const
{
	return atlasV0;
}

inline float QuadRendererInterface::textureAtlasV1() const
{
	return atlasV1;
}

inline void QuadRendererInterface::computeAtlasCoordinates(const Particle& particle) const
{
	int textureIndex = static_cast<int>(particle.getParamCurrentValue(PARAM_TEXTURE_INDEX));
	atlasU0 = atlasU1 = static_cast<float>(textureIndex % textureAtlasNbX) / textureAtlasNbX;
	atlasV0 = atlasV1 = static_cast<float>(textureIndex / textureAtlasNbX) / textureAtlasNbY;
	atlasU1 += textureAtlasW;
	atlasV1 += textureAtlasH;
}

inline void QuadRendererInterface::setAtlasDimensions(int nbX, int nbY)
{
	textureAtlasNbX = nbX;
	textureAtlasNbY = nbY;
	textureAtlasW = 1.0f / nbX;
	textureAtlasH = 1.0f / nbY;
}


#endif		// __SGP_SPARKQUADRENDERINTERFACE_HEADER__