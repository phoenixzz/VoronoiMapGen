

const uint32 SPARKOpenGLQuadRenderer::SPARK_BUFFER_NAME = String("SPK_OpenGLQuadRenderer_Buffer").hashCode();


SPARKOpenGLQuadRenderer::SPARKOpenGLQuadRenderer(ISGPRenderDevice* device, float scaleX, float scaleY) :
	ISGPParticleRenderer(device),
	SPARK::QuadRendererInterface(scaleX, scaleY),
	SPARK::Oriented3DRendererInterface(),
	renderParticle(NULL)
{
}

bool SPARKOpenGLQuadRenderer::setTexturingMode(SPARK::TexturingMode mode)
{
	if( mode == SPARK::TEXTURE_NONE )
	{
		Logger::getCurrentLogger()->writeToLog(String("SPARKOpenGLQuadRenderer could not support TEXTURE_NONE mode when setting particle quad renderer"), ELL_ERROR);
		texturingMode = SPARK::TEXTURE_2D;
		return false;
	}
	texturingMode = mode;
	return true;
}

void SPARKOpenGLQuadRenderer::createBuffers(const SPARK::Group& group)
{
	m_currentBuffer = dynamic_cast<OpenGLParticleDynamicBuffer*>(
		group.createBuffer( getBufferID(), OpenGLParticleDynamicBufferCreator(
			(COpenGLVertexCacheManager*)m_renderDevice->GetVertexCacheManager(),
			NUM_VERTICES_PER_QUAD,																	
			NUM_INDICES_PER_QUAD ), 0, false ) );

	OpenGLParticleDynamicBuffer *DB = static_cast<OpenGLParticleDynamicBuffer*>(m_currentBuffer);

	if( (texturingMode == SPARK::TEXTURE_2D) && group.getModel()->isEnabled(SPARK::PARAM_TEXTURE_INDEX) )
	{
		if( group.getModel()->isEnabled(SPARK::PARAM_ANGLE) )
			renderParticle = &SPARKOpenGLQuadRenderer::renderAtlasRot;
		else
			renderParticle = &SPARKOpenGLQuadRenderer::renderAtlas;
	}
	else
	{
		if( group.getModel()->isEnabled(SPARK::PARAM_ANGLE) )
			renderParticle = &SPARKOpenGLQuadRenderer::renderRot;
		else
			renderParticle = &SPARKOpenGLQuadRenderer::renderBasic;
	}

	m_material.VertexType = SGPVT_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX;
	m_material.ShaderType = SGPST_PARTICLE_QUAD_TEXATLAS;
	m_nVertexStride = sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX);

	DB->updateMaterial(	m_material );
	DB->initVertexBuffers();

	uint32 nbTotalParticles = group.getParticles().getSizeOfReserved();

	DB->getVBO()->bindVBO(SGPBT_INDEX);
	uint16* tmp_pIndis = (uint16*)DB->getVBO()->mapSubBufferToMemory(SGPBT_INDEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nbTotalParticles*NUM_INDICES_PER_QUAD*sizeof(uint16));

    for(uint32 t = 0; t < nbTotalParticles; ++t)
	{
        tmp_pIndis[NUM_INDICES_PER_QUAD*t+0] = (uint16)(NUM_VERTICES_PER_QUAD*t+0);
        tmp_pIndis[NUM_INDICES_PER_QUAD*t+1] = (uint16)(NUM_VERTICES_PER_QUAD*t+2);
        tmp_pIndis[NUM_INDICES_PER_QUAD*t+2] = (uint16)(NUM_VERTICES_PER_QUAD*t+1);
        tmp_pIndis[NUM_INDICES_PER_QUAD*t+3] = (uint16)(NUM_VERTICES_PER_QUAD*t+0);
        tmp_pIndis[NUM_INDICES_PER_QUAD*t+4] = (uint16)(NUM_VERTICES_PER_QUAD*t+3);
        tmp_pIndis[NUM_INDICES_PER_QUAD*t+5] = (uint16)(NUM_VERTICES_PER_QUAD*t+2);
	}

	DB->getVBO()->unmapBuffer(SGPBT_INDEX);
}

void SPARKOpenGLQuadRenderer::render(const SPARK::Group& group)
{
	if( !prepareBuffers(group) )
		return;
	if( group.getNumberOfParticles() == 0 )
		return;

	COpenGLRenderDevice *RI = static_cast<COpenGLRenderDevice*>(m_renderDevice);
	OpenGLParticleDynamicBuffer *DB = static_cast<OpenGLParticleDynamicBuffer*>(m_currentBuffer);
	
	// Computes the inverse model view
	Matrix4x4 invModelView;
	invModelView.InverseOf( RI->getOpenGLCamera()->m_mViewMatrix );

	DB->getVBO()->bindVBO(SGPBT_VERTEX);
	SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* tmp_pVerts = (SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX*)DB->getVBO()->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, NUM_VERTICES_PER_QUAD*group.getNumberOfParticles()*m_nVertexStride);

	bool globalOrientation = precomputeOrientation3D(
		group,
		Vector3D(invModelView._31, invModelView._32, invModelView._33),
		Vector3D(invModelView._21, invModelView._22, invModelView._23),
		Vector3D(invModelView._41, invModelView._42, invModelView._43));

	if(globalOrientation)
	{
		computeGlobalOrientation3D();

		for(uint32 t = 0; t < group.getNumberOfParticles(); ++t)
		{
			(this->*renderParticle)(tmp_pVerts, group.getParticle(t));
			tmp_pVerts += NUM_VERTICES_PER_QUAD;
		}
	}
	else
	{
		for(uint32 t = 0; t < group.getNumberOfParticles(); ++t)
		{
			const SPARK::Particle& particle = group.getParticle(t);
			computeSingleOrientation3D(particle);
			(this->*renderParticle)(tmp_pVerts, particle);
			tmp_pVerts += NUM_VERTICES_PER_QUAD;
		}
	}

	DB->getVBO()->unmapBuffer(SGPBT_VERTEX);
	DB->m_NumOfActiveParticles = group.getNumberOfParticles();


	CParticleQuadRenderBatch newRenderBatch(RI);
	newRenderBatch.m_BBOXCenter = group.getAABBox().vcCenter;
	newRenderBatch.m_MatWorld.Identity();	
	newRenderBatch.m_pSB = NULL;
	newRenderBatch.m_pVC = NULL;
	newRenderBatch.m_pParticleBuffer = DB;
	newRenderBatch.m_TextureAtlas = Vector4D( (float)getAtlasDimensionX(), (float)getAtlasDimensionY(), 1.0f/getAtlasDimensionX(), 1.0f/getAtlasDimensionY() );
	newRenderBatch.BuildQueueValue();
	RI->getOpenGLMaterialRenderer()->PushParticleQuadRenderBatch(newRenderBatch);
}

