#ifndef __SGP_OPENGLES2PARTICLEPOINTRENDERER_HEADER__
#define __SGP_OPENGLES2PARTICLEPOINTRENDERER_HEADER__

/**
* A OpenGL Renderer drawing particles as points with SGP Engine

* Rendering can be done only in :
	SPARK::POINT_SPRITE : point sprites (must be supported by the hardware)

* Regarding the size of the rendered point, they are dependant of the SGP settings.
* Basically size of the points is neither in pixels nor in the universe unit.
* Moreover, points are scaling with the distance but are rapidly clamped.

* Below are the parameters of Particle that are used in this Renderer (others have no effects) :

		SPARK::PARAM_RED
		SPARK::PARAM_GREEN
		SPARK::PARAM_BLUE
		SPARK::PARAM_ALPHA (only if blending is enabled)
*/
class SGP_API SPARKOpenGLES2PointRenderer : public ISGPParticleRenderer, public SPARK::PointRendererInterface
{
	SPARK_IMPLEMENT_REGISTERABLE(SPARKOpenGLES2PointRenderer)

public:
	SPARKOpenGLES2PointRenderer(ISGPRenderDevice* device, float _size = 1.0f);
	static SPARKOpenGLES2PointRenderer* create(ISGPRenderDevice* device, float _size = 1.0f);



	// Sets the texture id to map on particles
	void setTexture(uint32 textureID);

	// Reimplemented from SPARK::PointRendererInterface
	virtual bool setType(SPARK::PointType type);
	virtual void setSize(float _size);

	virtual void render(const SPARK::Group& group);

	void createBuffers(const SPARK::Group& group);

private:
	static const uint32 NUM_INDICES_PER_QUAD = 1;
	static const uint32 NUM_VERTICES_PER_QUAD = 1;

	// buffer name
	static const uint32 SPARK_BUFFER_NAME;

	virtual const uint32 getBufferID() const;

	uint32 m_nVertexStride;

};


inline SPARKOpenGLES2PointRenderer* SPARKOpenGLES2PointRenderer::create(ISGPRenderDevice* device, float _size)
{
	SPARKOpenGLES2PointRenderer* obj = new SPARKOpenGLES2PointRenderer(device, _size);
	registerObject(obj);
	return obj;
}
	
inline void SPARKOpenGLES2PointRenderer::setSize(float _size)
{
	m_material.Thickness = this->size = _size;
}

inline void SPARKOpenGLES2PointRenderer::setTexture(uint32 textureID)
{
	m_material.TextureID = textureID;
}

inline const uint32 SPARKOpenGLES2PointRenderer::getBufferID() const
{
	return SPARK_BUFFER_NAME;
}




#endif		// __SGP_OPENGLES2PARTICLEPOINTRENDERER_HEADER__