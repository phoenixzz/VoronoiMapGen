#ifndef __SGP_OPENGLES2PARTICLELINERENDERER_HEADER__
#define __SGP_OPENGLES2PARTICLELINERENDERER_HEADER__

/**
* A Renderer drawing particles as lines with SGP Engine
*
* The length of the lines is function of the Particle velocity and is defined in the universe space
* while the width is fixed and defines in the screen space (in pixels).
* Note that the width only works when using OpenGL. With Direct3D, the parameter is ignored and 1 is used instead.

* Below are the parameters of Particle that are used in this Renderer (others have no effects) :
		
		SPARK::PARAM_RED
		SPARK::PARAM_GREEN
		SPARK::PARAM_BLUE
		SPARK::PARAM_ALPHA (only if blending is enabled)

*/
class SGP_API SPARKOpenGLES2LineRenderer : public ISGPParticleRenderer, public SPARK::LineRendererInterface
{
	SPARK_IMPLEMENT_REGISTERABLE(SPARKOpenGLES2LineRenderer)
public:
	/**
	* Constructor of SPARKOpenGLES2LineRenderer
	*  length : the length multiplier of this Renderer
	*  width : the width of this Renderer in pixels
	*/
	SPARKOpenGLES2LineRenderer(ISGPRenderDevice* device, float length = 1.0f, float width = 1.0f);

	/**
	* Creates and registers a new SPARKOpenGLES2LineRenderer
	*  length : the length multiplier of this Renderer
	*  width : the width of this Renderer in pixels
	* return A new registered SPARKOpenGLLineRenderer
	*/
	static SPARKOpenGLES2LineRenderer* create(ISGPRenderDevice* device, float length = 1.0f, float width = 1.0f);



	virtual void setWidth(float width);


	virtual void render(const SPARK::Group& group);

	void createBuffers(const SPARK::Group& group);

private:
	static const uint32 NUM_INDICES_PER_QUAD = 2;
	static const uint32 NUM_VERTICES_PER_QUAD = 2;

	// buffer name
	static const uint32 SPARK_BUFFER_NAME;

	virtual const uint32 getBufferID() const;

	uint32 m_nVertexStride;
};


inline SPARKOpenGLES2LineRenderer* SPARKOpenGLES2LineRenderer::create(ISGPRenderDevice* device, float _length, float _width)
{
	SPARKOpenGLES2LineRenderer* obj = new SPARKOpenGLES2LineRenderer(device, _length, _width);
	registerObject(obj);
	return obj;
}

inline void SPARKOpenGLES2LineRenderer::setWidth(float width)
{
	m_material.Thickness = this->width = width;
}

inline const uint32 SPARKOpenGLES2LineRenderer::getBufferID() const
{
	return SPARK_BUFFER_NAME;
}

#endif		// __SGP_OPENGLES2PARTICLELINERENDERER_HEADER__