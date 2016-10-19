

COpenGLTerrainRenderer::COpenGLTerrainRenderer(COpenGLRenderDevice *pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_nChunkIndexVBO(0),
	m_nVeryDetailedChunkNumber(0), m_nLOD0ChunkNumber(0), m_nLOD1ChunkNumber(0),
	m_nLODBlendChunkNumber(0), m_nTerrainSize(1),
	m_TerrainChunkLightMapTexID(2),		// Default Black texture
	m_TerrainChunkAlphaBlendMapPBOID(0), m_TerrainChunkColorMiniMapPBOID(0)
{
	m_TerrainChunkRenderArray.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE);

	m_VeryDetailedChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);
	m_LOD0ChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);
	m_LOD1ChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);
	m_LODBlendChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);

	// Create chunk Index VBO (static)
	m_pRenderDevice->extGlGenBuffers(1, &m_nChunkIndexVBO);
	m_pRenderDevice->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nChunkIndexVBO);
	m_pRenderDevice->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk_index_count*sizeof(uint16), chunk_index_tile, GL_STATIC_DRAW);
	m_pRenderDevice->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

COpenGLTerrainRenderer::~COpenGLTerrainRenderer()
{
	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		releaseChunkVBO(i);
	}

	m_TerrainChunkRenderArray.clear(true);

	// Delete chunk Index VBO (static)
	if( m_nChunkIndexVBO != 0 )
		m_pRenderDevice->extGlDeleteBuffers(1, &m_nChunkIndexVBO);
}

void COpenGLTerrainRenderer::setTerrainSize(uint32 terrainsize)
{
	m_nTerrainSize = terrainsize;

	if( m_TerrainChunkRenderArray.size() > 0 )
		m_TerrainChunkRenderArray.clear(true);

	for( uint32 i=0; i<terrainsize*terrainsize; i++ )
		m_TerrainChunkRenderArray.add( NULL );
}


void COpenGLTerrainRenderer::createChunkVBO(uint32 chunkindex)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
		releaseChunkVBO(chunkindex);

	const CSGPTerrainChunk* pTerrainChunk = m_pRenderDevice->GetWorldSystemManager()->getTerrain()->m_TerrainChunks[chunkindex];


	GLsizei nStride = sizeof(SGPVertex_TERRAIN);


	OpenGLChunkRenderInfo* pChunkRenderInfo = new OpenGLChunkRenderInfo();
	memset( pChunkRenderInfo, 0, sizeof(OpenGLChunkRenderInfo) );

	// create VAO and VBO
	m_pRenderDevice->extGlGenVertexArray(1, &pChunkRenderInfo->nVAOID);
	m_pRenderDevice->extGlBindVertexArray(pChunkRenderInfo->nVAOID);

	m_pRenderDevice->extGlGenBuffers(1, &pChunkRenderInfo->nVBOID);
	m_pRenderDevice->extGlBindBuffer(GL_ARRAY_BUFFER, pChunkRenderInfo->nVBOID);
	m_pRenderDevice->extGlBufferData(GL_ARRAY_BUFFER, pTerrainChunk->GetVertexCount()*nStride, pTerrainChunk->m_ChunkTerrainVertex, GL_STATIC_DRAW);

	m_pRenderDevice->extGlEnableVertexAttribArray(0);
	m_pRenderDevice->extGlVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
	m_pRenderDevice->extGlEnableVertexAttribArray(1);
	m_pRenderDevice->extGlVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
	m_pRenderDevice->extGlEnableVertexAttribArray(2);	
	m_pRenderDevice->extGlVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
	m_pRenderDevice->extGlEnableVertexAttribArray(3);	
	m_pRenderDevice->extGlVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
	m_pRenderDevice->extGlEnableVertexAttribArray(4);
	m_pRenderDevice->extGlVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(11*sizeof(float)));
	m_pRenderDevice->extGlEnableVertexAttribArray(5);
	m_pRenderDevice->extGlVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(14*sizeof(float)));

	m_pRenderDevice->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nChunkIndexVBO);
	m_pRenderDevice->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk_index_count*sizeof(uint16), chunk_index_tile, GL_STATIC_DRAW);
	
	m_pRenderDevice->extGlBindVertexArray(0);


	m_TerrainChunkRenderArray.set(chunkindex, pChunkRenderInfo, false);

	createChunkLODInfo(chunkindex);
}

void COpenGLTerrainRenderer::flushChunkVBO(uint32 chunkindex)
{
	const CSGPTerrainChunk* pTerrainChunk = m_pRenderDevice->GetWorldSystemManager()->getTerrain()->m_TerrainChunks[chunkindex];
	GLsizei nStride = sizeof(SGPVertex_TERRAIN);

	m_pRenderDevice->extGlBindBuffer(GL_ARRAY_BUFFER, m_TerrainChunkRenderArray[chunkindex]->nVBOID);
	SGPTerrainVertex* pData = (SGPTerrainVertex*)m_pRenderDevice->extGlMapBufferRange(GL_ARRAY_BUFFER, 0, pTerrainChunk->GetVertexCount()*nStride, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	// copy data into the buffer
	memcpy(pData, pTerrainChunk->m_ChunkTerrainVertex, pTerrainChunk->GetVertexCount()*nStride);
	m_pRenderDevice->extGlUnmapBuffer(GL_ARRAY_BUFFER);
}

void COpenGLTerrainRenderer::releaseChunkVBO(uint32 chunkindex)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		// Delete VAO and VBO
		m_pRenderDevice->extGlDeleteBuffers(1, &m_TerrainChunkRenderArray[chunkindex]->nVBOID);
		m_pRenderDevice->extGlDeleteVertexArray(1, &m_TerrainChunkRenderArray[chunkindex]->nVAOID);


		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse0Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse0Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse1Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse1Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse2Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse2Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse3Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse3Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_NormalMapTexture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_NormalMapTexture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_DetailMapTexture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_DetailMapTexture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_SlopeMapTexture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_SlopeMapTexture]);


		m_TerrainChunkRenderArray.set(chunkindex, NULL, true);
	}
}

