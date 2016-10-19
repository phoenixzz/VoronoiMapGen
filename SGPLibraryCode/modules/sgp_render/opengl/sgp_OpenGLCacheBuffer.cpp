
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


COpenGLStaticBuffer::~COpenGLStaticBuffer()
{
	if( pVBO )
	{
		pVBO->deleteVBO(SGPBT_VERTEX_AND_INDEX);
		pVBO->deleteVAO();

		delete pVBO;
		pVBO = NULL;
	}
}

COpenGLDynamicBuffer::COpenGLDynamicBuffer( uint32 nVertsMax, 
	uint32 nIndisMax, 
	uint32 nStride, 
	SGP_VERTEX_TYPE vertextype,
	COpenGLVertexCacheManager *pVCManager )
	: m_pVCManager(pVCManager), m_BoundingBox(Vector3D(0,0,0), Vector3D(0,0,0))	
{
	m_nStride			= nStride;
	m_nNumVertsMax		= nVertsMax;
	m_nNumIndisMax		= nIndisMax;
	m_nNumVerts			= 0;
	m_nNumIndis			= 0;


	m_bCommited			= false;

	// create the VBO
	m_pVBO = new COpenGLVertexBufferObject( m_pVCManager->GetDevice() );

	m_pVBO->createVAO();
	m_pVBO->bindVAO();

	m_pVBO->createVBO(SGPBT_VERTEX);
	m_pVBO->createVBO(SGPBT_INDEX);

	m_pVBO->bindVBO(SGPBT_VERTEX);
	m_pVBO->initVBOBuffer(SGPBT_VERTEX, m_nNumVertsMax * m_nStride, GL_DYNAMIC_DRAW);
	switch(vertextype)
	{
	case SGPVT_UPOS_VERTEXCOLOR:
		m_pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(0));
		m_pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		break;
	case SGPVT_UPOS_TEXTURE:
		m_pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(0));
		m_pVBO->setVAOPointer(1, 2, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		break;
	case SGPVT_UPOS_TEXTURE_VERTEXCOLOR:
		m_pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(0));
		m_pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		m_pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, m_nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		break;
	default:
		jassertfalse;
		Logger::getCurrentLogger()->writeToLog(String("Only Support Three Dynamic Buffer Vertex Type"), ELL_ERROR);
		break;
	}

	m_pVBO->bindVBO(SGPBT_INDEX);
	m_pVBO->initVBOBuffer(SGPBT_INDEX, m_nNumIndisMax*sizeof(uint16), GL_DYNAMIC_DRAW);
	
	m_pVBO->unBindVAO();
}

COpenGLDynamicBuffer::~COpenGLDynamicBuffer(void)
{
	if(m_pVBO) 
	{
		m_pVBO->deleteVBO(SGPBT_VERTEX_AND_INDEX);
		m_pVBO->deleteVAO();

		delete m_pVBO;
		m_pVBO = NULL;
	}
	//if( m_MaterialModifier.m_material )
	//	delete m_MaterialModifier.m_material;
	//m_MaterialModifier.m_material = NULL;
}



/**
 * Set the given Material Skin
 */
void COpenGLDynamicBuffer::SetMaterialSkin(const SGPSkin& skin)
{
	// if a new texture or material is coming, flush all content of
	// the cache because that is using other material
	if( !IsUseSameSkin(skin) )
	{
		// New TexID so commit it
		if( IsNotEmpty() ) 
			Commit();
      
		m_MaterialSkin = skin;
	}
}

bool COpenGLDynamicBuffer::IsUseSameSkin(const SGPSkin& skin)
{
	return (m_MaterialSkin == skin);
}

/**
 * Fill data into the cache. If the cache is full it will be commited to render batch.
 * If no Indexlist is used set indexlist parameter to NULL.
 * -> IN: uint32  - number of vertices in list
 *        uint32  - number of indices in list
 *        void*   - pointer to vertex list
 *        uint16* - pointer to index list
 */
