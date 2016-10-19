

COpenGLES2TerrainRenderer::COpenGLES2TerrainRenderer(COpenGLES2RenderDevice *pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_nChunkIndexVBO(0),
	m_nVeryDetailedChunkNumber(0), m_nLOD0ChunkNumber(0), m_nLOD1ChunkNumber(0),
	m_nLODBlendChunkNumber(0), m_nTerrainSize(1),
	m_TerrainChunkLightMapTexID(2)		// Default Black texture
{
	m_TerrainChunkRenderArray.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE);

	m_VeryDetailedChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);
	m_LOD0ChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);
	m_LOD1ChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);
	m_LODBlendChunkArrayID.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE/3);

	// Create chunk Index VBO (static)
	glGenBuffers(1, &m_nChunkIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nChunkIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk_index_count*sizeof(uint16), chunk_index_tile, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

COpenGLES2TerrainRenderer::~COpenGLES2TerrainRenderer()
{
	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		releaseChunkVBO(i);
	}

	m_TerrainChunkRenderArray.clear(true);

	// Delete chunk Index VBO (static)
	if( m_nChunkIndexVBO != 0 )
		glDeleteBuffers(1, &m_nChunkIndexVBO);
}

void COpenGLES2TerrainRenderer::setTerrainSize(uint32 terrainsize)
{
	m_nTerrainSize = terrainsize;

	if( m_TerrainChunkRenderArray.size() > 0 )
		m_TerrainChunkRenderArray.clear(true);

	for( uint32 i=0; i<terrainsize*terrainsize; i++ )
		m_TerrainChunkRenderArray.add( NULL );
}


void COpenGLES2TerrainRenderer::createChunkVBO(uint32 chunkindex)
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

	glGenBuffers(1, &pChunkRenderInfo->nVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, pChunkRenderInfo->nVBOID);
	glBufferData(GL_ARRAY_BUFFER, pTerrainChunk->GetVertexCount()*nStride, pTerrainChunk->m_ChunkTerrainVertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
	glEnableVertexAttribArray(3);	
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(11*sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(14*sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nChunkIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk_index_count*sizeof(uint16), chunk_index_tile, GL_STATIC_DRAW);
	
	m_pRenderDevice->extGlBindVertexArray(0);


	m_TerrainChunkRenderArray.set(chunkindex, pChunkRenderInfo, false);

	createChunkLODInfo(chunkindex);
}



void COpenGLES2TerrainRenderer::releaseChunkVBO(uint32 chunkindex)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		// Delete VAO and VBO
		glDeleteBuffers(1, &m_TerrainChunkRenderArray[chunkindex]->nVBOID);
		m_pRenderDevice->extGlDeleteVertexArray(1, &m_TerrainChunkRenderArray[chunkindex]->nVAOID);


		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse0Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse0Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse1Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse1Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse2Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse2Texture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse3Texture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_Diffuse3Texture]);
		//if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_NormalMapTexture] != 0 )
		//	m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_NormalMapTexture]);
		if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_DetailMapTexture] != 0 )
			m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_DetailMapTexture]);
		//if( m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_SlopeMapTexture] != 0 )
		//	m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[eChunk_SlopeMapTexture]);


		m_TerrainChunkRenderArray.set(chunkindex, NULL, true);
	}
}

void COpenGLES2TerrainRenderer::createChunkTextureFromWorldMap(const CSGPWorldMap* pWorldMap, uint32 chunkindex)
{
	OpenGLChunkRenderInfo* pChunkRenderInfo = m_TerrainChunkRenderArray[chunkindex];

	jassert( pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_Diffuse0Texture] != -1 );
	

	pChunkRenderInfo->bUseDetailMap = (pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_DetailMapTexture] != -1); 
	//pChunkRenderInfo->bUseSlopeMap = (pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_SlopeMapTexture] != -1); 
	//pChunkRenderInfo->bUseTriplanarTex = true; 
	//pChunkRenderInfo->bUseNormalMap = (pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_NormalMapTexture] != -1); 
	

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

	//if( pChunkRenderInfo->bUseNormalMap )
	//	setChunkTextures(	chunkindex, eChunk_NormalMapTexture,
	//						String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_NormalMapTexture] ].m_ChunkTextureFileName) );
	if( pChunkRenderInfo->bUseDetailMap )
		setChunkTextures(	chunkindex, eChunk_DetailMapTexture,
							String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_DetailMapTexture] ].m_ChunkTextureFileName) );
	//if( pChunkRenderInfo->bUseSlopeMap )
	//	setChunkTextures(	chunkindex, eChunk_SlopeMapTexture,
	//						String(pWorldMap->m_pChunkTextureNames[ pWorldMap->m_pChunkTextureIndex[chunkindex].m_ChunkTextureIndex[eChunk_SlopeMapTexture] ].m_ChunkTextureFileName) );
	
	setChunkTextures(	chunkindex, eChunk_MiniColorMapTexture, 0);
}