void COpenGLTerrainRenderer::createChunkTextureFromWorldMap(const CSGPWorldMap* pWorldMap, uint32 chunkindex)
{
	OpenGLChunkRenderInfo* pChunkRenderInfo = m_TerrainChunkRenderArray[chunkindex];

	jassert( pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse0Texture] != -1 );
	

	pChunkRenderInfo->bUseDetailMap = (pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_DetailMapTexture] != -1); 
	pChunkRenderInfo->bUseSlopeMap = (pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_SlopeMapTexture] != -1); 
	pChunkRenderInfo->bUseTriplanarTex = true; 
	pChunkRenderInfo->bUseNormalMap = (pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_NormalMapTexture] != -1); 
	

	setChunkTextures(	chunkindex, eChunk_Diffuse0Texture, 
						String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse0Texture] ].m_ChunkTextureFileName) );
	
	if( pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse1Texture] != -1 )
		setChunkTextures(	chunkindex, eChunk_Diffuse1Texture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse1Texture] ].m_ChunkTextureFileName) );
	if( pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse2Texture] != -1 )
		setChunkTextures(	chunkindex, eChunk_Diffuse2Texture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse2Texture] ].m_ChunkTextureFileName) );
	if( pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse3Texture] != -1 )
		setChunkTextures(	chunkindex, eChunk_Diffuse3Texture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse3Texture] ].m_ChunkTextureFileName) );
	
	setChunkTextures(	chunkindex, eChunk_AlphaTexture, 0);

	if( pChunkRenderInfo->bUseNormalMap )
		setChunkTextures(	chunkindex, eChunk_NormalMapTexture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_NormalMapTexture] ].m_ChunkTextureFileName) );
	if( pChunkRenderInfo->bUseDetailMap )
		setChunkTextures(	chunkindex, eChunk_DetailMapTexture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_DetailMapTexture] ].m_ChunkTextureFileName) );
	if( pChunkRenderInfo->bUseSlopeMap )
		setChunkTextures(	chunkindex, eChunk_SlopeMapTexture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_SlopeMapTexture] ].m_ChunkTextureFileName) );
	
	setChunkTextures(	chunkindex, eChunk_MiniColorMapTexture, 0);
}

void COpenGLTerrainRenderer::updateChunkLODInfo(uint32 chunkindex, const Vector4D &ChunkCenter)
{
	Vector4D CamPos;
	m_pRenderDevice->getCamreaPosition( &CamPos );
	float fCameraDistance = (Vector2D(ChunkCenter.x, ChunkCenter.z) - Vector2D(CamPos.x, CamPos.z)).GetLength();

	float fDistance_X = std::fabs( ChunkCenter.x - CamPos.x );
	float fDistance_Z = std::fabs( ChunkCenter.z - CamPos.z );

	//float fSignDistance_X = ChunkCenter.x - CamPos.x;
	//float fSignDistance_Z = ChunkCenter.z - CamPos.z;



	if( fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eVeryHighDetailed+0.5f) &&
		fDistance_Z <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eVeryHighDetailed+0.5f) )
	{
		// LOD 0 (Very High Detailed)
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eVeryHighDetailed;
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(0,0,0);
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;
	}
	else if(/* fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) &&
			 fDistance_Z <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f)*/ 
			 fCameraDistance <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1.0f) )
	{
		// LOD 0
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eHighDetailed;
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(0,0,0);
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;
	}
	else if( fDistance_X > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eLowDetailed+0.5f) ||
			 fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eLowDetailed+0.5f) )
	{
		// LOD 1
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD1;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(0,0,0);
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1.indexcount;
	}
	else
	{
		// LOD Blend
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0TOLOD1;		
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(CamPos.x, CamPos.z, eHighDetailed*((float)SGPTT_TILENUM*SGPTT_TILE_METER), ((float)SGPTT_TILENUM*SGPTT_TILE_METER));
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;

	}

#if 0
	else if( fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed+0.5f) &&
			 fDistance_Z <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed+0.5f) )
	{
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;


		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD1 )
		{
			m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD1TOLOD0;
			m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance = fCameraDistance;
		}
		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD1TOLOD0 )
		{
			m_TerrainChunkRenderArray[chunkindex]->fChunkHeightBlend = (m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance - fCameraDistance) / (SGPTT_TILENUM*SGPTT_TILE_METER / 2.0f);
			if( m_TerrainChunkRenderArray[chunkindex]->fChunkHeightBlend > 1.0f )
				m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
		}
		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD0TOLOD1 )
		{
			m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
			m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance = 0;
		}




		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD0 )
		{
			
			EOpenGLChunkPosition Pos = getChunkPosition(fSignDistance_X, fSignDistance_Z);
			
			switch( Pos )
			{			
			case eChunk_UP:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_up.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_up.indexcount;
				break;
			case eChunk_BOTTOM:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_bottom.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_bottom.indexcount;
				break;
			case eChunk_LEFT:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_left.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_left.indexcount;
				break;
			case eChunk_RIGHT:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_right.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_right.indexcount;
				break;
			case eChunk_LEFTUP:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_leftup.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_leftup.indexcount;
				break;
			case eChunk_RIGHTUP:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_rightup.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_rightup.indexcount;
				break;
			case eChunk_LEFTBOTTOM:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_leftbottom.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_leftbottom.indexcount;
				break;
			case eChunk_RIGHTBOTTOM:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_rightbottom.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_rightbottom.indexcount;
				break;
			default:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;
				break;
			}

		}
		else if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD1TOLOD0 )
		{
			EOpenGLChunkPosition Pos = getChunkPosition(fSignDistance_X, fSignDistance_Z);

			switch( Pos )
			{			
			case eChunk_UP:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_up.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_up.indexcount;
				break;
			case eChunk_BOTTOM:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_bottom.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_bottom.indexcount;
				break;
			case eChunk_LEFT:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_left.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_left.indexcount;
				break;
			case eChunk_RIGHT:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_right.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_right.indexcount;
				break;
			case eChunk_LEFTUP:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_leftup.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_leftup.indexcount;
				break;				
			case eChunk_RIGHTUP:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_rightup.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_rightup.indexcount;
				break;
			case eChunk_LEFTBOTTOM:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_leftbottom.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_leftbottom.indexcount;
				break;
			case eChunk_RIGHTBOTTOM:
				m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1_to_0_rightbottom.indexoffset;
				m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1_to_0_rightbottom.indexcount;
				break;
			default:
				jassertfalse;
				break;
			}
		}
		else
			jassertfalse;
	}
	else 
	{
		// LOD 1 (nearby LOD chunk bondery)
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;

		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD0 )
		{
			m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0TOLOD1;
			m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance = fCameraDistance;
		}
		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD0TOLOD1 )
		{
			m_TerrainChunkRenderArray[chunkindex]->fChunkHeightBlend = (m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance - fCameraDistance) / (SGPTT_TILENUM*SGPTT_TILE_METER / 2.0f);
			if( m_TerrainChunkRenderArray[chunkindex]->fChunkHeightBlend < -1.0f )
				m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD1;
		}
		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD1TOLOD0 )
		{
			m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD1;
			m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance = 0;
		}


		if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD1 )
		{
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1.indexcount;
		}
		else if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD0TOLOD1 )
		{
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_side.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_side.indexcount;
		}
	}
