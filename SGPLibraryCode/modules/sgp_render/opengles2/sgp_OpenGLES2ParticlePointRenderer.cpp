

const uint32 SPARKOpenGLES2PointRenderer::SPARK_BUFFER_NAME = String("SPK_OpenGLES2PointRenderer_Buffer").hashCode();

SPARKOpenGLES2PointRenderer::SPARKOpenGLES2PointRenderer(ISGPRenderDevice* device, float _size) :
	ISGPParticleRenderer(device),
	SPARK::PointRendererInterface(SPARK::POINT_SPRITE, _size)
{
	m_material.Thickness = _size;
	m_material.VertexType = SGPVT_UPOS_VERTEXCOLOR;
	m_material.ShaderType = SGPST_PARTICLE_POINTSPRITES;
	m_nVertexStride = sizeof(SGPVertex_UPOS_VERTEXCOLOR);
}

bool SPARKOpenGLES2PointRenderer::setType(SPARK::PointType _type)
{
	if( _type == SPARK::POINT_SPRITE )
	{
		this->type = _type;
		return true;
	}
	Logger::getCurrentLogger()->writeToLog(String("SPARKOpenGLES2PointRenderer unable support point type when setting particle point renderer"), ELL_ERROR);
	return false;
}



void SPARKOpenGLES2PointRenderer::createBuffers(const SPARK::Group& group)
{
	m_currentBuffer = dynamic_cast<OpenGLES2ParticleDynamicBuffer*>( 
		group.createBuffer( getBufferID(), OpenGLES2ParticleDynamicBufferCreator(
			(COpenGLES2VertexCacheManager*)m_renderDevice->GetVertexCacheManager(),
			NUM_VERTICES_PER_QUAD,																	
			NUM_INDICES_PER_QUAD ),	0, false ) );

	OpenGLES2ParticleDynamicBuffer *DB = static_cast<OpenGLES2ParticleDynamicBuffer*>(m_currentBuffer);
	DB->updateMaterial(	m_material );
	DB->initVertexBuffers();

	uint32 nbTotalParticles = group.getParticles().getSizeOfReserved();

	DB->getVBO()->bindVBO(SGPBT_INDEX);
	uint16* tmp_pIndis = (uint16*)DB->getVBO()->mapBufferToMemory(SGPBT_INDEX,  GL_WRITE_ONLY_OES);

    for(uint32 t = 0; t < nbTotalParticles; ++t)
        tmp_pIndis[t] = (uint16)t;

	DB->getVBO()->unmapBuffer(SGPBT_INDEX);

}

void SPARKOpenGLES2PointRenderer::render(const SPARK::Group& group)
{
	if( !prepareBuffers(group) )
		return;
	if( group.getNumberOfParticles() == 0 )
		return;

	COpenGLES2RenderDevice *RI = static_cast<COpenGLES2RenderDevice*>(m_renderDevice);
	OpenGLES2ParticleDynamicBuffer *DB = static_cast<OpenGLES2ParticleDynamicBuffer*>(m_currentBuffer);

	// Update scale
	DB->m_Thickness = getSize() * getInstanceScale();
	

	DB->getVBO()->bindVBO(SGPBT_VERTEX);
	DB->getVBO()->initVBOBuffer(SGPBT_VERTEX, NUM_VERTICES_PER_QUAD*group.getNumberOfParticles()*m_nVertexStride, GL_STREAM_DRAW);

	SGPVertex_UPOS_VERTEXCOLOR* tmp_pVerts = (SGPVertex_UPOS_VERTEXCOLOR*)DB->getVBO()->mapBufferToMemory(SGPBT_VERTEX, GL_WRITE_ONLY_OES);

	for(uint32 t = 0; t < group.getNumberOfParticles(); ++t)
	{
		const SPARK::Particle& p = group.getParticle(t);
		tmp_pVerts[t].x = p.position().x;
		tmp_pVerts[t].y = p.position().y;
		tmp_pVerts[t].z = p.position().z;
		tmp_pVerts[t].VertexColor[0] = p.getR();
		tmp_pVerts[t].VertexColor[1] = p.getG();
		tmp_pVerts[t].VertexColor[2] = p.getB();
		tmp_pVerts[t].VertexColor[3] = p.getParamCurrentValue(SPARK::PARAM_ALPHA) * getInstanceAlpha();
	}

	DB->getVBO()->unmapBuffer(SGPBT_VERTEX);
	DB->m_NumOfActiveParticles = group.getNumberOfParticles();


	
	CParticlePSRenderBatch newRenderBatch(RI);
	newRenderBatch.m_BBOXCenter = group.getAABBox().vcCenter;
	newRenderBatch.m_MatWorld.Identity();	
	newRenderBatch.m_pSB = NULL;
	newRenderBatch.m_pVC = NULL;
	newRenderBatch.m_pParticleBuffer = DB;
	newRenderBatch.BuildQueueValue();
	RI->getOpenGLMaterialRenderer()->PushParticlePSRenderBatch(newRenderBatch);

}