void COpenGLES2TerrainRenderer::updateChunkLODInfo(uint32 chunkindex, const Vector4D &ChunkCenter)
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
	else if( /*fDistance_X <= ((float)SGPTT_TILENUM*SGPTT_TILE_METER)*(eHighDetailed-1+0.5f) &&
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



void COpenGLES2TerrainRenderer::createChunkLODInfo(uint32 chunkindex)
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
COpenGLES2TerrainRenderer::EOpenGLChunkPosition COpenGLES2TerrainRenderer::getChunkPosition(float fDistance_X, float fDistance_Z )
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

void COpenGLES2TerrainRenderer::setChunkTextures(uint32 chunkindex, uint8 textureslot, const String& texname)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		uint32 texID = m_pRenderDevice->GetTextureManager()->registerTexture(texname);
		m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[textureslot] = texID;
	}
}

void COpenGLES2TerrainRenderer::setChunkTextures(uint32 chunkindex, uint8 textureslot, uint32 SGPtextureID)
{
	if( m_TerrainChunkRenderArray[chunkindex] != NULL )
	{
		m_TerrainChunkRenderArray[chunkindex]->ChunkTextureID[textureslot] = SGPtextureID;
	}
}

void COpenGLES2TerrainRenderer::renderTerrainChunk(uint32 chunkindex)
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

void COpenGLES2TerrainRenderer::DoDrawTerrainRenderBatch()
{
	if( (m_VeryDetailedChunkArrayID.size() == 0) &&
		(m_LOD0ChunkArrayID.size() == 0) &&
		(m_LOD1ChunkArrayID.size() == 0) &&
		(m_LODBlendChunkArrayID.size() == 0) )
		return;

	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_pRenderDevice->GetShaderManager());
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
			//if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_NormalMapTexture] != 0 )
			//{
			//	m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_NormalMapTexture])->pSGPTexture->BindTexture2D(5);
			//	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerNormalmap", 5);
			//}
			if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_DetailMapTexture] != 0 )
			{
				m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_DetailMapTexture])->pSGPTexture->BindTexture2D(6);
				pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerDetail", 6);
			}
			//if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture] != 0 )
			//{
			//	m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture])->pSGPTexture->BindTexture2D(7);
			//	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("gSamplerSlope", 7);
			//}


			int32 RenderFlags = (m_TerrainChunkRenderArray[*pBegin]->bUseDetailMap ? 1 : 0) |
								//(m_TerrainChunkRenderArray[*pBegin]->bUseSlopeMap ? 2 : 0) |
								//(m_TerrainChunkRenderArray[*pBegin]->bUseTriplanarTex ? 4 : 0) |
								//(m_TerrainChunkRenderArray[*pBegin]->bUseNormalMap ? 8 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[1] != 0 ? 16 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[2] != 0 ? 32 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[3] != 0 ? 64 : 0);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_VERYHIGH)->setShaderUniform("RenderFlag", (float)RenderFlags);

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
			//if( m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture] != 0 )
			//{
			//	m_pRenderDevice->GetTextureManager()->getTextureByID(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[eChunk_SlopeMapTexture])->pSGPTexture->BindTexture2D(5);
			//	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("gSamplerSlope", 5);
			//}

			int32 RenderFlags = //(m_TerrainChunkRenderArray[*pBegin]->bUseSlopeMap ? 2 : 0) |
								//(m_TerrainChunkRenderArray[*pBegin]->bUseTriplanarTex ? 4 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[1] != 0 ? 16 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[2] != 0 ? 32 : 0) |
								(m_TerrainChunkRenderArray[*pBegin]->ChunkTextureID[3] != 0 ? 64 : 0);
			pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD0)->setShaderUniform("RenderFlag", (float)RenderFlags);

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

void COpenGLES2TerrainRenderer::DoDrawReflectionTerrainRenderBatch()
{
	if( (m_VeryDetailedChunkArrayID.size() == 0) &&
		(m_LOD0ChunkArrayID.size() == 0) &&
		(m_LOD1ChunkArrayID.size() == 0) &&
		(m_LODBlendChunkArrayID.size() == 0) )
		return;

	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_pRenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_pRenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_pRenderDevice->getOpenGLWaterRenderer()->m_ObliqueNearPlaneReflectionProjMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &TerrainMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_terrain);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( TerrainMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);



	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->useProgram();

	pShaderManager->GetGLSLShaderProgram(SGPST_TERRAIN_LOD1)->setShaderUniform("worldViewProjMatrix", MVP);
		
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

void COpenGLES2TerrainRenderer::AfterDrawTerrainRenderBatch()
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

void COpenGLES2TerrainRenderer::registerColorMinimapTexture(const CSGPWorldMap* pWorldMap, const String& MinimapTexName, uint32 terrainsize)
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

}

void COpenGLES2TerrainRenderer::unRegisterColorMinimapTexture(const String& MinimapTexName)
{
	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		setChunkTextures(i, eChunk_MiniColorMapTexture, 0);
	}
	m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(MinimapTexName);
}

void COpenGLES2TerrainRenderer::registerBlendTexture(const CSGPWorldMap* pWorldMap, const String& BlendTexName, uint32 terrainsize)
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

}

void COpenGLES2TerrainRenderer::unregisterBlendTexture(const String& BlendTexName)
{
	for( uint32 i=0; i<(uint32)m_TerrainChunkRenderArray.size(); i++ )
	{
		setChunkTextures(i, eChunk_AlphaTexture, 0);
	}
	m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(BlendTexName);

}

void COpenGLES2TerrainRenderer::registerLightmapTexture(const String& WorldMapName, const String& LightmapTexName)
{
	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTexture(
		String(L"Lightmap/") + WorldMapName + File::separator + LightmapTexName,
		false);
	if( TexID == 0 )
		m_TerrainChunkLightMapTexID = 2;		// If no lightmap texture exist, using black texture by default
	else
		m_TerrainChunkLightMapTexID = TexID;
}

void COpenGLES2TerrainRenderer::unregisterLightmapTexture()
{
	if( (m_TerrainChunkLightMapTexID != 0) && (m_TerrainChunkLightMapTexID != 2) )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_TerrainChunkLightMapTexID);
}