#endif
}



void COpenGLTerrainRenderer::createChunkLODInfo(uint32 chunkindex)
{
	Vector4D CamPos;
	m_pRenderDevice->getCamreaPosition( &CamPos );

	Vector4D ChunkCenter = m_pRenderDevice->GetWorldSystemManager()->getTerrain()->m_TerrainChunks[chunkindex]->GetChunkCenter();
	float fCameraDistance = (Vector2D(ChunkCenter.x, ChunkCenter.z) - Vector2D(CamPos.x, CamPos.z)).GetLength();

	float fDistance_X = std::fabs( ChunkCenter.x - CamPos.x );
	float fDistance_Z = std::fabs( ChunkCenter.z - CamPos.z );

	//float fSignDistance_X = ChunkCenter.x - CamPos.x;
	//float fSignDistance_Z = ChunkCenter.z - CamPos.z;


	if( fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eVeryHighDetailed+0.5f) &&
		fDistance_Z <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eVeryHighDetailed+0.5f) )
	{	
		// level 0
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eVeryHighDetailed;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(0,0,0);
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;
	}
	else if( /*fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) &&
			 fDistance_Z <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f)*/
			 fCameraDistance <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1.0f) )
	{
		// level 0
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eHighDetailed;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(0,0,0);
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;

	}
	else if( fDistance_X > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eLowDetailed+0.5f) ||
			 fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eLowDetailed+0.5f) )
	{
		// Level 1
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD1;
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(0,0,0);
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1.indexcount;

	}
	else
	{
		// LOD Blend
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0TOLOD1;
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;
		m_TerrainChunkRenderArray[chunkindex]->vCamPosWithBlendWidth.Set(CamPos.x, CamPos.z, eHighDetailed*((float)SGPTT_TILENUM*SGPTT_TILE_METER), ((float)SGPTT_TILENUM*SGPTT_TILE_METER));
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0.indexcount;

	}

#if 0
	else if( fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed+0.5f) &&
			 fDistance_Z <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed+0.5f) )
	{
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD0;
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;
		m_TerrainChunkRenderArray[chunkindex]->fChunkHeightBlend = 0;
		m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance = 0;



		EOpenGLChunkPosition Pos = getChunkPosition(fSignDistance_X, fSignDistance_Z);
		switch( Pos )
		{			
		case eChunk_UP:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_up.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_up.indexcount;
			break;
		case eChunk_BOTTOM:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_bottom.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_bottom.indexcount;
			break;
		case eChunk_LEFT:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_left.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_left.indexcount;
			break;
		case eChunk_RIGHT:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_right.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_right.indexcount;
			break;
		case eChunk_LEFTUP:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_leftup.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_leftup.indexcount;
			break;				
		case eChunk_RIGHTUP:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_rightup.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_rightup.indexcount;
			break;
		case eChunk_LEFTBOTTOM:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_leftbottom.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_leftbottom.indexcount;
			break;
		case eChunk_RIGHTBOTTOM:
			m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_0_rightbottom.indexoffset;
			m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_0_rightbottom.indexcount;
			break;
		default:
			jassertfalse;
			break;
		}
	}
	else
	{
		// Level 1
		m_TerrainChunkRenderArray[chunkindex]->nLODLevel = eChunk_LOD1;
		m_TerrainChunkRenderArray[chunkindex]->nRenderFlag = eLowDetailed;
		m_TerrainChunkRenderArray[chunkindex]->fChunkHeightBlend = 0;
		m_TerrainChunkRenderArray[chunkindex]->fFirstInDistance = 0;
		m_TerrainChunkRenderArray[chunkindex]->nIndexOffset = chunk_base_level_1.indexoffset;
		m_TerrainChunkRenderArray[chunkindex]->nIndexCount = chunk_base_level_1.indexcount;
	}
#endif
}

/*
COpenGLTerrainRenderer::EOpenGLChunkPosition COpenGLTerrainRenderer::getChunkPosition(float fDistance_X, float fDistance_Z )
{
	float fDistance_XABS = fabs(fDistance_X);
	float fDistance_ZABS = fabs(fDistance_Z);

	if( fDistance_XABS <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed+0.5f) &&
		fDistance_ZABS <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed+0.5f) )
	{
		if( fDistance_X > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
		{
			if( fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
				return eChunk_RIGHTUP;		// RIGHT - UP
			else if( -fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
				return eChunk_RIGHTBOTTOM;	// RIGHT - BOTTOM
			else
				return eChunk_RIGHT;		// RIGHT
		}
		else if( -fDistance_X > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
		{
			if( fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
				return eChunk_LEFTUP;		// LEFT - UP
			else if( -fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
				return eChunk_LEFTBOTTOM;	// LEFT - BOTTOM
			else
				return eChunk_LEFT;			// LEFT
		}
		else
		{
			if( fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
				return eChunk_UP;			// UP
			else if( -fDistance_Z > ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) )
				return eChunk_BOTTOM;		// BOTTOM
			else
			{
				jassertfalse;
				return eChunk_CENTER;		// CENTER
			}
		}
	}

	return eChunk_CENTER; 
}
*/

