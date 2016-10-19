
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

OpenGLParticleDynamicBuffer::OpenGLParticleDynamicBuffer(COpenGLVertexCacheManager *pVCManager, uint32 nbParticles, uint32 particleVertexSize, uint32 particleIndexSize) :
	Buffer(),
	m_pVCManager(pVCManager),
	m_NumOfParticles(nbParticles),
	m_particleVertexSize(particleVertexSize),
	m_particleIndexSize(particleIndexSize),
	m_NumOfActiveParticles(0)
{

}

OpenGLParticleDynamicBuffer::OpenGLParticleDynamicBuffer(const OpenGLParticleDynamicBuffer& buffer) :
	Buffer(buffer),
	m_pVCManager(buffer.m_pVCManager),
	m_NumOfParticles(buffer.m_NumOfParticles),
	m_particleVertexSize(buffer.m_particleVertexSize),
	m_particleIndexSize(buffer.m_particleIndexSize),
	m_NumOfActiveParticles(buffer.m_NumOfActiveParticles)
{

}

OpenGLParticleDynamicBuffer::~OpenGLParticleDynamicBuffer()
{
	if(m_pVBO)
	{
		m_pVBO->deleteVBO(SGPBT_VERTEX_AND_INDEX);
		m_pVBO->deleteVAO();

		delete m_pVBO;
		m_pVBO = NULL;
	}
}

void OpenGLParticleDynamicBuffer::initVertexBuffers()
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


	}
	
	// create the VBO
	m_pVBO = new COpenGLVertexBufferObject( m_pVCManager->GetDevice() );

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
	}

	m_pVBO->bindVBO(SGPBT_INDEX);
	m_pVBO->initVBOBuffer(SGPBT_INDEX, m_NumOfParticles*m_particleIndexSize*sizeof(uint16), GL_STATIC_DRAW);
	
	m_pVBO->unBindVAO();

}

//void OpenGLParticleDynamicBuffer::swap(int index0, int index1)
//{
	// Not useful but implemented for consistency
	//int32 startIndex0 = index0 * particleVertexSize;
	//int32 startIndex1 = index1 * particleVertexSize;
	//for(int32 i = 0; i < particleVertexSize; ++i)
	//	swapVariables(getVertexBuffer()[startIndex0 + i],getVertexBuffer()[startIndex1 + i]);
	//meshBuffer.setDirty(irr::scene::EBT_VERTEX);
//}

void OpenGLParticleDynamicBuffer::updateMaterial(const SGPParticleMaterial& material)
{
	m_MaterialSkin.bAlpha = true;


	switch( material.ShaderType )
	{
	case SGPST_PARTICLE_POINTSPRITES:
		m_MaterialSkin.nShaderType = SGPST_PARTICLE_POINTSPRITES;
		m_MaterialSkin.nPrimitiveType = SGPPT_POINT_SPRITES;
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


	}



}
















OpenGLParticleDynamicBufferCreator::OpenGLParticleDynamicBufferCreator(COpenGLVertexCacheManager *pVCManager, uint32 particleVertexSize, uint32 particleIndexSize) :
	SPARK::BufferCreator(),
	m_pVCManager(pVCManager),
	particleVertexSize(particleVertexSize),
	particleIndexSize(particleIndexSize)
{}

OpenGLParticleDynamicBuffer* OpenGLParticleDynamicBufferCreator::createBuffer(int nbParticles, const SPARK::Group& /*group*/) const
{
	if( nbParticles * particleIndexSize > 65536 )
	{
		Logger::getCurrentLogger()->writeToLog(String("Unable to create particle buffer using 16bits indices : too many particles"), ELL_ERROR);
		return NULL;
	}
	else
		return new OpenGLParticleDynamicBuffer(m_pVCManager, nbParticles, particleVertexSize, particleIndexSize);
}

