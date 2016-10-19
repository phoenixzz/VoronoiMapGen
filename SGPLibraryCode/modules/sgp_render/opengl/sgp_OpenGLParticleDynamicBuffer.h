#ifndef __SGP_OPENGLPARTICLEDYNAMICBUFFER_HEADER__
#define __SGP_OPENGLPARTICLEDYNAMICBUFFER_HEADER__

/**
* A buffer to store particle mesh data to be rendered with SGP OpenGL Engine
*
* The buffer can hold an vertex buffer and one index buffer of 16 bits indices.
* Moreover, it can be used as VBO and store on the GPU side in an optimized way.
*/
class SGP_API OpenGLParticleDynamicBuffer : public SPARK::Buffer
{
friend class OpenGLParticleDynamicBufferCreator;

public:
	inline COpenGLVertexBufferObject* getVBO() { return m_pVBO; }

	void updateMaterial(const SGPParticleMaterial& material);
	void initVertexBuffers();

	SGPSkin						m_MaterialSkin;	  // Render Material Skin

	uint32						m_NumOfParticles;
	uint32						m_particleVertexSize;
	uint32						m_particleIndexSize;

	float						m_Thickness;
	float						m_BlendMode;
	uint32						m_NumOfActiveParticles;

private:
    COpenGLVertexCacheManager	*m_pVCManager;
	COpenGLVertexBufferObject	*m_pVBO;


	OpenGLParticleDynamicBuffer(COpenGLVertexCacheManager *pVCManager, uint32 nbParticles, uint32 particleVertexSize, uint32 particleIndexSize);
	OpenGLParticleDynamicBuffer(const OpenGLParticleDynamicBuffer& buffer);
	virtual ~OpenGLParticleDynamicBuffer();

	// the assignment operator is private
	OpenGLParticleDynamicBuffer& operator=(const OpenGLParticleDynamicBuffer& buffer);

	virtual void swap(int , int ) {}

	
};



class SGP_API OpenGLParticleDynamicBufferCreator : public SPARK::BufferCreator
{
public:

	/**
	* Constructor of OpenGLParticleDynamicBufferCreator
	* pVCManager : A pointer to the OpenGL VertexCache Manager
	* particleVertexSize : the number of vertices per particle
	* particleIndexSize : the number of indices per particle
	*/
	OpenGLParticleDynamicBufferCreator(COpenGLVertexCacheManager *pVCManager, uint32 particleVertexSize, uint32 particleIndexSize);

private:	
	COpenGLVertexCacheManager* m_pVCManager;

	uint32 particleVertexSize;
	uint32 particleIndexSize;


    virtual OpenGLParticleDynamicBuffer* createBuffer(int nbParticles, const SPARK::Group& group) const;
};




//inline void IRRBuffer::setUsed(size_t nb)
//{
//	if (nb > nbParticles) // Prevents the buffers from growing
//		nb = nbParticles;
//
//	getVertexBuffer().set_used(nb * particleVertexSize);
//	getIndexBuffer().set_used(nb * particleIndexSize);
//}

//inline bool IRRBuffer::areVBOInitialized() const
//{
//	return VBOInitialized;
//}
//
//inline void IRRBuffer::setVBOInitialized(bool init)
//{
//	VBOInitialized = init;
//}



#endif		// __SGP_OPENGLPARTICLEDYNAMICBUFFER_HEADER__