void COpenGLTerrainRenderer::generateChunkColorMinimapData(uint32* pMinimapData, uint32 terrainsize, uint32 chunkRow, uint32 chunkCol)
{
	ISGPTexture *pDiffuse1Texture = NULL;
	ISGPTexture *pDiffuse2Texture = NULL;
	ISGPTexture *pDiffuse3Texture = NULL;

	float channel_r, channel_g, channel_b, red, green, blue;
	float red1, green1, blue1, red2, green2, blue2, red3, green3, blue3;
	uint32  Diffuse1Width=0, Diffuse1Height=0, Diffuse2Width=0, Diffuse2Height=0, Diffuse3Width=0, Diffuse3Height = 0;
	
	uint32* pDiffuse1Pixels = NULL;
	uint32* pDiffuse2Pixels = NULL;
	uint32* pDiffuse3Pixels = NULL;

	uint32  pixelColor;

	uint32	startpos = SGPTT_TILENUM*SGPTT_TILENUM*terrainsize*chunkRow + SGPTT_TILENUM*chunkCol;
	uint32	offset = 0;

	const OpenGLChunkRenderInfo* pChunkInfo = m_TerrainChunkRenderArray[chunkRow*terrainsize + chunkCol];

	ISGPTexture* pAlphaBlendTexture = m_pRenderDevice->GetTextureManager()->getTextureByID(pChunkInfo->ChunkTextureID[eChunk_AlphaTexture])->pSGPTexture;
	uint32 AlphaWidth = pAlphaBlendTexture->getSize().Width;
	uint32 AlphaHeight = pAlphaBlendTexture->getSize().Height;				
	uint32*	pAlphaPixels = new (std::nothrow) uint32[AlphaWidth * AlphaHeight];
	pAlphaBlendTexture->getMipmapData(pAlphaPixels, 0, SGPTT_TEXTURE_2D);

	ISGPTexture *pDiffuse0Texture = m_pRenderDevice->GetTextureManager()->getTextureByID(pChunkInfo->ChunkTextureID[eChunk_Diffuse0Texture])->pSGPTexture;
	uint32	Diffuse0Width = pDiffuse0Texture->getSize().Width;
	uint32	Diffuse0Height = pDiffuse0Texture->getSize().Height;
	uint32*	pDiffuse0Pixels = new (std::nothrow) uint32 [Diffuse0Width * Diffuse0Height];
	pDiffuse0Texture->getMipmapData(pDiffuse0Pixels, 0, SGPTT_TEXTURE_2D);

	if( pChunkInfo->ChunkTextureID[1] != 0 )
	{
		pDiffuse1Texture = m_pRenderDevice->GetTextureManager()->getTextureByID(pChunkInfo->ChunkTextureID[eChunk_Diffuse1Texture])->pSGPTexture;
		Diffuse1Width = pDiffuse1Texture->getSize().Width;
		Diffuse1Height = pDiffuse1Texture->getSize().Height;
		pDiffuse1Pixels = new uint32[Diffuse1Width * Diffuse1Height];
		pDiffuse1Texture->getMipmapData(pDiffuse1Pixels, 0, SGPTT_TEXTURE_2D);				
	}

	if( pChunkInfo->ChunkTextureID[2] != 0 )
	{
		pDiffuse2Texture = m_pRenderDevice->GetTextureManager()->getTextureByID(pChunkInfo->ChunkTextureID[eChunk_Diffuse2Texture])->pSGPTexture;
		Diffuse2Width = pDiffuse2Texture->getSize().Width;
		Diffuse2Height = pDiffuse2Texture->getSize().Height;
		pDiffuse2Pixels = new uint32[Diffuse2Width * Diffuse2Height];
		pDiffuse2Texture->getMipmapData(pDiffuse2Pixels, 0, SGPTT_TEXTURE_2D);				
	}

	if( pChunkInfo->ChunkTextureID[3] != 0 )
	{
		pDiffuse3Texture = m_pRenderDevice->GetTextureManager()->getTextureByID(pChunkInfo->ChunkTextureID[eChunk_Diffuse3Texture])->pSGPTexture;
		Diffuse3Width = pDiffuse3Texture->getSize().Width;
		Diffuse3Height = pDiffuse3Texture->getSize().Height;
		pDiffuse3Pixels = new uint32[Diffuse3Width * Diffuse3Height];
		pDiffuse3Texture->getMipmapData(pDiffuse3Pixels, 0, SGPTT_TEXTURE_2D);				
	}

	for( uint32 j=0; j<SGPTT_TILENUM; j++ )
	{
		for( uint32 i=0; i<SGPTT_TILENUM; i++ )
		{
			red = 0;
			green = 0;
			blue = 0;
			uint32 Diffuse0UV = uint32((float)(j)/SGPTT_TILENUM*Diffuse0Height*Diffuse0Width + (float)(i)/SGPTT_TILENUM*Diffuse0Width);
			for( uint32 kk = 0; kk<Diffuse0Height/SGPTT_TILENUM; kk++ )
				for( uint32 mm = 0; mm<Diffuse0Width/SGPTT_TILENUM; mm++ )
				{
					pixelColor = pDiffuse0Pixels[Diffuse0UV+Diffuse0Width*kk+mm];
					red += ((pixelColor & 0x00FF0000)>>16) / 255.0f;
					green += ((pixelColor & 0x0000FF00)>>8) / 255.0f;
					blue += (pixelColor & 0x000000FF) / 255.0f;
				}
			red /= (float)(Diffuse0Width/SGPTT_TILENUM * Diffuse0Height/SGPTT_TILENUM);
			green /= (float)(Diffuse0Width/SGPTT_TILENUM * Diffuse0Height/SGPTT_TILENUM);
			blue /= (float)(Diffuse0Width/SGPTT_TILENUM * Diffuse0Height/SGPTT_TILENUM);


			channel_r = 0;
			channel_g = 0;
			channel_b = 0;
			uint32 AlphaColorUV = uint32((float)(j+chunkRow*SGPTT_TILENUM)/(SGPTT_TILENUM*terrainsize)*AlphaHeight*AlphaWidth + (float)(i+chunkCol*SGPTT_TILENUM)/(SGPTT_TILENUM*terrainsize)*AlphaWidth);
			for( uint32 kk = 0; kk<AlphaHeight/(SGPTT_TILENUM*terrainsize); kk++ )
				for( uint32 mm = 0; mm<AlphaWidth/(SGPTT_TILENUM*terrainsize); mm++ )
				{	
					uint32 AlphaColor = pAlphaPixels[AlphaColorUV+AlphaWidth*kk+mm];
					channel_r += ((AlphaColor & 0x00FF0000) >> 16)/ 255.0f;
					channel_g += ((AlphaColor & 0x0000FF00) >> 8) / 255.0f;
					channel_b += (AlphaColor & 0x000000FF) / 255.0f;
				}
			channel_r /= (float)(AlphaHeight/(SGPTT_TILENUM*terrainsize) * AlphaWidth/(SGPTT_TILENUM*terrainsize));
			channel_g /= (float)(AlphaHeight/(SGPTT_TILENUM*terrainsize) * AlphaWidth/(SGPTT_TILENUM*terrainsize));
			channel_b /= (float)(AlphaHeight/(SGPTT_TILENUM*terrainsize) * AlphaWidth/(SGPTT_TILENUM*terrainsize));


			if( pChunkInfo->ChunkTextureID[1] != 0 && FloatCmp(channel_r, 0) != 0 )
			{
				red1 = 0;
				green1 = 0;
				blue1 = 0;
				uint32 Diffuse1UV = uint32((float)j/SGPTT_TILENUM*Diffuse1Height*Diffuse1Width + (float)i/SGPTT_TILENUM*Diffuse1Width);
				for( uint32 kk = 0; kk<Diffuse1Height/SGPTT_TILENUM; kk++ )
					for( uint32 mm = 0; mm<Diffuse1Width/SGPTT_TILENUM; mm++ )
					{
						uint32 Diffuse1Color = pDiffuse1Pixels[Diffuse1UV+Diffuse1Width*kk+mm];
						red1 += ((Diffuse1Color & 0x00FF0000)>>16) / 255.0f;
						green1 += ((Diffuse1Color & 0x0000FF00)>>8) / 255.0f;
						blue1 += (Diffuse1Color & 0x000000FF) / 255.0f;
					}
				red1 /= (float)(Diffuse1Width/SGPTT_TILENUM * Diffuse1Height/SGPTT_TILENUM);
				green1 /= (float)(Diffuse1Width/SGPTT_TILENUM * Diffuse1Height/SGPTT_TILENUM);
				blue1 /= (float)(Diffuse1Width/SGPTT_TILENUM * Diffuse1Height/SGPTT_TILENUM);

				red = red * (1.0f-channel_r) + red1 * channel_r;
				green = green * (1.0f-channel_r) + green1 * channel_r;
				blue = blue * (1.0f-channel_r) + blue1 * channel_r;
			}
			if( pChunkInfo->ChunkTextureID[2] != 0 && FloatCmp(channel_g, 0) != 0 )
			{
				red2 = 0;
				green2 = 0;
				blue2 = 0;
				uint32 Diffuse2UV = uint32((float)j/SGPTT_TILENUM*Diffuse2Height*Diffuse2Width + (float)i/SGPTT_TILENUM*Diffuse2Width);
				for( uint32 kk = 0; kk<Diffuse2Height/SGPTT_TILENUM; kk++ )
					for( uint32 mm = 0; mm<Diffuse2Width/SGPTT_TILENUM; mm++ )
					{	
						uint32 Diffuse2Color = pDiffuse2Pixels[Diffuse2UV+Diffuse2Width*kk+mm];
						red2 += ((Diffuse2Color & 0x00FF0000)>>16) / 255.0f;
						green2 += ((Diffuse2Color & 0x0000FF00)>>8) / 255.0f;
						blue2 += (Diffuse2Color & 0x000000FF) / 255.0f;
					}
				red2 /= (float)(Diffuse2Width/SGPTT_TILENUM * Diffuse2Height/SGPTT_TILENUM);
				green2 /= (float)(Diffuse2Width/SGPTT_TILENUM * Diffuse2Height/SGPTT_TILENUM);
				blue2 /= (float)(Diffuse2Width/SGPTT_TILENUM * Diffuse2Height/SGPTT_TILENUM);

				red = red * (1.0f-channel_g) + red2 * channel_g;
				green = green * (1.0f-channel_g) + green2 * channel_g;
				blue = blue * (1.0f-channel_g) + blue2 * channel_g;
			}
			if( pChunkInfo->ChunkTextureID[3] != 0 && FloatCmp(channel_b, 0) != 0 )
			{
				red3 = 0;
				green3 = 0;
				blue3 = 0;
				uint32 Diffuse3UV = uint32((float)j/SGPTT_TILENUM*Diffuse3Height*Diffuse3Width + (float)i/SGPTT_TILENUM*Diffuse3Width);
				for( uint32 kk = 0; kk<Diffuse3Height/SGPTT_TILENUM; kk++ )
					for( uint32 mm = 0; mm<Diffuse3Width/SGPTT_TILENUM; mm++ )
					{	
						uint32 Diffuse3Color = pDiffuse3Pixels[Diffuse3UV+Diffuse3Width*kk+mm];
						red3 += ((Diffuse3Color & 0x00FF0000)>>16) / 255.0f;
						green3 += ((Diffuse3Color & 0x0000FF00)>>8) / 255.0f;
						blue3 += (Diffuse3Color & 0x000000FF) / 255.0f;
					}							
				red3 /= (float)(Diffuse3Width/SGPTT_TILENUM * Diffuse3Height/SGPTT_TILENUM);
				green3 /= (float)(Diffuse3Width/SGPTT_TILENUM * Diffuse3Height/SGPTT_TILENUM);
				blue3 /= (float)(Diffuse3Width/SGPTT_TILENUM * Diffuse3Height/SGPTT_TILENUM);
							
				red = red * (1.0f-channel_b) + red3 * channel_b;
				green = green * (1.0f-channel_b) + green3 * channel_b;
				blue = blue * (1.0f-channel_b) + blue3 * channel_b;
			}

			pixelColor = 0xFF000000 | (uint32(red*255) << 16) | (uint32(green*255) << 8) | uint32(blue*255);

			pMinimapData[startpos + offset + i] = pixelColor;						
		}
		offset += SGPTT_TILENUM * terrainsize;
	}

	if( pAlphaPixels )
		delete [] pAlphaPixels;
	if( pDiffuse0Pixels )
		delete [] pDiffuse0Pixels;
	if( pDiffuse1Pixels )
		delete [] pDiffuse1Pixels;
	if( pDiffuse2Pixels ) 
		delete [] pDiffuse2Pixels;
	if( pDiffuse3Pixels )
		delete [] pDiffuse3Pixels;
}


