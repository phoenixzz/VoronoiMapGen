#ifndef __SGP_OPENGLPARTICLELINERENDERER_HEADER__
#define __SGP_OPENGLPARTICLELINERENDERER_HEADER__

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
class SGP_API SPARKOpenGLLineRenderer : public ISGPParticleRenderer, public SPARK::LineRendererInterface
{
	SPARK_IMPLEMENT_REGISTERABLE(SPARKOpenGLLineRenderer)
public:
	/**
	* Constructor of SPARKOpenGLLineRenderer
	*  length : the length multiplier of this Renderer
	*  width : the width of this Renderer in pixels
	*/
	SPARKOpenGLLineRenderer(ISGPRenderDevice* device, float length = 1.0f, float width = 1.0f);

	/**
	* Creates and registers a new SPARKOpenGLLineRenderer
	*  length : the length multiplier of this Renderer
	*  width : the width of this Renderer in pixels
	* return A new registered SPARKOpenGLLineRenderer
	*/
	static SPARKOpenGLLineRenderer* create(ISGPRenderDevice* device, float length = 1.0f, float width = 1.0f);



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


inline SPARKOpenGLLineRenderer* SPARKOpenGLLineRenderer::create(ISGPRenderDevice* _device, float _length, float _width)
{
	SPARKOpenGLLineRenderer* obj = new SPARKOpenGLLineRenderer(_device, _length, _width);
	registerObject(obj);
	return obj;
}

inline void SPARKOpenGLLineRenderer::setWidth(float width)
{
	m_material.Thickness = this->width = width;
}

inline const uint32 SPARKOpenGLLineRenderer::getBufferID() const
{
	return SPARK_BUFFER_NAME;
}

#endif		// __SGP_OPENGLPARTICLELINERENDERER_HEADER__