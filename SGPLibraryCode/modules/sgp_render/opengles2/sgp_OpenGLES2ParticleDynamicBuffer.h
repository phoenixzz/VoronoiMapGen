#ifndef __SGP_OPENGLES2PARTICLEDYNAMICBUFFER_HEADER__
#define __SGP_OPENGLES2PARTICLEDYNAMICBUFFER_HEADER__

/**
* A buffer to store particle mesh data to be rendered with SGP OpenGL Engine
*
* The buffer can hold an vertex buffer and one index buffer of 16 bits indices.
* Moreover, it can be used as VBO and store on the GPU side in an optimized way.
*/
class SGP_API OpenGLES2ParticleDynamicBuffer : public SPARK::Buffer
{
friend class OpenGLES2ParticleDynamicBufferCreator;

public:
	inline COpenGLES2VertexBufferObject* getVBO() { return m_pVBO; }

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
    COpenGLES2VertexCacheManager	*m_pVCManager;
	COpenGLES2VertexBufferObject	*m_pVBO;


	OpenGLES2ParticleDynamicBuffer(COpenGLES2VertexCacheManager *pVCManager, uint32 nbParticles, uint32 particleVertexSize, uint32 particleIndexSize);
	OpenGLES2ParticleDynamicBuffer(const OpenGLES2ParticleDynamicBuffer& buffer);
	OpenGLES2ParticleDynamicBuffer& operator=(const OpenGLES2ParticleDynamicBuffer& buffer);
	virtual ~OpenGLES2ParticleDynamicBuffer();

	virtual void swap(int , int ) {}

	
};



class SGP_API OpenGLES2ParticleDynamicBufferCreator : public SPARK::BufferCreator
{
public:

	/**
	* Constructor of OpenGLParticleDynamicBufferCreator
	* pVCManager : A pointer to the OpenGL VertexCache Manager
	* particleVertexSize : the number of vertices per particle
	* particleIndexSize : the number of indices per particle
	*/
	OpenGLES2ParticleDynamicBufferCreator(COpenGLES2VertexCacheManager *pVCManager, uint32 particleVertexSize, uint32 particleIndexSize);

private:	
	COpenGLES2VertexCacheManager* m_pVCManager;

	uint32 particleVertexSize;
	uint32 particleIndexSize;


    virtual OpenGLES2ParticleDynamicBuffer* createBuffer(int nbParticles, const SPARK::Group& group) const;
};







#endif		// __SGP_OPENGLES2PARTICLEDYNAMICBUFFER_HEADER__