void COpenGLTerrainRenderer::setChunkTextures(uint32 chunkindex, uint8 textureslot, const String& texname)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		uint32 texID = m_pRenderDevice->GetTextureManager()->registerTexture(texname);
		m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[textureslot] = texID;
	}
}

void COpenGLTerrainRenderer::setChunkTextures(uint32 chunkindex, uint8 textureslot, uint32 SGPtextureID)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[textureslot] = SGPtextureID;
	}
}

void COpenGLTerrainRenderer::renderTerrainChunk(uint32 chunkindex)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		if( m_TerrainChunkRenderArray[chunkindex]->nRenderFlag == eVeryHighDetailed )
			m_VeryDetailedChunkArrayID.add(chunkindex);
		else if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD0 )
			m_LOD0ChunkArrayID.add(chunkindex);
		else if( m_TerrainChunkRenderArray[chunkindex]->nLODLevel == eChunk_LOD1 )
			m_LOD1ChunkArrayID.add(chunkindex);
		else
		{
			m_LODBlendChunkArrayID.add(chunkindex);
		}
	}
}

void COpenGLTerrainRenderer::DoDrawTerrainRenderBatch()
{
	COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &TerrainMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_terrain);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( TerrainMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

	// Very High Detailed Terrain Chunks
	if( m_VeryDetailedChunkArrayID.size() > 0 )
	{
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->useProgram();

		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("worldViewProjMatrix", MVP);	
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("fFarPlane", m_pRenderDevice->getOpenGLCamera()->m_fFar);


		m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkLightMapTexID)->pSGPTexture->BindTexture2D(8);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerLightmap", 8);

		if( m_TerrainChunkRenderArray[m_VeryDetailedChunkArrayID[0]]->ChunkTextureID[eChunk_AlphaTexture] != 0 )
		{
			m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[m_VeryDetailedChunkArrayID[0]]->ChunkTextureID[eChunk_AlphaTexture])->pSGPTexture->BindTexture2D(4);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerAlphaBlend", 4);
		}

		uint32* pEnd = m_VeryDetailedChunkArrayID.end();
		for( uint32* pBegin = m_VeryDetailedChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
		{
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse0Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse0Texture])->pSGPTexture->BindTexture2D(0);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerDiffuse0", 0);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse1Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse1Texture])->pSGPTexture->BindTexture2D(1);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerDiffuse1", 1);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse2Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse2Texture])->pSGPTexture->BindTexture2D(2);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerDiffuse2", 2);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse3Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse3Texture])->pSGPTexture->BindTexture2D(3);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerDiffuse3", 3);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_NormalMapTexture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_NormalMapTexture])->pSGPTexture->BindTexture2D(5);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerNormalmap", 5);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_DetailMapTexture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_DetailMapTexture])->pSGPTexture->BindTexture2D(6);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerDetail", 6);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture])->pSGPTexture->BindTexture2D(7);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerSlope", 7);
			}


			int32 RenderFlags = (m_TerrainChunkRenderArray[*pBegin]->bUseDetailMap ? 1 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->bUseSlopeMap ? 2 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->bUseTriplanarTex ? 4 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->bUseNormalMap ? 8 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[1] != 0 ? 16 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[2] != 0 ? 32 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[3] != 0 ? 64 : 0);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("RenderFlag", RenderFlags);

			m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);

			glDrawElements( GL_TRIANGLES,
							m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
							GL_UNSIGNED_SHORT,
							(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );

		}
		m_pRenderDevice->extGlBindVertexArray(0);
	}


	// High Detailed Terrain Chunks (LOD0)
	if( m_LOD0ChunkArrayID.size() > 0 )
	{
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->useProgram();

		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("worldViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("fFarPlane", m_pRenderDevice->getOpenGLCamera()->m_fFar);


		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());

		m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkLightMapTexID)->pSGPTexture->BindTexture2D(6);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerLightmap", 6);

		if( m_TerrainChunkRenderArray[m_LOD0ChunkArrayID[0]]->ChunkTextureID[eChunk_AlphaTexture] != 0 )
		{
			m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[m_LOD0ChunkArrayID[0]]->ChunkTextureID[eChunk_AlphaTexture])->pSGPTexture->BindTexture2D(4);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerAlphaBlend", 4);
		}

		uint32* pEnd = m_LOD0ChunkArrayID.end();
		for( uint32* pBegin = m_LOD0ChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
		{
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse0Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse0Texture])->pSGPTexture->BindTexture2D(0);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerDiffuse0", 0);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse1Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse1Texture])->pSGPTexture->BindTexture2D(1);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerDiffuse1", 1);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse2Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse2Texture])->pSGPTexture->BindTexture2D(2);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerDiffuse2", 2);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse3Texture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_Diffuse3Texture])->pSGPTexture->BindTexture2D(3);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerDiffuse3", 3);
			}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture])->pSGPTexture->BindTexture2D(5);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerSlope", 5);
			}

			int32 RenderFlags = (m_TerrainChunkRenderArray[*pBegin]->bUseSlopeMap ? 2 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->bUseTriplanarTex ? 4 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[1] != 0 ? 16 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[2] != 0 ? 32 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[3] != 0 ? 64 : 0);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("RenderFlag", RenderFlags);

			m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);

			glDrawElements( GL_TRIANGLES,
							m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
							GL_UNSIGNED_SHORT,
							(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );

		}
		m_pRenderDevice->extGlBindVertexArray(0);
	}

	// Low Detailed Terrain Chunks (LOD Tile blending)
	if( m_LODBlendChunkArrayID.size() > 0 )
	{
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->useProgram();

		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("worldViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("fFarPlane", m_pRenderDevice->getOpenGLCamera()->m_fFar);	

		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());

		m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkLightMapTexID)->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("gSamplerLightmap", 1);

		if( m_TerrainChunkRenderArray[m_LODBlendChunkArrayID[0]]->ChunkTextureID[eChunk_MiniColorMapTexture] != 0 )
		{				
			m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[m_LODBlendChunkArrayID[0]]->ChunkTextureID[eChunk_MiniColorMapTexture])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("gSamplerMiniMap", 0);
		}

		uint32* pEnd = m_LODBlendChunkArrayID.end();
		for( uint32* pBegin = m_LODBlendChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
		{
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LODBLEND)->setShaderUniform("cameraPosWithBlendWidth", m_TerrainChunkRenderArray[*pBegin]->vCamPosWithBlendWidth);
			
			m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);

			glDrawElements( GL_TRIANGLES,
							m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
							GL_UNSIGNED_SHORT,
							(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );

		}
		m_pRenderDevice->extGlBindVertexArray(0);
	}

	// Low Detailed Terrain Chunks (LOD1)
	if( m_LOD1ChunkArrayID.size() > 0 )
	{
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->useProgram();

		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("worldViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("fFarPlane", m_pRenderDevice->getOpenGLCamera()->m_fFar);	
		
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());
	
		m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkLightMapTexID)->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("gSamplerLightmap", 1);
		
		if( m_TerrainChunkRenderArray[m_LOD1ChunkArrayID[0]]->ChunkTextureID[eChunk_MiniColorMapTexture] != 0 )
		{				
			m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[m_LOD1ChunkArrayID[0]]->ChunkTextureID[eChunk_MiniColorMapTexture])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("gSamplerMiniMap", 0);
		}

		uint32* pEnd = m_LOD1ChunkArrayID.end();
		for( uint32* pBegin = m_LOD1ChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
		{
			m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);

			glDrawElements( GL_TRIANGLES,
							m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
							GL_UNSIGNED_SHORT,
							(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );
		}
		m_pRenderDevice->extGlBindVertexArray(0);
	}

	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();
}

