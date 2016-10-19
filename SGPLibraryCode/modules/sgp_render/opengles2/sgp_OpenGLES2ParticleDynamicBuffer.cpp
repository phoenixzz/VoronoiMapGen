


OpenGLES2ParticleDynamicBuffer::OpenGLES2ParticleDynamicBuffer(COpenGLES2VertexCacheManager *pVCManager, uint32 nbParticles, uint32 particleVertexSize, uint32 particleIndexSize) :
	Buffer(),
	m_pVCManager(pVCManager),
	m_NumOfParticles(nbParticles),
	m_particleVertexSize(particleVertexSize),
	m_particleIndexSize(particleIndexSize),
	m_NumOfActiveParticles(0)
{

}

OpenGLES2ParticleDynamicBuffer::OpenGLES2ParticleDynamicBuffer(const OpenGLES2ParticleDynamicBuffer& buffer) :
	Buffer(buffer),
	m_pVCManager(buffer.m_pVCManager),
	m_NumOfParticles(buffer.m_NumOfParticles),
	m_particleVertexSize(buffer.m_particleVertexSize),
	m_particleIndexSize(buffer.m_particleIndexSize),
	m_NumOfActiveParticles(buffer.m_NumOfActiveParticles)
{

}

OpenGLES2ParticleDynamicBuffer::~OpenGLES2ParticleDynamicBuffer()
{
	if(m_pVBO)
	{
		m_pVBO->deleteVBO(SGPBT_VERTEX_AND_INDEX);
		m_pVBO->deleteVAO();

		delete m_pVBO;
		m_pVBO = NULL;
	}
}

void OpenGLES2ParticleDynamicBuffer::initVertexBuffers()
{
	uint32 nStride = 0;
	switch(m_MaterialSkin.nShaderType) 
	{
	case SGPST_PARTICLE_POINTSPRITES:
	case SGPST_PARTICLE_LINE:
		nStride = sizeof(SGPVertex_UPOS_VERTEXCOLOR);
		break;
	case SGPST_PARTICLE_QUAD_TEXATLAS:
		nStride = sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX);
		break;

    default:
        break;
	}
	
	// create the VBO
	m_pVBO = new COpenGLES2VertexBufferObject( m_pVCManager->GetDevice() );

	m_pVBO->createVAO();
	m_pVBO->bindVAO();

	m_pVBO->createVBO(SGPBT_VERTEX);
	m_pVBO->createVBO(SGPBT_INDEX);

	m_pVBO->bindVBO(SGPBT_VERTEX);
	m_pVBO->initVBOBuffer(SGPBT_VERTEX, m_NumOfParticles*m_particleVertexSize*nStride, GL_STREAM_DRAW);

	switch(m_MaterialSkin.nShaderType) 
	{
	case SGPST_PARTICLE_POINTSPRITES:
	case SGPST_PARTICLE_LINE:
		m_pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
		m_pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		break;
	case SGPST_PARTICLE_QUAD_TEXATLAS:
		m_pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
		m_pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		m_pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		m_pVBO->setVAOPointer(3, 1, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
		break;
    default:
        break;
    }

	m_pVBO->bindVBO(SGPBT_INDEX);
	m_pVBO->initVBOBuffer(SGPBT_INDEX, m_NumOfParticles*m_particleIndexSize*sizeof(uint16), GL_STATIC_DRAW);
	
	m_pVBO->unBindVAO();

}



void OpenGLES2ParticleDynamicBuffer::updateMaterial(const SGPParticleMaterial& material)
{
	m_MaterialSkin.bAlpha = true;


	switch( material.ShaderType )
	{
	case SGPST_PARTICLE_POINTSPRITES:
		m_MaterialSkin.nShaderType = SGPST_PARTICLE_POINTSPRITES;
		m_MaterialSkin.nPrimitiveType = SGPPT_POINTS;
		this->m_Thickness = material.Thickness;
		this->m_BlendMode = material.Blend_Additive ? 0.0f : 1.0f;
		m_MaterialSkin.nTextureID[0] = material.TextureID;
		m_MaterialSkin.nTextureNum = 1;
		break;

	case SGPST_PARTICLE_LINE:
		m_MaterialSkin.nShaderType = SGPST_PARTICLE_LINE;
		m_MaterialSkin.nPrimitiveType = SGPPT_LINES;
		this->m_Thickness = material.Thickness;
		this->m_BlendMode = material.Blend_Additive ? 0.0f : 1.0f;
		m_MaterialSkin.nTextureNum = 0;
		break;

	case SGPST_PARTICLE_QUAD_TEXATLAS:
		m_MaterialSkin.nShaderType = SGPST_PARTICLE_QUAD_TEXATLAS;
		m_MaterialSkin.nPrimitiveType = SGPPT_TRIANGLES;
		this->m_BlendMode = material.Blend_Additive ? 0.0f : 1.0f;
		m_MaterialSkin.nTextureID[0] = material.TextureID;
		m_MaterialSkin.nTextureNum = 1;
		break;
    default:
        break;

	}



}
















OpenGLES2ParticleDynamicBufferCreator::OpenGLES2ParticleDynamicBufferCreator(COpenGLES2VertexCacheManager *pVCManager, uint32 particleVertexSize, uint32 particleIndexSize) :
	SPARK::BufferCreator(),
	m_pVCManager(pVCManager),
	particleVertexSize(particleVertexSize),
	particleIndexSize(particleIndexSize)
{}

OpenGLES2ParticleDynamicBuffer* OpenGLES2ParticleDynamicBufferCreator::createBuffer(int nbParticles, const SPARK::Group& /*group*/) const
{
	if( nbParticles * particleIndexSize > 65536 )
	{
		Logger::getCurrentLogger()->writeToLog(String("Unable to create particle buffer using 16bits indices : too many particles"), ELL_ERROR);
		return NULL;
	}
	else
		return new OpenGLES2ParticleDynamicBuffer(m_pVCManager, nbParticles, particleVertexSize, particleIndexSize);
}

