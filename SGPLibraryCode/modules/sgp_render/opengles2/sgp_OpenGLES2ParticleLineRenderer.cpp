
const uint32 SPARKOpenGLES2LineRenderer::SPARK_BUFFER_NAME = String("SPK_OpenGLES2LineRenderer_Buffer").hashCode();

SPARKOpenGLES2LineRenderer::SPARKOpenGLES2LineRenderer(ISGPRenderDevice* device, float length, float width) :
	ISGPParticleRenderer(device),
	LineRendererInterface(length, width)
{
	m_material.Thickness = width;
	m_material.VertexType = SGPVT_UPOS_VERTEXCOLOR;
	m_material.ShaderType = SGPST_PARTICLE_LINE;
	m_nVertexStride = sizeof(SGPVertex_UPOS_VERTEXCOLOR);
}



void SPARKOpenGLES2LineRenderer::createBuffers(const SPARK::Group& group)
{
	m_currentBuffer = dynamic_cast<OpenGLES2ParticleDynamicBuffer*>(
		group.createBuffer( getBufferID(), OpenGLES2ParticleDynamicBufferCreator(
			(COpenGLES2VertexCacheManager*)m_renderDevice->GetVertexCacheManager(),
			NUM_VERTICES_PER_QUAD,																	
			NUM_INDICES_PER_QUAD ), 0, false ) );

	OpenGLES2ParticleDynamicBuffer *DB = static_cast<OpenGLES2ParticleDynamicBuffer*>(m_currentBuffer);
	DB->updateMaterial(	m_material );
	DB->initVertexBuffers();

		
	uint16 nbTotalIndices = (uint16)group.getParticles().getSizeOfReserved() * 2;

	DB->getVBO()->bindVBO(SGPBT_INDEX);
	uint16* tmp_pIndis = (uint16*)DB->getVBO()->mapBufferToMemory(SGPBT_INDEX, GL_WRITE_ONLY_OES);
		
    for(uint16 t = 0; t < nbTotalIndices; ++t)
        tmp_pIndis[t] = t;

	DB->getVBO()->unmapBuffer(SGPBT_INDEX);
}

void SPARKOpenGLES2LineRenderer::render(const SPARK::Group& group)
{
	if( !prepareBuffers(group) )
		return;
	if( group.getNumberOfParticles() == 0 )
		return;

	COpenGLES2RenderDevice *RI = static_cast<COpenGLES2RenderDevice*>(m_renderDevice);
	OpenGLES2ParticleDynamicBuffer *DB = static_cast<OpenGLES2ParticleDynamicBuffer*>(m_currentBuffer);

	DB->getVBO()->bindVBO(SGPBT_VERTEX);
	DB->getVBO()->initVBOBuffer(SGPBT_VERTEX, NUM_VERTICES_PER_QUAD*group.getNumberOfParticles()*m_nVertexStride, GL_STREAM_DRAW);

	SGPVertex_UPOS_VERTEXCOLOR* tmp_pVerts = (SGPVertex_UPOS_VERTEXCOLOR*)DB->getVBO()->mapBufferToMemory(SGPBT_VERTEX, GL_WRITE_ONLY_OES);

	for(uint32 t = 0; t < group.getNumberOfParticles() * 2; t += 2)
	{
		const SPARK::Particle& particle = group.getParticle(t >> 1);

		tmp_pVerts[t].x = particle.position().x;
		tmp_pVerts[t].y = particle.position().y;
		tmp_pVerts[t].z = particle.position().z;
		tmp_pVerts[t].VertexColor[0] = particle.getR();
		tmp_pVerts[t].VertexColor[1] = particle.getG();
		tmp_pVerts[t].VertexColor[2] = particle.getB();
		tmp_pVerts[t].VertexColor[3] = particle.getParamCurrentValue(SPARK::PARAM_ALPHA) * getInstanceAlpha();

		Vector3D targetPos = particle.position() + particle.velocity() * length * getInstanceScale();
		tmp_pVerts[t+1].x = targetPos.x;
		tmp_pVerts[t+1].y = targetPos.y;
		tmp_pVerts[t+1].z = targetPos.z;
		tmp_pVerts[t+1].VertexColor[0] = particle.getR();
		tmp_pVerts[t+1].VertexColor[1] = particle.getG();
		tmp_pVerts[t+1].VertexColor[2] = particle.getB();
		tmp_pVerts[t+1].VertexColor[3] = particle.getParamCurrentValue(SPARK::PARAM_ALPHA) * getInstanceAlpha();
	}

	DB->getVBO()->unmapBuffer(SGPBT_VERTEX);
	DB->m_NumOfActiveParticles = group.getNumberOfParticles();

	CParticleLineRenderBatch newRenderBatch(RI);
	newRenderBatch.m_BBOXCenter = group.getAABBox().vcCenter;
	newRenderBatch.m_MatWorld.Identity();	
	newRenderBatch.m_pSB = NULL;
	newRenderBatch.m_pVC = NULL;
	newRenderBatch.m_pParticleBuffer = DB;
	newRenderBatch.BuildQueueValue();
	RI->getOpenGLMaterialRenderer()->PushParticleLineRenderBatch(newRenderBatch);

}