void COpenGLTerrainRenderer::DoDrawReflectionTerrainRenderBatch()
{
	COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_pRenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_pRenderDevice->getOpenGLWaterRenderer()->m_ObliqueNearPlaneReflectionProjMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &TerrainMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_terrain);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( TerrainMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);



	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->useProgram();

	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("worldViewProjMatrix", MVP);
	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("fFarPlane", m_pRenderDevice->getOpenGLCamera()->m_fFar);	
		
	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());
		
	uint32 minimapID = m_TerrainChunkRenderArray[0]->ChunkTextureID[eChunk_MiniColorMapTexture];
	m_pRenderDevice->GetTextureManager()->getTextureByID(minimapID)->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("gSamplerMiniMap", 0);
	m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkLightMapTexID)->pSGPTexture->BindTexture2D(1);
	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("gSamplerLightmap", 1);


	uint32* pEnd = m_VeryDetailedChunkArrayID.end();	
	for( uint32* pBegin = m_VeryDetailedChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
	{
		m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);
		glDrawElements( GL_TRIANGLES,
						m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
						GL_UNSIGNED_SHORT,
						(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );
	}

	pEnd = m_LOD0ChunkArrayID.end();
	for( uint32* pBegin = m_LOD0ChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
	{
		m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);
		glDrawElements( GL_TRIANGLES,
						m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
						GL_UNSIGNED_SHORT,
						(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );
	}

	pEnd = m_LODBlendChunkArrayID.end();
	for( uint32* pBegin = m_LODBlendChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
	{
		m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);
		glDrawElements( GL_TRIANGLES,
						m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
						GL_UNSIGNED_SHORT,
						(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );
	}

	pEnd = m_LOD1ChunkArrayID.end();
	for( uint32* pBegin = m_LOD1ChunkArrayID.begin(); pBegin < pEnd; pBegin++ )
	{
		m_pRenderDevice->extGlBindVertexArray(m_TerrainChunkRenderArray[*pBegin]->nVAOID);

		glDrawElements( GL_TRIANGLES,
						m_TerrainChunkRenderArray[*pBegin]->nIndexCount, 
						GL_UNSIGNED_SHORT,
						(GLvoid*)(sizeof(GLushort)*m_TerrainChunkRenderArray[*pBegin]->nIndexOffset) );
	}
	m_pRenderDevice->extGlBindVertexArray(0);

	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();

}

void COpenGLTerrainRenderer::AfterDrawTerrainRenderBatch()
{
	m_nVeryDetailedChunkNumber = (uint32)m_VeryDetailedChunkArrayID.size();
	m_nLOD0ChunkNumber = (uint32)m_LOD0ChunkArrayID.size();
	m_nLOD1ChunkNumber = (uint32)m_LOD1ChunkArrayID.size();
	m_nLODBlendChunkNumber = (uint32)m_LODBlendChunkArrayID.size();

	m_VeryDetailedChunkArrayID.clearQuick();
	m_LOD0ChunkArrayID.clearQuick();
	m_LOD1ChunkArrayID.clearQuick();
	m_LODBlendChunkArrayID.clearQuick();
}

void COpenGLTerrainRenderer::registerColorMinimapTexture(const CSGPWorldMap* pWorldMap, const String& MinimapTexName, uint32 terrainsize)
{
	uint32* pMinimapData = pWorldMap->m_WorldChunkColorMiniMapTextureData;

	ISGPImage* pMiniMapImage = m_pRenderDevice->GetTextureManager()->createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(terrainsize*SGPTT_TILENUM, terrainsize*SGPTT_TILENUM),
		pMinimapData,
		false);

	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTextureFromImage(MinimapTexName, pMiniMapImage, false);
	jassert( TexID != 0 );

	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		setChunkTextures(i, eChunk_MiniColorMapTexture, TexID);
	}

	delete pMiniMapImage;
	pMiniMapImage = NULL;

	// create 1 pixel buffer objects for Terrain Chunk ColorMini map, you need to delete them when unregister.
	uint32 nPBOSize = terrainsize*SGPTT_TILENUM * terrainsize*SGPTT_TILENUM * sizeof(uint32);
	m_pRenderDevice->extGlGenBuffers(1, &m_TerrainChunkColorMiniMapPBOID);
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_TerrainChunkColorMiniMapPBOID);
	m_pRenderDevice->extGlBufferData(GL_PIXEL_UNPACK_BUFFER, nPBOSize, pMinimapData, GL_STREAM_DRAW);
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

}

