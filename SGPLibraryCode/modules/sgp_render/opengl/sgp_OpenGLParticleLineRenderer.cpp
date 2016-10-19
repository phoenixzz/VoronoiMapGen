
const uint32 SPARKOpenGLLineRenderer::SPARK_BUFFER_NAME = String("SPK_OpenGLLineRenderer_Buffer").hashCode();

SPARKOpenGLLineRenderer::SPARKOpenGLLineRenderer(ISGPRenderDevice* device, float length, float width) :
	ISGPParticleRenderer(device),
	LineRendererInterface(length, width)
{
	m_material.Thickness = width;
	m_material.VertexType = SGPVT_UPOS_VERTEXCOLOR;
	m_material.ShaderType = SGPST_PARTICLE_LINE;
	m_nVertexStride = sizeof(SGPVertex_UPOS_VERTEXCOLOR);
}



void SPARKOpenGLLineRenderer::createBuffers(const SPARK::Group& group)
{
	m_currentBuffer = dynamic_cast<OpenGLParticleDynamicBuffer*>(
		group.createBuffer( getBufferID(), OpenGLParticleDynamicBufferCreator(
			(COpenGLVertexCacheManager*)m_renderDevice->GetVertexCacheManager(),
			NUM_VERTICES_PER_QUAD,																	
			NUM_INDICES_PER_QUAD ), 0, false ) );

	OpenGLParticleDynamicBuffer *DB = static_cast<OpenGLParticleDynamicBuffer*>(m_currentBuffer);
	DB->updateMaterial(	m_material );
	DB->initVertexBuffers();

		
	uint16 nbTotalIndices = (uint16)group.getParticles().getSizeOfReserved() * 2;

	DB->getVBO()->bindVBO(SGPBT_INDEX);
	uint16* tmp_pIndis = (uint16*)DB->getVBO()->mapSubBufferToMemory(SGPBT_INDEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nbTotalIndices*sizeof(uint16));
		
    for(uint16 t = 0; t < nbTotalIndices; ++t)
        tmp_pIndis[t] = t;

	DB->getVBO()->unmapBuffer(SGPBT_INDEX);
}

void SPARKOpenGLLineRenderer::render(const SPARK::Group& group)
{
	if( !prepareBuffers(group) )
		return;
	if( group.getNumberOfParticles() == 0 )
		return;

	COpenGLRenderDevice *RI = static_cast<COpenGLRenderDevice*>(m_renderDevice);
	OpenGLParticleDynamicBuffer *DB = static_cast<OpenGLParticleDynamicBuffer*>(m_currentBuffer);

	DB->getVBO()->bindVBO(SGPBT_VERTEX);
	SGPVertex_UPOS_VERTEXCOLOR* tmp_pVerts = (SGPVertex_UPOS_VERTEXCOLOR*)DB->getVBO()->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, NUM_VERTICES_PER_QUAD*group.getNumberOfParticles()*m_nVertexStride);

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