void SPARKOpenGLQuadRenderer::renderBasic(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const
{
	scaleQuadVectors(particle, scaleX*getInstanceScale(), scaleY*getInstanceScale());
	FillBufferColorAndVertex(pVertex, particle);
}

void SPARKOpenGLQuadRenderer::renderRot(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const
{
	rotateAndScaleQuadVectors(particle, scaleX*getInstanceScale(), scaleY*getInstanceScale());
	FillBufferColorAndVertex(pVertex, particle);
}

void SPARKOpenGLQuadRenderer::renderAtlas(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const
{
	scaleQuadVectors(particle, scaleX*getInstanceScale(), scaleY*getInstanceScale());
	FillBufferColorAndVertex(pVertex, particle);
	FillBufferTextureAtlas(pVertex, particle);
}

void SPARKOpenGLQuadRenderer::renderAtlasRot(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const
{
	rotateAndScaleQuadVectors(particle, scaleX*getInstanceScale(), scaleY*getInstanceScale());
	FillBufferColorAndVertex(pVertex, particle);
	FillBufferTextureAtlas(pVertex, particle);
}


void SPARKOpenGLQuadRenderer::FillBufferColorAndVertex( SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle ) const
{
	float x = particle.position().x;
	float y = particle.position().y;
	float z = particle.position().z;

	// According to SGP Engine coordinates system, quads are drawn in anticlockwise order
	// Note that the quad side points towards the left as it is a left handed system
	
	// top left vertex
	pVertex[0].x =  x + quadSide().x + quadUp().x;
	pVertex[0].y =  y + quadSide().y + quadUp().y;
	pVertex[0].z =  z + quadSide().z + quadUp().z;
	pVertex[0].VertexColor[0] = particle.getR();
	pVertex[0].VertexColor[1] = particle.getG();
	pVertex[0].VertexColor[2] = particle.getB();
	pVertex[0].VertexColor[3] = particle.getParamCurrentValue(SPARK::PARAM_ALPHA) * getInstanceAlpha();
	pVertex[0].tu = 0;
	pVertex[0].tv = 0;
	pVertex[0].textureIndex = 0;

	// top right vertex
	pVertex[1].x =	x - quadSide().x + quadUp().x;
	pVertex[1].y =	y - quadSide().y + quadUp().y;
	pVertex[1].z =	z - quadSide().z + quadUp().z;
	pVertex[1].VertexColor[0] = pVertex[0].VertexColor[0];
	pVertex[1].VertexColor[1] =	pVertex[0].VertexColor[1];
	pVertex[1].VertexColor[2] =	pVertex[0].VertexColor[2];
	pVertex[1].VertexColor[3] =	pVertex[0].VertexColor[3];
	pVertex[1].tu = 1.0f;
	pVertex[1].tv = 0;
	pVertex[1].textureIndex = 0;

	// bottom right vertex
	pVertex[2].x =	x - quadSide().x - quadUp().x;
	pVertex[2].y =	y - quadSide().y - quadUp().y;
	pVertex[2].z =	z - quadSide().z - quadUp().z;
	pVertex[2].VertexColor[0] = pVertex[0].VertexColor[0];
	pVertex[2].VertexColor[1] =	pVertex[0].VertexColor[1];
	pVertex[2].VertexColor[2] =	pVertex[0].VertexColor[2];
	pVertex[2].VertexColor[3] =	pVertex[0].VertexColor[3];
	pVertex[2].tu = 1.0f;
	pVertex[2].tv = 1.0f;
	pVertex[2].textureIndex = 0;

	// bottom left vertex
	pVertex[3].x =	x + quadSide().x - quadUp().x;
	pVertex[3].y =	y + quadSide().y - quadUp().y;
	pVertex[3].z =	z + quadSide().z - quadUp().z;
	pVertex[3].VertexColor[0] = pVertex[0].VertexColor[0];
	pVertex[3].VertexColor[1] =	pVertex[0].VertexColor[1];
	pVertex[3].VertexColor[2] =	pVertex[0].VertexColor[2];
	pVertex[3].VertexColor[3] =	pVertex[0].VertexColor[3];
	pVertex[3].tu = 0;
	pVertex[3].tv = 1.0f;
	pVertex[3].textureIndex = 0;
}

void SPARKOpenGLQuadRenderer::FillBufferTextureAtlas(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX* pVertex, const SPARK::Particle& particle) const
{
	pVertex[0].textureIndex = pVertex[1].textureIndex = pVertex[2].textureIndex = pVertex[3].textureIndex =	(float)(static_cast<int>(particle.getParamCurrentValue(SPARK::PARAM_TEXTURE_INDEX)));
}