void COpenGLTerrainRenderer::unRegisterColorMinimapTexture(const String& MinimapTexName)
{
	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		setChunkTextures(i, eChunk_MiniColorMapTexture, 0);
	}
	m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(MinimapTexName);

	m_pRenderDevice->extGlDeleteBuffers(1, &m_TerrainChunkColorMiniMapPBOID);
	m_TerrainChunkColorMiniMapPBOID = 0;
}

void COpenGLTerrainRenderer::registerBlendTexture(const CSGPWorldMap* pWorldMap, const String& BlendTexName, uint32 terrainsize)
{
	uint32* pBlendData = pWorldMap->m_WorldChunkAlphaTextureData;

	ISGPImage* pBlendMapImage = m_pRenderDevice->GetTextureManager()->createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(terrainsize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION, terrainsize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION),
		pBlendData,
		false);

	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTextureFromImage(BlendTexName, pBlendMapImage, false);
	jassert( TexID != 0 );

	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		setChunkTextures(i, eChunk_AlphaTexture, TexID);
	}

	delete pBlendMapImage;
	pBlendMapImage = NULL;


	// create 1 pixel buffer objects for Terrain Chunk AlphaBlend Map, you need to delete them when unregister.
	uint32 nPBOSize = terrainsize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION * terrainsize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION * sizeof(uint32);
	m_pRenderDevice->extGlGenBuffers(1, &m_TerrainChunkAlphaBlendMapPBOID);
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_TerrainChunkAlphaBlendMapPBOID);
	m_pRenderDevice->extGlBufferData(GL_PIXEL_UNPACK_BUFFER, nPBOSize, pBlendData, GL_STREAM_DRAW);
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

}