void COpenGLDynamicBuffer::Add(uint32 nVertexNum, uint32 nIndexNum, const void *pVerts, const uint16 *pIndis) 
{
	uint8  *tmp_pVerts = NULL;         // pointer to VB memory
	uint16 *tmp_pIndis = NULL;         // pointer to IB memory

	if( !pIndis ) 
		nIndexNum = nVertexNum;
   
	uint32 nSizeV = m_nStride * nVertexNum;
	uint32 nSizeI = sizeof(uint16) * nIndexNum;

	int nPosV = 0;
	int nPosI = 0;


	// lists will never fit into this cache
	if(nVertexNum > m_nNumVertsMax || nIndexNum > m_nNumIndisMax)
	{
		jassertfalse;
		Logger::getCurrentLogger()->writeToLog(String("Too many vertex or index for Dynamic Buffer"), ELL_ERROR);
		return;
	}

	// if nothing is in buffer discard content
	if(m_nNumVerts == 0) 
	{
		nPosV = nPosI = 0;

		m_pVBO->bindVBO(SGPBT_VERTEX);
		tmp_pVerts = (uint8*)m_pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, nPosV, nSizeV);
		m_pVBO->bindVBO(SGPBT_INDEX);
		tmp_pIndis = (uint16*)m_pVBO->mapSubBufferToMemory(SGPBT_INDEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, nPosI, nSizeI);
	}	
	else	// else just append data without overwrites
	{
		nPosV = m_nStride * m_nNumVerts;
		nPosI = sizeof(uint16) * m_nNumIndis;

		m_pVBO->bindVBO(SGPBT_VERTEX);
		tmp_pVerts = (uint8*)m_pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT, nPosV, nSizeV);
		m_pVBO->bindVBO(SGPBT_INDEX);
		tmp_pIndis = (uint16*)m_pVBO->mapSubBufferToMemory(SGPBT_INDEX, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT, nPosI, nSizeI);
    }



	// copy vertex data into the buffer
	memcpy(tmp_pVerts, pVerts, nSizeV);

	// copy indices into the buffer
	uint16 nBase = (uint16)m_nNumVerts;

	for(uint16 i=0; i<nIndexNum; i++)
	{
		if(pIndis != NULL)
			tmp_pIndis[i] = pIndis[i] + nBase;
		else
			tmp_pIndis[i] = i + nBase;
		m_nNumIndis++;
	}

	// add to count
	m_nNumVerts += nVertexNum;

	m_pVBO->unmapBuffer(SGPBT_VERTEX_AND_INDEX);

	return;
}


/**
 * Send the content of the cache to the renderbatch.
 */
void COpenGLDynamicBuffer::Commit()
{
	// Have commited? OR anything to do at all?
	if( m_nNumVerts <= 0 || m_bCommited )
		return;

//	m_pVCManager->GetDevice()->getOpenGLMaterialRenderer()->CommitAdditionalMaterial(m_MaterialModifier, m_MaterialSkin);

	// Commit this cache to RenderBatch
	if( m_MaterialSkin.nPrimitiveType == SGPPT_POINTS ||
		m_MaterialSkin.nPrimitiveType == SGPPT_LINE_STRIP ||
		m_MaterialSkin.nPrimitiveType == SGPPT_LINE_LOOP ||
		m_MaterialSkin.nPrimitiveType == SGPPT_LINES )
	{
		CLineRenderBatch newRenderBatch(m_pVCManager->GetDevice());
		newRenderBatch.m_BBOXCenter = m_BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld.Identity();	
		newRenderBatch.m_pSB = NULL;
		newRenderBatch.m_pVC = this;
		newRenderBatch.BuildQueueValue();
		m_pVCManager->GetDevice()->getOpenGLMaterialRenderer()->PushLineRenderBatch(newRenderBatch);
	}
	else if( m_MaterialSkin.bAlpha )
	{
		//transparent
		CTransparentRenderBatch newRenderBatch(m_pVCManager->GetDevice());
		newRenderBatch.m_BBOXCenter = m_BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld.Identity();	
		newRenderBatch.m_pSB = NULL;
		newRenderBatch.m_pVC = this;
		newRenderBatch.BuildQueueValue();
		m_pVCManager->GetDevice()->getOpenGLMaterialRenderer()->PushTransparentRenderBatch(newRenderBatch);
	}
	else
	{
		//opaque
		COpaqueRenderBatch newRenderBatch(m_pVCManager->GetDevice());
		newRenderBatch.m_BBOXCenter = m_BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld.Identity();	
		newRenderBatch.m_pSB = NULL;
		newRenderBatch.m_pVC = this;
		newRenderBatch.BuildQueueValue();
		m_pVCManager->GetDevice()->getOpenGLMaterialRenderer()->PushOpaqueRenderBatch(newRenderBatch);
	}

	m_bCommited = true;
	return;
}

void COpenGLDynamicBuffer::Clear(void)
{
	m_MaterialSkin.bAlpha = false;
	m_MaterialSkin.nShaderType = SGPST_VERTEXCOLOR; 
	m_MaterialSkin.nPrimitiveType = SGPPT_TRIANGLES;
	m_MaterialSkin.nTextureNum = 0;
	memset( m_MaterialSkin.nTextureID, 0, sizeof(uint32)*SGP_MATERIAL_MAX_TEXTURES );
	m_MaterialSkin.vUVSpeed[0] = m_MaterialSkin.vUVSpeed[1] = 0;
	m_MaterialSkin.vUVTile[0] = m_MaterialSkin.vUVTile[1] = 1;
	m_MaterialSkin.vUVTile[2] = m_MaterialSkin.vUVTile[3] = 0;
	m_MaterialSkin.vUVTile[4] = 60;

	m_MaterialSkin.nNumMatKeyFrame = 0;
	m_MaterialSkin.pMatKeyFrame = NULL;

	m_nNumVerts = 0;
	m_nNumIndis = 0;
	memset( &m_BoundingBox, 0, sizeof(AABBox) );
	m_bCommited = false;
}

