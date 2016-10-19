#ifndef __SGP_OPENGLES2PARTICLEQUADRENDERER_HEADER__
#define __SGP_OPENGLES2PARTICLEQUADRENDERER_HEADER__

/**
* A Renderer drawing particles as quads with SGP Engine
*
* The orientation of the quads depends on the orientation parameters set.
* This orientation is computed during rendering by the CPU.
* NOTE: this Renderer does NOT support TEXTURE_NONE mode
* Below are the parameters of Particle that are used in this Renderer (others have no effects) :
		
		SPK::PARAM_SIZE
		SPK::PARAM_RED
		SPK::PARAM_GREEN
		SPK::PARAM_BLUE
		SPK::PARAM_ALPHA (only if blending is enabled)
		SPK::PARAM_ANGLE
		SPK::PARAM_TEXTURE_INDEX (only if not in TEXTURE_NONE mode)

*/
class SGP_API SPARKOpenGLES2QuadRenderer :	public ISGPParticleRenderer,
											public SPARK::QuadRendererInterface,
											public SPARK::Oriented3DRendererInterface
{
	SPARK_IMPLEMENT_REGISTERABLE(SPARKOpenGLES2QuadRenderer)

public:
	/**
	*  device : the SGP Render device
	*  scaleX the scale of the width of the quad
	*  scaleY the scale of the height of the quad
	*/
	SPARKOpenGLES2QuadRenderer(ISGPRenderDevice* device, float scaleX = 1.0f, float scaleY = 1.0f);

	/**
	*  device : the SGP Render device
	*  scaleX the scale of the width of the quad
	*  scaleY the scale of the height of the quad
	*/
	static SPARKOpenGLES2QuadRenderer* create(ISGPRenderDevice* device, float scaleX = 1.0f, float scaleY = 1.0f);

	// Sets the texturing mode for this QuadRenderer,
	// Inherit from class QuadRendererInterface
	// NOTE: if setting TEXTURE_NONE mode, will return false
	virtual bool setTexturingMode(SPARK::TexturingMode mode);

	// Sets/Gets the texture to map on particles
	void setTexture(uint32 textureID);
	uint32 getTexture() const;

	// Sets/Gets the atlas dimension of the texture 
	void setAtlasDimensions(SDimension2D dim);
	using SPARK::QuadRendererInterface::setAtlasDimensions;

	SDimension2D getAtlasDimensions() const;

	virtual void render(const SPARK::Group& group);

	void createBuffers(const SPARK::Group& group);

private:
	static const uint32 NUM_INDICES_PER_QUAD = 6;
	static const uint32 NUM_VERTICES_PER_QUAD = 4;

	// buffer name
	static const uint32 SPARK_BUFFER_NAME;

	virtual const uint32 getBufferID() const;

	void FillBufferColorAndVertex(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const;	// Fills OpenGL buffer with color and position
	void FillBufferTextureAtlas(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const;	// Fills OpenGL buffer with atlas texture coordinates

	void (SPARKOpenGLES2QuadRenderer::*renderParticle)(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle&) const;	// pointer to the right render method

	void renderBasic(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const;		// Rendering for particles with texture or no texture
	void renderRot(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const;		// Rendering for particles with texture or no texture and rotation
	void renderAtlas(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const;		// Rendering for particles with texture atlas
	void renderAtlasRot(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const;	// Rendering for particles with texture atlas and rotation

	uint32 m_nVertexStride;
};


inline SPARKOpenGLES2QuadRenderer* SPARKOpenGLES2QuadRenderer::create(ISGPRenderDevice* device, float _scaleX, float _scaleY)
{
	SPARKOpenGLES2QuadRenderer* obj = new SPARKOpenGLES2QuadRenderer(device, _scaleX, _scaleY);
	registerObject(obj);
	return obj;
}

inline void SPARKOpenGLES2QuadRenderer::setTexture(uint32 textureID)
{
	m_material.TextureID = textureID;
}
inline uint32 SPARKOpenGLES2QuadRenderer::getTexture() const
{
	return m_material.TextureID;
}

inline void SPARKOpenGLES2QuadRenderer::setAtlasDimensions(SDimension2D dim)
{
	setAtlasDimensions(dim.Width, dim.Height);
}

inline SDimension2D SPARKOpenGLES2QuadRenderer::getAtlasDimensions() const
{
	return SDimension2D(textureAtlasNbX, textureAtlasNbY);
}

inline const uint32 SPARKOpenGLES2QuadRenderer::getBufferID() const
{
	return SPARK_BUFFER_NAME;
}



#endif		// __SGP_OPENGLES2PARTICLEQUADRENDERER_HEADER__