void COpenGLTerrainRenderer::unregisterBlendTexture(const String& BlendTexName)
{
	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		setChunkTextures(i, eChunk_AlphaTexture, 0);
	}
	m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(BlendTexName);


	m_pRenderDevice->extGlDeleteBuffers(1, &m_TerrainChunkAlphaBlendMapPBOID);
	m_TerrainChunkAlphaBlendMapPBOID = 0;

}

void COpenGLTerrainRenderer::registerLightmapTexture(const String& WorldMapName, const String& LightmapTexName)
{
	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTexture(
		String(L"Lightmap\\") + WorldMapName + File::separator + LightmapTexName,
		false);
	if( TexID == 0 )
		m_TerrainChunkLightMapTexID = 2;		// If no lightmap texture exist, using black texture by default
	else
		m_TerrainChunkLightMapTexID = TexID;
}

void COpenGLTerrainRenderer::unregisterLightmapTexture()
{
	if( (m_TerrainChunkLightMapTexID != 0) && (m_TerrainChunkLightMapTexID != 2) )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkLightMapTexID);
}

void COpenGLTerrainRenderer::updateLightmapTexture(uint32 TexWidth, uint32 TexHeight, uint32 *pLightMapData)
{
	static uint32 tempLightmapNum = 0;

	unregisterLightmapTexture();

	ISGPImage* pLightMapImage = m_pRenderDevice->GetTextureManager()->createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(TexWidth, TexHeight),
		pLightMapData,
		false);

	tempLightmapNum++;
	String LightmapTexName = String( L"TEMP_LightMap_" ) + String( tempLightmapNum );

	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTextureFromImage(LightmapTexName, pLightMapImage, false);
	jassert( TexID != 0 );

	m_TerrainChunkLightMapTexID = TexID;

	delete pLightMapImage;
	pLightMapImage = NULL;

}

void COpenGLTerrainRenderer::updateTerrainChunkLayerTexture(uint32 chunkindex, ESGPTerrainChunkTexture nLayer, const String& TextureName)
{
	if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[nLayer] != 0 )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[nLayer]);

	if( TextureName == String::empty )
		m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[nLayer] = 0;
	else
		setChunkTextures( chunkindex, (uint8)nLayer, TextureName );

	switch( nLayer )
	{
	case eChunk_NormalMapTexture:
		m_TerrainChunkRenderArray[chunkindex]->bUseNormalMap = (TextureName != String::empty);
		break;
	case eChunk_DetailMapTexture:
		m_TerrainChunkRenderArray[chunkindex]->bUseDetailMap = (TextureName != String::empty);
		break;
	case eChunk_SlopeMapTexture:
		m_TerrainChunkRenderArray[chunkindex]->bUseSlopeMap = (TextureName != String::empty);
		break;
	}
	m_TerrainChunkRenderArray[chunkindex]->bUseTriplanarTex = true;

}

uint32* COpenGLTerrainRenderer::updateColorMinimapTexture(uint32* pChunkIndex, uint32 ichunkNum)
{
	if( !pChunkIndex || (ichunkNum <= 0) )
		return NULL;

	uint32 TexID = m_TerrainChunkRenderArray[pChunkIndex[0]]->ChunkTextureID[eChunk_MiniColorMapTexture];


	// bind the color minimap texture and PBO
	m_pRenderDevice->GetTextureManager()->getTextureByID(TexID)->pSGPTexture->BindTexture2D(0);

	uint32 nPBOSize = m_nTerrainSize * SGPTT_TILENUM * m_nTerrainSize * SGPTT_TILENUM * sizeof(uint32);

	// bind PBO to update pixel values
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_TerrainChunkColorMiniMapPBOID);

    m_pRenderDevice->extGlBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, nPBOSize, NULL, GL_STREAM_DRAW);
    uint32* pDst = (uint32*)m_pRenderDevice->extGlMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);


	for( uint32 i=0; i<ichunkNum; i++ )
	{
		uint32 chunkRow = pChunkIndex[i] / m_nTerrainSize; 
		uint32 chunkCol = pChunkIndex[i] % m_nTerrainSize; 
		generateChunkColorMinimapData( pDst, m_nTerrainSize, chunkRow, chunkCol );
	}
    m_pRenderDevice->extGlUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
	
	// copy pixels from PBO to texture object
    // Use offset instead of ponter.
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_nTerrainSize*SGPTT_TILENUM, m_nTerrainSize*SGPTT_TILENUM, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);

	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	return pDst;
}

void COpenGLTerrainRenderer::updateBlendTexture(uint32 SrcX, uint32 SrcZ, uint32 width, uint32 height, uint32* pAlphaBlendData)
{
	uint32 TexID = m_TerrainChunkRenderArray[0]->ChunkTextureID[eChunk_AlphaTexture];

	uint32 nPBOSize = m_nTerrainSize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION * m_nTerrainSize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION * sizeof(uint32);


	// bind the alpha blend texture and PBO
	m_pRenderDevice->GetTextureManager()->getTextureByID(TexID)->pSGPTexture->BindTexture2D(0);

	// bind PBO to update pixel values
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_TerrainChunkAlphaBlendMapPBOID);

	m_pRenderDevice->extGlBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, nPBOSize, NULL, GL_STREAM_DRAW);

    uint32* pDst = (uint32*)m_pRenderDevice->extGlMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
	uint32 Offset = SrcZ * m_nTerrainSize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION + SrcX;
	for( uint32 i=0; i<height; i++ )
	{
		for( uint32 j=0; j<width; j++ )
		{
			pDst[ Offset + j] = pAlphaBlendData[Offset + j];
		}
		Offset += m_nTerrainSize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION;
	}
    m_pRenderDevice->extGlUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer

	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_nTerrainSize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION );
	// copy pixels from PBO to texture object
    // Use offset instead of ponter.
	void* nDataOffset = ((char *)NULL + (sizeof(uint32) * (SrcZ * m_nTerrainSize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION + SrcX)));
    glTexSubImage2D( GL_TEXTURE_2D, 0, SrcX, SrcZ, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, nDataOffset );

	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
}