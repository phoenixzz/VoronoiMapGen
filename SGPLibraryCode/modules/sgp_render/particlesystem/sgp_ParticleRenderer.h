#ifndef __SGP_PARTICLERENDERER_HEADER__
#define __SGP_PARTICLERENDERER_HEADER__


//! Struct for holding parameters for a SPARK Particle renderer
struct SGPParticleMaterial
{
	SGP_VERTEX_TYPE VertexType;	// Vertex Type for particle 
	SGP_SHADER_TYPE ShaderType;	// shader type for particle 
	uint32 TextureID;			// OpenGL texture ID
	bool Blend_Additive;		// use alpha blend additive?
	float Thickness;			// Thickness of non-3dimensional elements such as lines and points.

	SGPParticleMaterial() : VertexType(SGPVT_UPOS_VERTEXCOLOR),
		ShaderType(SGPST_PARTICLE_POINTSPRITES),
		TextureID(0),
		Blend_Additive(false),
		Thickness(1.0f)
	{}
};


/**
* The base renderer for all SGP Engine Particle renderers
* 
* This class presents a convenient interface to set some parameters common to all Particle renderers (blending mode...).

* Note that rendering hints work with SGP renderers except the SPARK::ALPHA_TEST
* which is always enabled with a threshold of 0. (meaning alpha values of 0 are never rendered).

*/
class SGP_API ISGPParticleRenderer : public SPARK::Renderer
{
public:

	ISGPParticleRenderer(ISGPRenderDevice* device);
	virtual ~ISGPParticleRenderer(){};


	/**
	* Sets the blending mode
	* This method allows to set any blending mode supported by SGP SPARK ENGINE.
	*/
	virtual void setBlending(SPARK::BlendingMode blendMode) 
	{
		if( blendMode == SPARK::BLENDING_ADD )
			m_material.Blend_Additive = true;
		else
			m_material.Blend_Additive = false;
	}

	virtual void enableRenderingHint(SPARK::RenderingHint , bool ) {}
	virtual void setAlphaTestThreshold(float ) {}


	inline ISGPRenderDevice* getRenderDevice() const { return m_renderDevice; }




	//SGP_BLEND_FACTOR getBlendSrcFunc() const;
	//SGP_BLEND_FACTOR getBlendDestFunc() const;
	//uint32 getAlphaSource() const;

	/**
	* Gets the material of this renderer
	* Note that the renderer is constant and therefore cannot be modified directly
	*/
	inline const SGPParticleMaterial& getMaterialSkin() const
	{ return m_material; }

	virtual bool isRenderingHintEnabled(SPARK::RenderingHint ) const { return false; }

	virtual void destroyBuffers(const SPARK::Group& group);

protected:
	ISGPRenderDevice*		m_renderDevice;		// the device
	SGPParticleMaterial		m_material;			// the material

	SPARK::Buffer*			m_currentBuffer;

	virtual bool checkBuffers(const SPARK::Group& group);


private:
	//SGP_BLEND_FACTOR blendSrcFunc;
	//SGP_BLEND_FACTOR blendDestFunc;
	//uint32 alphaSource;

	/**
	* Gets the ID of the ISGPParticleBuffer used by the renderer
	* This method must be implemented in all children renderers
	* return the ID of the ISGPParticleBuffer
	*/
	virtual const uint32 getBufferID() const = 0;

	//void updateMaterialBlendingMode();
};

	
//inline void ISGPParticleRenderer::setAlphaTestThreshold(float alphaThreshold)
//{
//	SPARK::Renderer::setAlphaTestThreshold(0.0f); // the alpha threshold of the SGP material is always 0
//}
//	
//
//
//inline SGP_BLEND_FACTOR ISGPParticleRenderer::getBlendSrcFunc() const
//{
//	return blendSrcFunc;
//}
//
//inline SGP_BLEND_FACTOR ISGPParticleRenderer::getBlendDestFunc() const
//{
//	return blendDestFunc;
//}
//
//inline uint32 ISGPParticleRenderer::getAlphaSource() const
//{
//	return alphaSource;
//}



inline void ISGPParticleRenderer::destroyBuffers(const SPARK::Group& group)
{
	group.destroyBuffer( getBufferID() );
}

inline bool ISGPParticleRenderer::checkBuffers(const SPARK::Group& group)
{
	m_currentBuffer = dynamic_cast<SPARK::Buffer*>( group.getBuffer(getBufferID()) );
	return m_currentBuffer != NULL;
}



//inline void ISGPParticleRenderer::updateMaterialBlendingMode()
//{
//	m_material.AlphaParam = pack_textureBlendFunc(
//		blendSrcFunc,
//		blendDestFunc,
//		SGPMFN_MODULATE_1X,
//		alphaSource);
//}


#endif		// __SGP_PARTICLERENDERER_HEADER__