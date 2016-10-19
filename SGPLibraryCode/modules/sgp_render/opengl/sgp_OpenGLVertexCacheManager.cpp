

uint32 COpenGLVertexCacheManager::StaticBuffer_ID = 0;
float COpenGLVertexCacheManager::m_fSin[CIRCLE_SLIDES_MAX] = {0};
float COpenGLVertexCacheManager::m_fCos[CIRCLE_SLIDES_MAX] = {0};

COpenGLVertexCacheManager::COpenGLVertexCacheManager(COpenGLRenderDevice* pRenderDevice) 
	: m_pRenderDevice(pRenderDevice), m_pFullScreenQuadVAO(NULL)
{
	for( int i=0; i<CIRCLE_SLIDES_MAX; i++ )
	{
		float a = i * 2.0f * float_Pi / CIRCLE_SLIDES_MAX;
		m_fSin[i] = std::sinf(a);
		m_fCos[i] = std::cosf(a);
	}

	m_UPOSVCCache.add( new COpenGLDynamicBuffer(	INIT_DB_MAXSIZE, 
													INIT_DB_MAXSIZE,
													sizeof(SGPVertex_UPOS_VERTEXCOLOR),
													SGPVT_UPOS_VERTEXCOLOR,
													this ) );

	m_UPOSTEXCache.add( new COpenGLDynamicBuffer(	INIT_DB_MAXSIZE, 
													INIT_DB_MAXSIZE,
													sizeof(SGPVertex_UPOS_TEXTURE),
													SGPVT_UPOS_TEXTURE,
													this ) );

	m_UPOSTEXVCCache.add( new COpenGLDynamicBuffer( INIT_DB_MAXSIZE, 
													INIT_DB_MAXSIZE,
													sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR),
													SGPVT_UPOS_TEXTURE_VERTEXCOLOR,
													this ) );
	/*
	1			2
	|-----------|
	|			|
	|			|
	|			|
	|-----------|
	0			3
	*/
	// create full screen quad VAO
	uint16 quadIndex[6] = { 0, 2, 1, 0, 3, 2 };
	SGPVertex_FONT quadVert[4];
	quadVert[0].x = -1.0f; quadVert[0].y = -1.0f; quadVert[0].z = 0; quadVert[0].w = 1.0f;
	quadVert[0].VertexColor[0] = quadVert[0].VertexColor[1] = quadVert[0].VertexColor[2] = quadVert[0].VertexColor[3] = 1;
	quadVert[0].tu = 0.0f; quadVert[0].tv = 0.0f;
	quadVert[1].x = -1.0f; quadVert[1].y =  1.0f; quadVert[1].z = 0; quadVert[1].w = 1.0f;
	quadVert[1].VertexColor[0] = quadVert[1].VertexColor[1] = quadVert[1].VertexColor[2] = quadVert[1].VertexColor[3] = 1;
	quadVert[1].tu = 0.0f; quadVert[1].tv = 1.0f;
	quadVert[2].x =  1.0f; quadVert[2].y =  1.0f; quadVert[2].z = 0; quadVert[2].w = 1.0f;
	quadVert[2].VertexColor[0] = quadVert[2].VertexColor[1] = quadVert[2].VertexColor[2] = quadVert[2].VertexColor[3] = 1;
	quadVert[2].tu = 1.0f; quadVert[2].tv = 1.0f;
	quadVert[3].x =  1.0f; quadVert[3].y = -1.0f; quadVert[3].z = 0; quadVert[3].w = 1.0f;
	quadVert[3].VertexColor[0] = quadVert[3].VertexColor[1] = quadVert[3].VertexColor[2] = quadVert[3].VertexColor[3] = 1;
	quadVert[3].tu = 1.0f; quadVert[3].tv = 0.0f;

	m_pFullScreenQuadVAO = new COpenGLVertexBufferObject(pRenderDevice);
	m_pFullScreenQuadVAO->ensureDataSpace(SGPBT_VERTEX, sizeof(SGPVertex_FONT) * 4);
	m_pFullScreenQuadVAO->ensureDataSpace(SGPBT_INDEX, sizeof(uint16) * 6);
	m_pFullScreenQuadVAO->addData(SGPBT_VERTEX, quadVert, sizeof(SGPVertex_FONT) * 4);
	m_pFullScreenQuadVAO->addData(SGPBT_INDEX, quadIndex, sizeof(uint16) * 6);

	m_pFullScreenQuadVAO->createVAO();
	m_pFullScreenQuadVAO->bindVAO();

	m_pFullScreenQuadVAO->createVBO(SGPBT_VERTEX);
	m_pFullScreenQuadVAO->createVBO(SGPBT_INDEX);

	m_pFullScreenQuadVAO->bindVBO(SGPBT_VERTEX);
	m_pFullScreenQuadVAO->uploadDataToGPU(SGPBT_VERTEX, GL_STATIC_DRAW);

	m_pFullScreenQuadVAO->setVAOPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SGPVertex_FONT), (GLvoid *)BUFFER_OFFSET(0));
	m_pFullScreenQuadVAO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SGPVertex_FONT), (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
	m_pFullScreenQuadVAO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SGPVertex_FONT), (GLvoid *)BUFFER_OFFSET(8*sizeof(float)));

	m_pFullScreenQuadVAO->bindVBO(SGPBT_INDEX);
	m_pFullScreenQuadVAO->uploadDataToGPU(SGPBT_INDEX, GL_STATIC_DRAW);
	m_pFullScreenQuadVAO->unBindVAO();

}

COpenGLVertexCacheManager::~COpenGLVertexCacheManager()
{
	// Last Chance release all static buffers
	for( int i=0; i<m_pSB.size(); i++ )
	{
		if( m_pSB[i] )
			delete m_pSB[i];
	}

	// Last Chance release all TBO
	for( int i=0; i<m_pTBO.size(); i++ )
	{
		if( m_pTBO[i] )
		{
			m_pTBO[i]->deleteTBO();
			delete m_pTBO[i];
		}
	}
	

	// free dynamic vertex caches
	for( int i=0; i<m_UPOSVCCache.size(); i++ )
	{
		if( m_UPOSVCCache[i] )
			delete m_UPOSVCCache[i];
	}
	for( int i=0; i<m_UPOSTEXCache.size(); i++ )
	{
		if( m_UPOSTEXCache[i] )
			delete m_UPOSTEXCache[i];
	}
	for( int i=0; i<m_UPOSTEXVCCache.size(); i++ )
	{
		if( m_UPOSTEXVCCache[i] )
			delete m_UPOSTEXVCCache[i];
	}

	if( m_pFullScreenQuadVAO )
	{
		m_pFullScreenQuadVAO->deleteVBO(SGPBT_VERTEX_AND_INDEX);
		m_pFullScreenQuadVAO->deleteVAO();
		delete m_pFullScreenQuadVAO;
		m_pFullScreenQuadVAO = NULL;
	}
}

/**
 * commit all dynamic buffers. Call this function if you want to force all
 * contents send to render batch to be drawn. Normally it will be called
 * before Rendering render batch.
 */
void COpenGLVertexCacheManager::ForcedCommitAll(void)
{
	for( int i=0; i<m_UPOSVCCache.size(); i++ )
	{
		if( m_UPOSVCCache[i]->IsNotEmpty() )
			m_UPOSVCCache[i]->Commit();
	}
	for( int i=0; i<m_UPOSTEXCache.size(); i++ )
	{
		if( m_UPOSTEXCache[i]->IsNotEmpty() )
			m_UPOSTEXCache[i]->Commit();
				
	}
	for( int i=0; i<m_UPOSTEXVCCache.size(); i++ )
	{
		if( m_UPOSTEXVCCache[i]->IsNotEmpty() )
			m_UPOSTEXVCCache[i]->Commit();
	}
}

/**
 * clear all dynamic buffers. 
 */
void COpenGLVertexCacheManager::ForcedClearAll(void)
{
	for( int i=0; i<m_UPOSVCCache.size(); i++ )
	{
		if( m_UPOSVCCache[i]->IsNotEmpty() )
			m_UPOSVCCache[i]->Clear();
	}
	for( int i=0; i<m_UPOSTEXCache.size(); i++ )
	{
		if( m_UPOSTEXCache[i]->IsNotEmpty() )
			m_UPOSTEXCache[i]->Clear();
				
	}
	for( int i=0; i<m_UPOSTEXVCCache.size(); i++ )
	{
		if( m_UPOSTEXVCCache[i]->IsNotEmpty() )
			m_UPOSTEXVCCache[i]->Clear();
	}
}

/**
 * Create a static vertex/index buffer for the given data and returns
 * a index to that buffer for later rendering processes.
 * -> IN:  SGP_VERTEX_TYPE - identify the vertex format used
 *         SGPSkin       - mat skin that should be used
 *         uint32        - number of vertices to come
 *         uint32        - number of indices to come
 *         void*         - pointer to vertex stream
 *         uint16*       - pointer to index stream
 * -> OUT: uint32        - ID to the created StaticBuffer (0 for error)
 */
uint32 COpenGLVertexCacheManager::CreateStaticBuffer(
	SGP_VERTEX_TYPE VertexType,
	const SGPSkin& skin,
	const AABBox& boundingbox,
	uint32  nVertexNum, 
	uint32  nIndexNum, 
	const void   *pVerts,
	const uint16 *pIndis )
{
	jassert((nVertexNum>0) && (nIndexNum>0) && (pVerts) && (pIndis));

	COpenGLStaticBuffer* pNewSB = new COpenGLStaticBuffer();
	pNewSB->nNumVerts = nVertexNum;
	pNewSB->nNumIndis = nIndexNum;
	pNewSB->MaterialSkin = skin;
	pNewSB->BoundingBox = boundingbox;


	switch(VertexType)
	{
	case SGPVT_UPOS_VERTEXCOLOR:
		pNewSB->nStride = sizeof(SGPVertex_UPOS_VERTEXCOLOR);
		break;
	case SGPVT_UPOS_TEXTURE:
		pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURE);
		break;
	case SGPVT_UPOS_TEXTURE_VERTEXCOLOR:
		pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR);
		break;
	case SGPVT_UPOS_TEXTURETWO:
		pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURETWO);
		break;
	case SGPVT_UPOS_TEXTURETWO_VERTEXCOLOR:
		pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURETWO_VC);
		break;
	//case SGPVT_UPOS_NORMAL_TEXTURE_VERTEXCOLOR:
	//	pNewSB->nStride = sizeof(SGPVertex_UPOS_NOR_TEX_VC);
	//	break;
	case SGPVT_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX:
		pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX);
		break;
	case SGPVT_ANIM:
		pNewSB->nStride = sizeof(SGPVertex_ANIM);
		break;
	default:
		jassertfalse;
		Logger::getCurrentLogger()->writeToLog(String("Not Supported Static Buffer Vertex Type"), ELL_ERROR);
		return 0;
	}

	// create the VBO
	pNewSB->pVBO = new COpenGLVertexBufferObject( m_pRenderDevice );

	pNewSB->pVBO->ensureDataSpace(SGPBT_VERTEX, nVertexNum*pNewSB->nStride);
	pNewSB->pVBO->ensureDataSpace(SGPBT_INDEX, nIndexNum*sizeof(uint16));

	pNewSB->pVBO->addData(SGPBT_VERTEX, pVerts, nVertexNum*pNewSB->nStride);
	pNewSB->pVBO->addData(SGPBT_INDEX, pIndis, nIndexNum*sizeof(uint16));

	pNewSB->pVBO->createVAO();
	pNewSB->pVBO->bindVAO();

	pNewSB->pVBO->createVBO(SGPBT_VERTEX);
	pNewSB->pVBO->createVBO(SGPBT_INDEX);

	pNewSB->pVBO->bindVBO(SGPBT_VERTEX);
	pNewSB->pVBO->initVBOBuffer(SGPBT_VERTEX, nVertexNum*pNewSB->nStride, GL_STATIC_DRAW);
	pNewSB->pVBO->uploadDataToGPU(SGPBT_VERTEX, GL_STATIC_DRAW);
	switch(VertexType)
	{
	case SGPVT_UPOS_VERTEXCOLOR:
		pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		break;
	case SGPVT_UPOS_TEXTURE:
		pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		break;
	case SGPVT_UPOS_TEXTURE_VERTEXCOLOR:
		pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		break;
	case SGPVT_UPOS_TEXTURETWO:
		pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(5*sizeof(float)));
		break;
	case SGPVT_UPOS_TEXTURETWO_VERTEXCOLOR:
		pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(3, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
		break;
	//case SGPVT_UPOS_NORMAL_TEXTURE_VERTEXCOLOR:
	//	pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
	//	pNewSB->pVBO->setVAOPointer(1, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
	//	pNewSB->pVBO->setVAOPointer(2, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(6*sizeof(float)));
	//	pNewSB->pVBO->setVAOPointer(3, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(10*sizeof(float)));
	//	break;
	case SGPVT_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX:
		pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(3, 1, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
		break;
	case SGPVT_ANIM:
		pNewSB->pVBO->setVAOPointer(0, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
		pNewSB->pVBO->setVAOPointer(1, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(3, 4, GL_BYTE, GL_TRUE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
		pNewSB->pVBO->setVAOPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)+4*sizeof(uint8)));
		break;
	default:
		break;
	}

	pNewSB->pVBO->bindVBO(SGPBT_INDEX);
	pNewSB->pVBO->initVBOBuffer(SGPBT_INDEX, nIndexNum*sizeof(uint16), GL_STATIC_DRAW);
	pNewSB->pVBO->uploadDataToGPU(SGPBT_INDEX, GL_STATIC_DRAW);

	pNewSB->pVBO->unBindVAO();

	StaticBuffer_ID++;
	pNewSB->nSBID = StaticBuffer_ID;
	
	//GetDevice()->getOpenGLMaterialRenderer()->CommitAdditionalMaterial(pNewSB->MaterialModifier, pNewSB->MaterialSkin);
	m_pSB.add(pNewSB);

	return StaticBuffer_ID;
}

/**
 * Create a static vertex/index buffer for the given SGPMF1Skin, SGPMF1Mesh
 * and returns a handle to that buffer for later rendering processes.
 * -> IN:  
 *         SGPMF1Skin    - material skin that come from MF1 file
 *         SGPMF1Mesh    - vertices and index data that come from MF1 file
 * -> OUT: uint32        - ID to the created StaticBuffer (0 for error)
 */
uint32 COpenGLVertexCacheManager::CreateMF1MeshStaticBuffer( 
	const SGPMF1Skin& MeshSkin, 
	const SGPMF1Mesh& MF1Mesh,
	const SGPMF1BoneGroup* pBoneGroup,
	uint32 NumBoneGroup )
{
	jassert( (MF1Mesh.m_iNumVerts>0) && (MF1Mesh.m_iNumIndices>0) &&
			 (MF1Mesh.m_pVertex) && (MF1Mesh.m_pIndices) );

	bool bAVMesh = ( MF1Mesh.m_nType == static_cast<uint32>(SGPMESHCF_AVMESH) );

	COpenGLStaticBuffer* pNewSB = new COpenGLStaticBuffer();
	pNewSB->nNumVerts = MF1Mesh.m_iNumVerts;
	pNewSB->nNumIndis = MF1Mesh.m_iNumIndices;
	pNewSB->BoundingBox = MF1Mesh.m_bbox;
	pNewSB->MaterialSkin.bAlpha = ((MeshSkin.m_iMtlFlag & SGPMESHRF_ALPHABLEND) > 0) ? true : false;
	pNewSB->MaterialSkin.bAlpha |=  bAVMesh;
	pNewSB->MaterialSkin.bAlphaTest = ((MeshSkin.m_iMtlFlag & SGPMESHRF_ALPHATEST) > 0) ? true : false;
	pNewSB->MaterialSkin.nTextureNum = 1;
	pNewSB->MaterialSkin.nTextureID[0] = GetDevice()->GetTextureManager()->getTextureIDByName(String(MeshSkin.m_cName));
	
	pNewSB->MaterialSkin.vUVSpeed[0] = MeshSkin.m_fUSpeed;
	pNewSB->MaterialSkin.vUVSpeed[1] = MeshSkin.m_fVSpeed;
	pNewSB->MaterialSkin.vUVTile[0] = MeshSkin.m_UTile;
	pNewSB->MaterialSkin.vUVTile[1] = MeshSkin.m_VTile;
	pNewSB->MaterialSkin.vUVTile[2] = MeshSkin.m_iStartFrame;
	pNewSB->MaterialSkin.vUVTile[3] = MeshSkin.m_iLoopMode;
	pNewSB->MaterialSkin.vUVTile[4] = (uint16)MeshSkin.m_fPlayBackRate;
	pNewSB->MaterialSkin.nNumMatKeyFrame = MeshSkin.m_iNumMatKeyFrame;
	pNewSB->MaterialSkin.pMatKeyFrame = MeshSkin.m_pMatKeyFrame;

	SGP_VERTEX_TYPE VertexType = SGPVT_UPOS_TEXTURE;
	if( ( MF1Mesh.m_nType < static_cast<uint32>(SGPMESHCF_BBRD) ||
		  MF1Mesh.m_nType > static_cast<uint32>(SGPMESHCF_BBRD_VERTICALGROUND) ) &&
		(NumBoneGroup > 0) &&
		pBoneGroup )
	{
		VertexType = SGPVT_ANIM;
		pNewSB->nStride = sizeof(SGPVertex_ANIM);
		pNewSB->MaterialSkin.nShaderType = pNewSB->MaterialSkin.bAlphaTest ? SGPST_SKELETONANIM_ALPHATEST : SGPST_SKELETONANIM;
	}
	else if( (MF1Mesh.m_iNumUV1 > 0) && MF1Mesh.m_pTexCoords1 &&
			 (MF1Mesh.m_iNumUV0 > 0) && MF1Mesh.m_pTexCoords0 )
	{
		jassert( MF1Mesh.m_iNumUV0 == MF1Mesh.m_iNumVerts );
		jassert( MF1Mesh.m_iNumUV1 == MF1Mesh.m_iNumVerts );

		pNewSB->MaterialSkin.nTextureNum = 2;
		pNewSB->MaterialSkin.nTextureID[1] = 1;		// Default White Texture as lightmap texture
		pNewSB->MaterialSkin.bLightMap = true;

		if( (MF1Mesh.m_iNumVertexColor > 0) && MF1Mesh.m_pVertexColor )
		{
			jassert( (MF1Mesh.m_iNumVertexColor == MF1Mesh.m_iNumVerts) || (MF1Mesh.m_iNumVertexColor == 1) );
			VertexType = SGPVT_UPOS_TEXTURETWO_VERTEXCOLOR;
			pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURETWO_VC);
			pNewSB->MaterialSkin.nShaderType = pNewSB->MaterialSkin.bAlphaTest ? SGPST_VERTEXCOLOR_LIGHTMAP_ALPHATEST : SGPST_VERTEXCOLOR_LIGHTMAP;
		}
		else
		{
			VertexType = SGPVT_UPOS_TEXTURETWO;
			pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURETWO);
			pNewSB->MaterialSkin.nShaderType = pNewSB->MaterialSkin.bAlphaTest ? SGPST_LIGHTMAP_ALPHATEST : SGPST_LIGHTMAP;
		}
	}
	else if( MF1Mesh.m_iNumUV0 > 0 && MF1Mesh.m_pTexCoords0 )
	{
		jassert( MF1Mesh.m_iNumUV0 == MF1Mesh.m_iNumVerts );

		if( MF1Mesh.m_iNumVertexColor > 0 && MF1Mesh.m_pVertexColor )
		{
			jassert( MF1Mesh.m_iNumVertexColor == MF1Mesh.m_iNumVerts || MF1Mesh.m_iNumVertexColor == 1 );
			VertexType = SGPVT_UPOS_TEXTURE_VERTEXCOLOR;
			pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR);
			pNewSB->MaterialSkin.nShaderType = pNewSB->MaterialSkin.bAlphaTest ? SGPST_VERTEXCOLOR_TEXTURE_ALPHATEST : SGPST_VERTEXCOLOR_TEXTURE;
		}
		else
		{
			VertexType = SGPVT_UPOS_TEXTURE;
			pNewSB->nStride = sizeof(SGPVertex_UPOS_TEXTURE);
			pNewSB->MaterialSkin.nShaderType = pNewSB->MaterialSkin.bAlphaTest ? SGPST_TEXTURE_ALPHATEST : SGPST_TEXTURE;
		}
	}
	else
	{
		jassert( MF1Mesh.m_iNumVertexColor == MF1Mesh.m_iNumVerts || MF1Mesh.m_iNumVertexColor == 1 );
		VertexType = SGPVT_UPOS_VERTEXCOLOR;
		pNewSB->nStride = sizeof(SGPVertex_UPOS_VERTEXCOLOR);
		pNewSB->MaterialSkin.nShaderType = SGPST_VERTEXCOLOR;
	}


	uint32 nSizeV = pNewSB->nStride * pNewSB->nNumVerts;
	uint32 nSizeI = sizeof(uint16) * pNewSB->nNumIndis;

	// create the VBO
	pNewSB->pVBO = new COpenGLVertexBufferObject( m_pRenderDevice );

	pNewSB->pVBO->createVAO();
	pNewSB->pVBO->bindVAO();

	pNewSB->pVBO->createVBO(SGPBT_VERTEX);
	pNewSB->pVBO->createVBO(SGPBT_INDEX);

	pNewSB->pVBO->bindVBO(SGPBT_VERTEX);
	pNewSB->pVBO->initVBOBuffer(SGPBT_VERTEX, pNewSB->nNumVerts*pNewSB->nStride, GL_STATIC_DRAW);

	switch(VertexType)
	{
	case SGPVT_UPOS_VERTEXCOLOR:
		{
			pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
			pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		
			// copy vertex data into the video memory
			SGPVertex_UPOS_VERTEXCOLOR* tmp_pVerts = (SGPVertex_UPOS_VERTEXCOLOR*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeV);
			for( uint32 i=0; i<pNewSB->nNumVerts; i++ )
			{
				tmp_pVerts[i].x = MF1Mesh.m_pVertex[i].vPos[0];
				tmp_pVerts[i].y = MF1Mesh.m_pVertex[i].vPos[1];
				tmp_pVerts[i].z = MF1Mesh.m_pVertex[i].vPos[2];
				if( bAVMesh )
				{
					tmp_pVerts[i].VertexColor[0] = tmp_pVerts[i].VertexColor[1] = tmp_pVerts[i].VertexColor[2] = 1.0f;
					// If AVMESH, Using R Channel as alpha, RGB channel is ignored (1.0)
					tmp_pVerts[i].VertexColor[3] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0];
				}
				else
				{
					// If Not AVMESH, Alpha Channel is ignored (1.0)
					tmp_pVerts[i].VertexColor[0] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0];
					tmp_pVerts[i].VertexColor[1] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[1] : MF1Mesh.m_pVertexColor[i].m_fVertColor[1];
					tmp_pVerts[i].VertexColor[2] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[2] : MF1Mesh.m_pVertexColor[i].m_fVertColor[2];
					tmp_pVerts[i].VertexColor[3] = 1.0f;
				}
			}
		}
		break;
	case SGPVT_UPOS_TEXTURE:
		{
			pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
			pNewSB->pVBO->setVAOPointer(1, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
		
			// copy vertex data into the video memory
			SGPVertex_UPOS_TEXTURE* tmp_pVerts = (SGPVertex_UPOS_TEXTURE*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeV);
			for( uint32 i=0; i<pNewSB->nNumVerts; i++ )
			{
				tmp_pVerts[i].x = MF1Mesh.m_pVertex[i].vPos[0];
				tmp_pVerts[i].y = MF1Mesh.m_pVertex[i].vPos[1];
				tmp_pVerts[i].z = MF1Mesh.m_pVertex[i].vPos[2];
				tmp_pVerts[i].tu = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[0];
				tmp_pVerts[i].tv = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[1];
			}
		}
		break;
	case SGPVT_UPOS_TEXTURETWO:
		{
			pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
			pNewSB->pVBO->setVAOPointer(1, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(5*sizeof(float)));

			// copy vertex data into the video memory
			SGPVertex_UPOS_TEXTURETWO* tmp_pVerts = (SGPVertex_UPOS_TEXTURETWO*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeV);
			for( uint32 i=0; i<pNewSB->nNumVerts; i++ )
			{
				tmp_pVerts[i].x = MF1Mesh.m_pVertex[i].vPos[0];
				tmp_pVerts[i].y = MF1Mesh.m_pVertex[i].vPos[1];
				tmp_pVerts[i].z = MF1Mesh.m_pVertex[i].vPos[2];
				tmp_pVerts[i].tu = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[0];
				tmp_pVerts[i].tv = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[1];
				tmp_pVerts[i].tu1 = MF1Mesh.m_pTexCoords1[i].m_fTexCoord[0];
				tmp_pVerts[i].tv1 = MF1Mesh.m_pTexCoords1[i].m_fTexCoord[1];
			}
		}
		break;
	case SGPVT_UPOS_TEXTURETWO_VERTEXCOLOR:
		{
			pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
			pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(3, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));

			// copy vertex data into the video memory
			SGPVertex_UPOS_TEXTURETWO_VC* tmp_pVerts = (SGPVertex_UPOS_TEXTURETWO_VC*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeV);
			for( uint32 i=0; i<pNewSB->nNumVerts; i++ )
			{
				tmp_pVerts[i].x = MF1Mesh.m_pVertex[i].vPos[0];
				tmp_pVerts[i].y = MF1Mesh.m_pVertex[i].vPos[1];
				tmp_pVerts[i].z = MF1Mesh.m_pVertex[i].vPos[2];
				if( bAVMesh )
				{
					tmp_pVerts[i].VertexColor[0] = tmp_pVerts[i].VertexColor[1] = tmp_pVerts[i].VertexColor[2] = 1.0f;
					// If AVMESH, Using R Channel as alpha RGB channel is ignored (1.0)
					tmp_pVerts[i].VertexColor[3] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0];
				}
				else
				{
					// If Not AVMESH, Alpha Channel is ignored (1.0)
					tmp_pVerts[i].VertexColor[0] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0];
					tmp_pVerts[i].VertexColor[1] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[1] : MF1Mesh.m_pVertexColor[i].m_fVertColor[1];
					tmp_pVerts[i].VertexColor[2] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[2] : MF1Mesh.m_pVertexColor[i].m_fVertColor[2];
					tmp_pVerts[i].VertexColor[3] = 1.0f;
				}
				tmp_pVerts[i].tu = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[0];
				tmp_pVerts[i].tv = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[1];
				tmp_pVerts[i].tu1 = MF1Mesh.m_pTexCoords1[i].m_fTexCoord[0];
				tmp_pVerts[i].tv1 = MF1Mesh.m_pTexCoords1[i].m_fTexCoord[1];
			}
		}
		break;
	case SGPVT_UPOS_TEXTURE_VERTEXCOLOR:
		{
			pNewSB->pVBO->setVAOPointer(0, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
			pNewSB->pVBO->setVAOPointer(1, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
		
			// copy vertex data into the video memory
			SGPVertex_UPOS_TEXTURE_VERTEXCOLOR* tmp_pVerts = (SGPVertex_UPOS_TEXTURE_VERTEXCOLOR*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeV);
			for( uint32 i=0; i<pNewSB->nNumVerts; i++ )
			{
				tmp_pVerts[i].x = MF1Mesh.m_pVertex[i].vPos[0];
				tmp_pVerts[i].y = MF1Mesh.m_pVertex[i].vPos[1];
				tmp_pVerts[i].z = MF1Mesh.m_pVertex[i].vPos[2];
				if( bAVMesh )
				{
					tmp_pVerts[i].VertexColor[0] = tmp_pVerts[i].VertexColor[1] = tmp_pVerts[i].VertexColor[2] = 1.0f;
					// If AVMESH, Using R Channel as alpha RGB channel is ignored (1.0)
					tmp_pVerts[i].VertexColor[3] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0];
				}
				else
				{
					// If Not AVMESH, Alpha Channel is ignored (1.0)
					tmp_pVerts[i].VertexColor[0] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0];
					tmp_pVerts[i].VertexColor[1] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[1] : MF1Mesh.m_pVertexColor[i].m_fVertColor[1];
					tmp_pVerts[i].VertexColor[2] = (MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[2] : MF1Mesh.m_pVertexColor[i].m_fVertColor[2];
					tmp_pVerts[i].VertexColor[3] = 1.0f;
				}
				tmp_pVerts[i].tu = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[0];
				tmp_pVerts[i].tv = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[1];
			}
		}
		break;
	case SGPVT_ANIM:
		{
			pNewSB->pVBO->setVAOPointer(0, 4, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(0));
			pNewSB->pVBO->setVAOPointer(1, 3, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(2, 2, GL_FLOAT, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(7*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(3, 4, GL_UNSIGNED_BYTE, GL_FALSE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)));
			pNewSB->pVBO->setVAOPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, pNewSB->nStride, (GLvoid *)BUFFER_OFFSET(9*sizeof(float)+4*sizeof(uint8)));

			// copy vertex data into the video memory
			SGPVertex_ANIM* tmp_pVerts = (SGPVertex_ANIM*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_VERTEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeV);
			for( uint32 i=0; i<pNewSB->nNumVerts; i++ )
			{
				tmp_pVerts[i].x = MF1Mesh.m_pVertex[i].vPos[0];
				tmp_pVerts[i].y = MF1Mesh.m_pVertex[i].vPos[1];
				tmp_pVerts[i].z = MF1Mesh.m_pVertex[i].vPos[2];
				tmp_pVerts[i].alpha = bAVMesh ? ((MF1Mesh.m_iNumVertexColor == 1) ? MF1Mesh.m_pVertexColor[0].m_fVertColor[0] : MF1Mesh.m_pVertexColor[i].m_fVertColor[0]) : 1.0f;
				tmp_pVerts[i].vcNormal[0] = MF1Mesh.m_pVertex[i].vNormal[0];
				tmp_pVerts[i].vcNormal[1] = MF1Mesh.m_pVertex[i].vNormal[1];
				tmp_pVerts[i].vcNormal[2] = MF1Mesh.m_pVertex[i].vNormal[2];
				tmp_pVerts[i].tu = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[0];
				tmp_pVerts[i].tv = MF1Mesh.m_pTexCoords0[i].m_fTexCoord[1];
				uint16 nBGID = MF1Mesh.m_pVertexBoneGroupID[i];
				if( nBGID != 0xFFFF )
				{
					tmp_pVerts[i].BlendIndices[0] = pBoneGroup[nBGID].BoneIndex.x;
					tmp_pVerts[i].BlendIndices[1] = pBoneGroup[nBGID].BoneIndex.y;
					tmp_pVerts[i].BlendIndices[2] =	pBoneGroup[nBGID].BoneIndex.z;
					tmp_pVerts[i].BlendIndices[3] =	pBoneGroup[nBGID].BoneIndex.w;
					tmp_pVerts[i].BlendWeights[0] = pBoneGroup[nBGID].BoneWeight.x;
					tmp_pVerts[i].BlendWeights[1] =	pBoneGroup[nBGID].BoneWeight.y;
					tmp_pVerts[i].BlendWeights[2] =	pBoneGroup[nBGID].BoneWeight.z;
					tmp_pVerts[i].BlendWeights[3] =	pBoneGroup[nBGID].BoneWeight.w;
				}
				else
				{
					tmp_pVerts[i].BlendIndices[0] = tmp_pVerts[i].BlendIndices[1] = tmp_pVerts[i].BlendIndices[2] = tmp_pVerts[i].BlendIndices[3] = 0;
					tmp_pVerts[i].BlendWeights[0] = tmp_pVerts[i].BlendWeights[1] = tmp_pVerts[i].BlendWeights[2] = tmp_pVerts[i].BlendWeights[3] = 0;
				}
			}
		}
		break;
	}
	
	pNewSB->pVBO->bindVBO(SGPBT_INDEX);
	pNewSB->pVBO->initVBOBuffer(SGPBT_INDEX, pNewSB->nNumIndis*sizeof(uint16), GL_STATIC_DRAW);
	
	// copy index data into the video memory
	uint16* tmp_pIndis = (uint16*)pNewSB->pVBO->mapSubBufferToMemory(SGPBT_INDEX, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT, 0, nSizeI);
	memcpy(tmp_pIndis, MF1Mesh.m_pIndices, nSizeI);

	pNewSB->pVBO->unmapBuffer(SGPBT_VERTEX_AND_INDEX);

	pNewSB->pVBO->unBindVAO();

	StaticBuffer_ID++;
	pNewSB->nSBID = StaticBuffer_ID;

	m_pSB.add(pNewSB);

	return StaticBuffer_ID;
}



/**
 * Takes the vertex and index lists and sorts them into one fitting
 * vertex cache that features the same vertexID. Note that output of pixel
 * to any buffers is not immediately. 
 * -> IN: SGP_VERTEX_TYPE - vertex type to be processed
 *		  SGPSkin         - mat skin that should be used
 *        uint32        - number of vertices in this call
 *        uint32        - number of indices in this call
 *        void*         - array of vertices
 *        uint16*       - array of indices
 **/
void COpenGLVertexCacheManager::RenderDynamicBuffer(
	SGP_VERTEX_TYPE VertexType,
	const SGPSkin& skin,
	uint32 nVertexNum, 
	uint32 nIndexNum, 
	const void   *pVerts,
	const uint16 *pIndis )
{
	Array<COpenGLDynamicBuffer*> *pCache = NULL;

	// which vertex type is to be processed?
	switch(VertexType)
	{
    case SGPVT_UPOS_VERTEXCOLOR:		{ pCache = &m_UPOSVCCache; } break;
    case SGPVT_UPOS_TEXTURE:			{ pCache = &m_UPOSTEXCache; } break;
    case SGPVT_UPOS_TEXTURE_VERTEXCOLOR:{ pCache = &m_UPOSTEXVCCache; } break;

    default: 
		break;
    }

	if( !pCache )
	{
		jassertfalse;
		Logger::getCurrentLogger()->writeToLog(String("Only Support Three Dynamic Buffer Vertex Type"), ELL_ERROR);
		return;
	}

	for(int i=0; i<pCache->size(); i++)
	{
		// first check if any cache uses same skin.
		if( (*pCache)[i]->IsUseSameSkin(skin) )
		{
			if( (*pCache)[i]->IsCapacityEnough(nVertexNum, nIndexNum) )
				return (*pCache)[i]->Add(nVertexNum, nIndexNum, pVerts, pIndis);
		}
		// second check if empty
		if( !(*pCache)[i]->IsNotEmpty() )
		{
			(*pCache)[i]->SetMaterialSkin(skin);
			return (*pCache)[i]->Add(nVertexNum, nIndexNum, pVerts, pIndis);
		}
	}

	// Push new vertex cache
	COpenGLDynamicBuffer *pNewVC = NULL;
	// which vertex type is to be processed?
	switch(VertexType)
	{
    case SGPVT_UPOS_VERTEXCOLOR:		
		{ 
			pNewVC = new COpenGLDynamicBuffer(	INIT_DB_MAXSIZE, 
												INIT_DB_MAXSIZE,
												sizeof(SGPVertex_UPOS_VERTEXCOLOR),
												SGPVT_UPOS_VERTEXCOLOR,
												this );
			pCache->add( pNewVC );
		} break;
    case SGPVT_UPOS_TEXTURE:		
		{ 
			pNewVC = new COpenGLDynamicBuffer(	INIT_DB_MAXSIZE, 
												INIT_DB_MAXSIZE,
												sizeof(SGPVertex_UPOS_TEXTURE),
												SGPVT_UPOS_TEXTURE,
												this );
			pCache->add( pNewVC );
		} break;
    case SGPVT_UPOS_TEXTURE_VERTEXCOLOR:	
		{ 
			pNewVC = new COpenGLDynamicBuffer(	INIT_DB_MAXSIZE, 
												INIT_DB_MAXSIZE,
												sizeof(SGPVertex_UPOS_TEXTURE_VERTEXCOLOR),
												SGPVT_UPOS_TEXTURE_VERTEXCOLOR,
												this );
			pCache->add( pNewVC );
		} break;
    }

	return pNewVC->Add(nVertexNum, nIndexNum, pVerts, pIndis);
}

void COpenGLVertexCacheManager::ClearAllStaticBuffer()
{
	for( int i=0; i<m_pSB.size(); i++ )
	{
		if( m_pSB[i] )
			delete m_pSB[i];
		m_pSB.getReference(i) = NULL;
	}
	m_pSB.clear();
}

void COpenGLVertexCacheManager::ClearStaticBuffer( uint32 nSBufferID )
{
	if( nSBufferID == 0 )
		return;

	int i = 0;
	for( i=0; i<m_pSB.size(); i++ )
	{
		if( m_pSB[i]->nSBID == nSBufferID )
		{
			delete m_pSB[i];
			break;
		}
	}

	if( i<m_pSB.size() )
		m_pSB.remove(i);
}

void COpenGLVertexCacheManager::ClearAllTextureBufferObject()
{
	for( int i=0; i<m_pTBO.size(); i++ )
	{
		if( m_pTBO[i] )
		{
			m_pTBO[i]->deleteTBO();
			delete m_pTBO[i];
			m_pTBO.getReference(i) = NULL;
		}
		
	}
	m_pTBO.clear();
}

void COpenGLVertexCacheManager::ClearTextureBufferObject( uint32 nTBOID )
{
	if( nTBOID == 0xFFFFFFFF || nTBOID >= (uint32)m_pTBO.size() )
		return;

	if( m_pTBO[nTBOID] )
	{
		m_pTBO[nTBOID]->deleteTBO();
		delete m_pTBO[nTBOID];
		m_pTBO.getReference(nTBOID) = NULL;
	}

	m_pTBO.remove(nTBOID);
}


void* COpenGLVertexCacheManager::GetStaticBufferByID(uint32 nSBufferID)
{
	int i = 0;
	for( i=0; i<m_pSB.size(); i++ )
	{
		if( m_pSB[i]->nSBID == nSBufferID )
			break;
	}

	if( i == m_pSB.size() )
		return NULL;

	return (void*)m_pSB[i];
}

void COpenGLVertexCacheManager::RenderStaticBuffer( uint32 nSBufferID, const Matrix4x4& matWorld, const RenderBatchConfig& config )
{
	int i = 0;
	for( i=0; i<m_pSB.size(); i++ )
	{
		if( m_pSB[i]->nSBID == nSBufferID )
			break;
	}

	if( i == m_pSB.size() )
		return;

	COpenGLStaticBuffer *pStaticBuffer = m_pSB[i];

	// Get correct Material info
	const SGPSkin& StaticBufferSkin = pStaticBuffer->MaterialSkin;
	
	// Commit this cache to RenderBatch
	if( StaticBufferSkin.nPrimitiveType == SGPPT_POINTS ||
		StaticBufferSkin.nPrimitiveType == SGPPT_LINE_STRIP ||
		StaticBufferSkin.nPrimitiveType == SGPPT_LINE_LOOP ||
		StaticBufferSkin.nPrimitiveType == SGPPT_LINES )
	{
		CLineRenderBatch newRenderBatch(GetDevice());
		newRenderBatch.m_BBOXCenter = pStaticBuffer->BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld = matWorld;
		newRenderBatch.m_BatchConfig = config;
		newRenderBatch.m_pSB = pStaticBuffer;
		newRenderBatch.m_pVC = NULL;
		newRenderBatch.BuildQueueValue();
		GetDevice()->getOpenGLMaterialRenderer()->PushLineRenderBatch(newRenderBatch);
	}
	else if( StaticBufferSkin.bAlpha || (config.m_fBatchAlpha < 1.0f) )
	{
		//transparent
		CTransparentRenderBatch newRenderBatch(GetDevice());
		newRenderBatch.m_BBOXCenter = pStaticBuffer->BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld = matWorld;
		newRenderBatch.m_BatchConfig = config;
		newRenderBatch.m_pSB = pStaticBuffer;
		newRenderBatch.m_pVC = NULL;
		newRenderBatch.BuildQueueValue();
		GetDevice()->getOpenGLMaterialRenderer()->PushTransparentRenderBatch(newRenderBatch);
	}
	else
	{
		//opaque
		COpaqueRenderBatch newRenderBatch(GetDevice());
		newRenderBatch.m_BBOXCenter = pStaticBuffer->BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld = matWorld;
		newRenderBatch.m_BatchConfig = config;
		newRenderBatch.m_pSB = pStaticBuffer;
		newRenderBatch.m_pVC = NULL;
		newRenderBatch.BuildQueueValue();
		GetDevice()->getOpenGLMaterialRenderer()->PushOpaqueRenderBatch(newRenderBatch);
	}	
}

void COpenGLVertexCacheManager::RenderSkeletonMesh( uint32 nSBufferID, const Matrix4x4& matWorld, uint32 nTBOID, const RenderBatchConfig& config )
{
	int i = 0;
	for( i=0; i<m_pSB.size(); i++ )
	{
		if( m_pSB[i]->nSBID == nSBufferID )
			break;
	}

	if( i == m_pSB.size() )
		return;

	COpenGLStaticBuffer *pStaticBuffer = m_pSB[i];

	// Get correct Material info
	const SGPSkin& StaticBufferSkin = pStaticBuffer->MaterialSkin;

	if( (StaticBufferSkin.nShaderType == SGPST_SKELETONANIM) ||
		(StaticBufferSkin.nShaderType == SGPST_SKELETONANIM_ALPHATEST) )
	{
		CSkinAnimRenderBatch newRenderBatch(GetDevice());
		newRenderBatch.m_BBOXCenter = pStaticBuffer->BoundingBox.vcCenter;
		newRenderBatch.m_MatWorld = matWorld;
		newRenderBatch.m_BatchConfig = config;
		newRenderBatch.m_pSB = pStaticBuffer;
		newRenderBatch.m_pVC = NULL;
		newRenderBatch.m_TBOID = nTBOID;
		newRenderBatch.BuildQueueValue();
		GetDevice()->getOpenGLMaterialRenderer()->PushSkinAnimRenderBatch(newRenderBatch);
	}
	else
		RenderStaticBuffer(nSBufferID, matWorld, config);

}

/**
 * Create a texture buffer object for the given Model Resource
 * return a index to that buffer for later rendering processes.
 * -> IN:  uint32        - index of Model Resource index of m_MF1Models Array in ModelManager
 * -> OUT: uint32        - ID to the created TBO (0xFFFFFFFF for error)
 */
uint32 COpenGLVertexCacheManager::CreateTextureBufferObjectByID(uint32 ModelResourceID)
{
	CMF1FileResource* pMF1Res =	GetDevice()->GetModelManager()->getModelByID(ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
	{
		uint32 TBOID = m_pTBO.size();
		if( pMF1Res->pModelMF1->m_iNumBones > 0 )
		{
			COpenGLTextureBufferObject *pTBO = new COpenGLTextureBufferObject(GetDevice());
			pTBO->createTBO();
			pTBO->bindTBO();
			pTBO->initTBOBuffer(pMF1Res->pModelMF1->m_iNumBones, GL_STREAM_DRAW);


			m_pTBO.add( pTBO );
			return TBOID;
		}
	}

	return 0xFFFFFFFF;
}

/**
 * Update a texture buffer object for the given TBO ID
 * -> IN:   float*	- BoneMatrix Buffer data (updated data for TBO)
 *			uint32  - Bone Count 
 *			uint32  - index of TBO ID
 */
void COpenGLVertexCacheManager::UpdateTextureBufferObjectByID(float* pBoneMatrixBuffer, uint32 nBoneCount, uint32 TBOID)
{
	if( TBOID == 0xFFFFFFFF || !pBoneMatrixBuffer )
		return;

	COpenGLTextureBufferObject *pTBO = m_pTBO.getReference(TBOID);

	if( pTBO )
	{
		pTBO->bindTBO();

		float* MatrixData = (float*)pTBO->mapBufferToMemory(GL_WRITE_ONLY);
			memcpy(MatrixData, pBoneMatrixBuffer, sizeof(float)*12*nBoneCount);
		pTBO->unmapBuffer();
  
		pTBO->unbindTBO();
	}
}


/**
 * Bind a texture buffer object for the given TBO ID to one texture unit
 * -> IN:   
 *			uint32  - index of TBO ID
 *			int		- texture unit to bind texture to
 * -> OUT:  bool	- if no TBOID or texture unit is invalide return false
 */
bool COpenGLVertexCacheManager::BindTextureBufferObjectByID(uint32 TBOID, int iTextureUnit)
{
	if( TBOID == 0xFFFFFFFF )
		return false;

	COpenGLTextureBufferObject *pTBO = m_pTBO.getReference(TBOID);

	return pTBO->bindTextureBuffer(iTextureUnit);
}




void COpenGLVertexCacheManager::RenderPoints( uint32 nVertexNum, const SGPVertex_UPOS_VERTEXCOLOR *pVerts )
{
	SGPSkin pointSkin;
	pointSkin.bAlpha = false;
	pointSkin.nShaderType = SGPST_VERTEXCOLOR;
	pointSkin.nPrimitiveType = SGPPT_POINTS;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, pointSkin, nVertexNum, 0, pVerts, NULL);
	
}

void COpenGLVertexCacheManager::RenderLines( uint32	nVertexNum,
	const SGPVertex_UPOS_VERTEXCOLOR *pVerts, bool bStrip )
{
	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;	
	lineSkin.nPrimitiveType = bStrip ? SGPPT_LINE_STRIP : SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, nVertexNum, 0, pVerts, NULL);
}

void COpenGLVertexCacheManager::RenderLine(	const float  *fStart,
											const float  *fEnd, 
											const Colour *pColor )
{
	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;	
	lineSkin.nPrimitiveType = SGPPT_LINES;

	SGPVertex_UPOS_VERTEXCOLOR Vert[2];
	Vert[0].x = fStart[0];
	Vert[0].y = fStart[1];
	Vert[0].z = fStart[2];
	Vert[1].x = fEnd[0];
	Vert[1].y = fEnd[1];
	Vert[1].z = fEnd[2];
	Vert[0].VertexColor[0] = Vert[1].VertexColor[0] = pColor->getFloatRed();
	Vert[0].VertexColor[1] = Vert[1].VertexColor[1] = pColor->getFloatGreen();
	Vert[0].VertexColor[2] = Vert[1].VertexColor[2] = pColor->getFloatBlue();
	Vert[0].VertexColor[3] = Vert[1].VertexColor[3] = pColor->getFloatAlpha();

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, 2, 0, &Vert, NULL);

}

void COpenGLVertexCacheManager::RenderTriangles( uint32 nVertexNum,
	uint32 nIndexNum, const void *pVerts, const uint16 *pIndis,	bool bStrip )
{
	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;
	lineSkin.nPrimitiveType = bStrip ? SGPPT_LINE_STRIP : SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, nVertexNum, nIndexNum, pVerts, pIndis);
}

void COpenGLVertexCacheManager::FillTriangles( SGP_VERTEX_TYPE VertexType,
	uint32 nVertexNum, uint32 nIndexNum, const void	*pVerts, const uint16 *pIndis,
	const SGPSkin& skin )
{
	RenderDynamicBuffer( VertexType, skin, nVertexNum, nIndexNum, pVerts, pIndis);
}




void COpenGLVertexCacheManager::RenderBox( const AABBox& aabbox, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[8];
	uint16 ib[24] = { 0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	vb[0].x = aabbox.vcMin.x;	vb[0].y = aabbox.vcMin.y;	vb[0].z = aabbox.vcMin.z;
	vb[0].VertexColor[0] = color_r;	vb[0].VertexColor[1] = color_g;	vb[0].VertexColor[2] = color_b;	vb[0].VertexColor[3] = color_a;
	vb[1].x = aabbox.vcMax.x;	vb[1].y = aabbox.vcMin.y;	vb[1].z = aabbox.vcMin.z;
	vb[1].VertexColor[0] = color_r;	vb[1].VertexColor[1] = color_g;	vb[1].VertexColor[2] = color_b;	vb[1].VertexColor[3] = color_a;
	vb[2].x = aabbox.vcMin.x;	vb[2].y = aabbox.vcMax.y;	vb[2].z = aabbox.vcMin.z;
	vb[2].VertexColor[0] = color_r;	vb[2].VertexColor[1] = color_g;	vb[2].VertexColor[2] = color_b;	vb[2].VertexColor[3] = color_a;
	vb[3].x = aabbox.vcMax.x;	vb[3].y = aabbox.vcMax.y;	vb[3].z = aabbox.vcMin.z;
	vb[3].VertexColor[0] = color_r;	vb[3].VertexColor[1] = color_g;	vb[3].VertexColor[2] = color_b;	vb[3].VertexColor[3] = color_a;
	vb[4].x = aabbox.vcMin.x;	vb[4].y = aabbox.vcMin.y;	vb[4].z = aabbox.vcMax.z;	vb[4].VertexColor[0] = color_r;
	vb[4].VertexColor[1] = color_g;	vb[4].VertexColor[2] = color_b;	vb[4].VertexColor[3] = color_a;	
	vb[5].x = aabbox.vcMax.x;	vb[5].y = aabbox.vcMin.y;	vb[5].z = aabbox.vcMax.z;
	vb[5].VertexColor[0] = color_r;	vb[5].VertexColor[1] = color_g;	vb[5].VertexColor[2] = color_b;	vb[5].VertexColor[3] = color_a;
	vb[6].x = aabbox.vcMin.x;	vb[6].y = aabbox.vcMax.y;	vb[6].z = aabbox.vcMax.z;
	vb[6].VertexColor[0] = color_r;	vb[6].VertexColor[1] = color_g;	vb[6].VertexColor[2] = color_b;	vb[6].VertexColor[3] = color_a;
	vb[7].x = aabbox.vcMax.x;	vb[7].y = aabbox.vcMax.y;	vb[7].z = aabbox.vcMax.z;
	vb[7].VertexColor[0] = color_r;	vb[7].VertexColor[1] = color_g;	vb[7].VertexColor[2] = color_b;	vb[7].VertexColor[3] = color_a;

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;	
	lineSkin.nPrimitiveType = SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, 8, 24, vb, ib);
}

void COpenGLVertexCacheManager::RenderBox( const OBBox& obbox, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[8];
	uint16 ib[24] = { 0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
	
	Vector3D rx = obbox.vcA0 * obbox.fA0;
	Vector3D ry = obbox.vcA1 * obbox.fA1;
	Vector3D rz = obbox.vcA2 * obbox.fA2;
	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	vb[0].x = (obbox.vcCenter - rx - ry - rz).x;
	vb[0].y = (obbox.vcCenter - rx - ry - rz).y;
	vb[0].z = (obbox.vcCenter - rx - ry - rz).z;
	vb[0].VertexColor[0] = color_r;	vb[0].VertexColor[1] = color_g;	vb[0].VertexColor[2] = color_b;	vb[0].VertexColor[3] = color_a;
	vb[1].x = (obbox.vcCenter + rx - ry - rz).x;
	vb[1].y = (obbox.vcCenter + rx - ry - rz).y;
	vb[1].z = (obbox.vcCenter + rx - ry - rz).z;
	vb[1].VertexColor[0] = color_r;	vb[1].VertexColor[1] = color_g;	vb[1].VertexColor[2] = color_b;	vb[1].VertexColor[3] = color_a;
	vb[2].x = (obbox.vcCenter - rx + ry - rz).x;
	vb[2].y = (obbox.vcCenter - rx + ry - rz).y;
	vb[2].z = (obbox.vcCenter - rx + ry - rz).z;
	vb[2].VertexColor[0] = color_r;	vb[2].VertexColor[1] = color_g;	vb[2].VertexColor[2] = color_b;	vb[2].VertexColor[3] = color_a;
	vb[3].x = (obbox.vcCenter + rx + ry - rz).x;
	vb[3].y = (obbox.vcCenter + rx + ry - rz).y;
	vb[3].z = (obbox.vcCenter + rx + ry - rz).z;
	vb[3].VertexColor[0] = color_r;	vb[3].VertexColor[1] = color_g;	vb[3].VertexColor[2] = color_b;	vb[3].VertexColor[3] = color_a;
	vb[4].x = (obbox.vcCenter - rx - ry + rz).x;
	vb[4].y = (obbox.vcCenter - rx - ry + rz).y;
	vb[4].z = (obbox.vcCenter - rx - ry + rz).z;
	vb[4].VertexColor[0] = color_r;	vb[4].VertexColor[1] = color_g;	vb[4].VertexColor[2] = color_b;	vb[4].VertexColor[3] = color_a;
	vb[5].x = (obbox.vcCenter + rx - ry + rz).x;
	vb[5].y = (obbox.vcCenter + rx - ry + rz).y;
	vb[5].z = (obbox.vcCenter + rx - ry + rz).z;
	vb[5].VertexColor[0] = color_r;	vb[5].VertexColor[1] = color_g;	vb[5].VertexColor[2] = color_b;	vb[5].VertexColor[3] = color_a;
	vb[6].x = (obbox.vcCenter - rx + ry + rz).x;
	vb[6].y = (obbox.vcCenter - rx + ry + rz).y;
	vb[6].z = (obbox.vcCenter - rx + ry + rz).z;
	vb[6].VertexColor[0] = color_r;	vb[6].VertexColor[1] = color_g;	vb[6].VertexColor[2] = color_b;	vb[6].VertexColor[3] = color_a;
	vb[7].x = (obbox.vcCenter + rx + ry + rz).x;
	vb[7].y = (obbox.vcCenter + rx + ry + rz).y;
	vb[7].z = (obbox.vcCenter + rx + ry + rz).z;
	vb[7].VertexColor[0] = color_r;	vb[7].VertexColor[1] = color_g;	vb[7].VertexColor[2] = color_b;	vb[7].VertexColor[3] = color_a;

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;	
	lineSkin.nPrimitiveType = SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, 8, 24, vb, ib);
}

void COpenGLVertexCacheManager::RenderBox( const Vector3D& boxmin, const Vector3D& boxmax, const Colour& vColor )
{
	RenderBox( AABBox(boxmin, boxmax), vColor );
}

void COpenGLVertexCacheManager::FillBox( const AABBox& aabbox, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[8];
	uint16 ib[36] = { 0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 5, 1, 0, 4, 5, 2, 3, 7, 2, 7, 6, 1, 7, 3, 1, 5, 7, 0, 2, 4, 2, 6, 4 };

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	vb[0].x = aabbox.vcMin.x;	vb[0].y = aabbox.vcMin.y;	vb[0].z = aabbox.vcMin.z;
	vb[0].VertexColor[0] = color_r;	vb[0].VertexColor[1] = color_g;	vb[0].VertexColor[2] = color_b;	vb[0].VertexColor[3] = color_a;
	vb[1].x = aabbox.vcMax.x;	vb[1].y = aabbox.vcMin.y;	vb[1].z = aabbox.vcMin.z;
	vb[1].VertexColor[0] = color_r;	vb[1].VertexColor[1] = color_g;	vb[1].VertexColor[2] = color_b;	vb[1].VertexColor[3] = color_a;
	vb[2].x = aabbox.vcMin.x;	vb[2].y = aabbox.vcMax.y;	vb[2].z = aabbox.vcMin.z;
	vb[2].VertexColor[0] = color_r;	vb[2].VertexColor[1] = color_g;	vb[2].VertexColor[2] = color_b;	vb[2].VertexColor[3] = color_a;
	vb[3].x = aabbox.vcMax.x;	vb[3].y = aabbox.vcMax.y;	vb[3].z = aabbox.vcMin.z;
	vb[3].VertexColor[0] = color_r;	vb[3].VertexColor[1] = color_g;	vb[3].VertexColor[2] = color_b;	vb[3].VertexColor[3] = color_a;
	vb[4].x = aabbox.vcMin.x;	vb[4].y = aabbox.vcMin.y;	vb[4].z = aabbox.vcMax.z;	vb[4].VertexColor[0] = color_r;
	vb[4].VertexColor[1] = color_g;	vb[4].VertexColor[2] = color_b;	vb[4].VertexColor[3] = color_a;	
	vb[5].x = aabbox.vcMax.x;	vb[5].y = aabbox.vcMin.y;	vb[5].z = aabbox.vcMax.z;
	vb[5].VertexColor[0] = color_r;	vb[5].VertexColor[1] = color_g;	vb[5].VertexColor[2] = color_b;	vb[5].VertexColor[3] = color_a;
	vb[6].x = aabbox.vcMin.x;	vb[6].y = aabbox.vcMax.y;	vb[6].z = aabbox.vcMax.z;
	vb[6].VertexColor[0] = color_r;	vb[6].VertexColor[1] = color_g;	vb[6].VertexColor[2] = color_b;	vb[6].VertexColor[3] = color_a;
	vb[7].x = aabbox.vcMax.x;	vb[7].y = aabbox.vcMax.y;	vb[7].z = aabbox.vcMax.z;
	vb[7].VertexColor[0] = color_r;	vb[7].VertexColor[1] = color_g;	vb[7].VertexColor[2] = color_b;	vb[7].VertexColor[3] = color_a;

	SGPSkin boxSkin;
	boxSkin.bAlpha = false;
	boxSkin.nShaderType = SGPST_VERTEXCOLOR;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, boxSkin, 8, 36, vb, ib);
}

void COpenGLVertexCacheManager::FillBox( const OBBox& obbox, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[8];
	uint16 ib[36] = { 0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 5, 1, 0, 4, 5, 2, 3, 7, 2, 7, 6, 1, 7, 3, 1, 5, 7, 0, 2, 4, 2, 6, 4 };
	
	Vector3D rx = obbox.vcA0 * obbox.fA0;
	Vector3D ry = obbox.vcA1 * obbox.fA1;
	Vector3D rz = obbox.vcA2 * obbox.fA2;
	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	vb[0].x = (obbox.vcCenter - rx - ry - rz).x;
	vb[0].y = (obbox.vcCenter - rx - ry - rz).y;
	vb[0].z = (obbox.vcCenter - rx - ry - rz).z;
	vb[0].VertexColor[0] = color_r;	vb[0].VertexColor[1] = color_g;	vb[0].VertexColor[2] = color_b;	vb[0].VertexColor[3] = color_a;
	vb[1].x = (obbox.vcCenter + rx - ry - rz).x;
	vb[1].y = (obbox.vcCenter + rx - ry - rz).y;
	vb[1].z = (obbox.vcCenter + rx - ry - rz).z;
	vb[1].VertexColor[0] = color_r;	vb[1].VertexColor[1] = color_g;	vb[1].VertexColor[2] = color_b;	vb[1].VertexColor[3] = color_a;
	vb[2].x = (obbox.vcCenter - rx + ry - rz).x;
	vb[2].y = (obbox.vcCenter - rx + ry - rz).y;
	vb[2].z = (obbox.vcCenter - rx + ry - rz).z;
	vb[2].VertexColor[0] = color_r;	vb[2].VertexColor[1] = color_g;	vb[2].VertexColor[2] = color_b;	vb[2].VertexColor[3] = color_a;
	vb[3].x = (obbox.vcCenter + rx + ry - rz).x;
	vb[3].y = (obbox.vcCenter + rx + ry - rz).y;
	vb[3].z = (obbox.vcCenter + rx + ry - rz).z;
	vb[3].VertexColor[0] = color_r;	vb[3].VertexColor[1] = color_g;	vb[3].VertexColor[2] = color_b;	vb[3].VertexColor[3] = color_a;
	vb[4].x = (obbox.vcCenter - rx - ry + rz).x;
	vb[4].y = (obbox.vcCenter - rx - ry + rz).y;
	vb[4].z = (obbox.vcCenter - rx - ry + rz).z;
	vb[4].VertexColor[0] = color_r;	vb[4].VertexColor[1] = color_g;	vb[4].VertexColor[2] = color_b;	vb[4].VertexColor[3] = color_a;
	vb[5].x = (obbox.vcCenter + rx - ry + rz).x;
	vb[5].y = (obbox.vcCenter + rx - ry + rz).y;
	vb[5].z = (obbox.vcCenter + rx - ry + rz).z;
	vb[5].VertexColor[0] = color_r;	vb[5].VertexColor[1] = color_g;	vb[5].VertexColor[2] = color_b;	vb[5].VertexColor[3] = color_a;
	vb[6].x = (obbox.vcCenter - rx + ry + rz).x;
	vb[6].y = (obbox.vcCenter - rx + ry + rz).y;
	vb[6].z = (obbox.vcCenter - rx + ry + rz).z;
	vb[6].VertexColor[0] = color_r;	vb[6].VertexColor[1] = color_g;	vb[6].VertexColor[2] = color_b;	vb[6].VertexColor[3] = color_a;
	vb[7].x = (obbox.vcCenter + rx + ry + rz).x;
	vb[7].y = (obbox.vcCenter + rx + ry + rz).y;
	vb[7].z = (obbox.vcCenter + rx + ry + rz).z;
	vb[7].VertexColor[0] = color_r;	vb[7].VertexColor[1] = color_g;	vb[7].VertexColor[2] = color_b;	vb[7].VertexColor[3] = color_a;

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;	

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, 8, 36, vb, ib);
}

void COpenGLVertexCacheManager::FillBox( const Vector3D& boxmin, const Vector3D& boxmax, const Colour& vColor )
{
	FillBox( AABBox(boxmin, boxmax), vColor );
}

void COpenGLVertexCacheManager::RenderCircle( const Vector3D& center, float radius, int axis, const Colour& vColor )
{
	RenderEllipse( center, Vector3D(radius, radius, radius), axis, vColor );
}

void COpenGLVertexCacheManager::RenderCircle( const Vector3D& center, float radius, const Vector3D& axis, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[CIRCLE_SLIDES_MAX+1];
	uint16 ib[CIRCLE_SLIDES_MAX*2];

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	Vector3D ax(1, 0, 0);
	Vector3D ay;
	Vector3D az = axis;

	az.Normalize();
	ay.Cross( az, ax );
	if( ay.GetLengthSquared() < 0.00001f )
	{
		ax.Set(0, 1, 0);
		ay.Cross( az, ax );
	}
	ay.Normalize();
	ax.Cross( ay, az );
	ax *= radius;
	ay *= radius;

	uint16 i;
	for(i=0; i<CIRCLE_SLIDES_MAX; i++)
	{
		Vector3D p = center + ay * m_fCos[i] + ax * m_fSin[i];
		vb[i].x = p.x; vb[i].y = p.y; vb[i].z = p.z;
		vb[i].VertexColor[0] = color_r;	vb[i].VertexColor[1] = color_g;	vb[i].VertexColor[2] = color_b;	vb[i].VertexColor[3] = color_a;

		ib[i*2] = i;
		ib[i*2+1] = i+1;
	}
	vb[i] = vb[0];

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;
	lineSkin.nPrimitiveType = SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, CIRCLE_SLIDES_MAX+1, CIRCLE_SLIDES_MAX*2, vb, ib);
}

void COpenGLVertexCacheManager::RenderEllipse( const Vector3D& center, const Vector3D& radius, int axis, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[CIRCLE_SLIDES_MAX+1];
	uint16 ib[CIRCLE_SLIDES_MAX*2];

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	uint16 i;
	for(i=0; i<CIRCLE_SLIDES_MAX; i++)
	{
		if( axis == 0 )
		{
			vb[i].x = center.x; 
			vb[i].y = center.y + m_fCos[i]*radius.y; 
			vb[i].z = center.z + m_fSin[i]*radius.z;
		}
		else if( axis == 1 )
		{
			vb[i].x = center.x + m_fCos[i]*radius.x; 
			vb[i].y = center.y; 
			vb[i].z = center.z + m_fSin[i]*radius.z;
		}
		else
		{
			vb[i].x = center.x + m_fCos[i]*radius.x; 
			vb[i].y = center.y + m_fSin[i]*radius.y; 
			vb[i].z = center.z;
		}
		vb[i].VertexColor[0] = color_r;	vb[i].VertexColor[1] = color_g;	vb[i].VertexColor[2] = color_b;	vb[i].VertexColor[3] = color_a;

		ib[i*2] = i;
		ib[i*2+1] = i+1;
	}
	vb[i] = vb[0];

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;
	lineSkin.nPrimitiveType = SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, CIRCLE_SLIDES_MAX+1, CIRCLE_SLIDES_MAX*2, vb, ib);
}

void COpenGLVertexCacheManager::RenderEllipse( const Vector3D& center, const Vector3D& xaxis, float rx, const Vector3D& yaxis, float ry, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[CIRCLE_SLIDES_MAX+1];
	uint16 ib[CIRCLE_SLIDES_MAX*2];

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	Vector3D ax = xaxis * rx;
	Vector3D ay = yaxis * ry;


	uint16 i;
	for(i=0; i<CIRCLE_SLIDES_MAX; i++)
	{
		Vector3D p = center + ay * m_fCos[i] + ax * m_fSin[i];
		vb[i].x = p.x; vb[i].y = p.y; vb[i].z = p.z;
		vb[i].VertexColor[0] = color_r;	vb[i].VertexColor[1] = color_g;	vb[i].VertexColor[2] = color_b;	vb[i].VertexColor[3] = color_a;

		ib[i*2] = i;
		ib[i*2+1] = i+1;
	}
	vb[i] = vb[0];

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;
	lineSkin.nPrimitiveType = SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, CIRCLE_SLIDES_MAX+1, CIRCLE_SLIDES_MAX*2, vb, ib);
}

void COpenGLVertexCacheManager::RenderDetailSphere( const Vector3D& center, float radius, int rings, int segments, const Colour& vColor )
{
	//Establish constants used in sphere generation
    float rDeltaRingAngle = (float_Pi / (float)rings);
    float rDeltaSegAngle = (2.0f * float_Pi / (float)segments);

    //Generate the group of rings for the sphere
    for(int nCurrentRing = 0; nCurrentRing < rings + 1; nCurrentRing++)
    {
        float r0 = sinf(nCurrentRing * rDeltaRingAngle);
        float y0 = cosf(nCurrentRing * rDeltaRingAngle);
		RenderCircle( center, radius, Vector3D(r0, y0, 0), vColor );
	}
    //Generate the group of segments for the current ring
    for(int nCurrentSegment = 0; nCurrentSegment < segments + 1; nCurrentSegment++)
    {
        float x0 = sinf(nCurrentSegment * rDeltaSegAngle);
        float z0 = cosf(nCurrentSegment * rDeltaSegAngle);
		RenderCircle( center, radius, Vector3D(x0, 0, z0), vColor );
	}
}

void COpenGLVertexCacheManager::RenderSphere( const Vector3D& center, float radius, const Colour& vColor )
{
	RenderCircle( center, radius, 0, vColor );
	RenderCircle( center, radius, 1, vColor );
	RenderCircle( center, radius, 2, vColor );
}

void COpenGLVertexCacheManager::RenderSphere( const Matrix4x4& matrix, float radius, const Colour& vColor )
{
	RenderCircle( Vector3D(matrix._41, matrix._42, matrix._43), radius, Vector3D(matrix._11, matrix._12, matrix._13), vColor );
	RenderCircle( Vector3D(matrix._41, matrix._42, matrix._43), radius, Vector3D(matrix._21, matrix._22, matrix._23), vColor );
	RenderCircle( Vector3D(matrix._41, matrix._42, matrix._43), radius, Vector3D(matrix._31, matrix._32, matrix._33), vColor );
}


void COpenGLVertexCacheManager::FillSphere( const Vector3D& center, float radius, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[(CIRCLE_SLIDES_MAX+1)*2];
	uint16 ib[CIRCLE_SLIDES_MAX*6] = { 0 };

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	SGPSkin sphereSkin;
	sphereSkin.bAlpha = false;
	sphereSkin.nShaderType = SGPST_VERTEXCOLOR;

	uint16 i,j;
	for( j=0; j<CIRCLE_SLIDES_MAX/2; j++ )
	{
		float z1 = center.z + m_fCos[j] * radius;
		float r1 = m_fSin[j] * radius;
		float z2 = center.z + m_fCos[j+1] * radius;
		float r2 = m_fSin[j+1] * radius;

		for( i=0; i<CIRCLE_SLIDES_MAX; i++ )
		{
			vb[i].x = center.x + m_fSin[i] * r1;
			vb[i].y = center.y + m_fCos[i] * r1;
			vb[i].z = z1;
			vb[i].VertexColor[0] = color_r;	vb[i].VertexColor[1] = color_g;	vb[i].VertexColor[2] = color_b;	vb[i].VertexColor[3] = color_a;
			vb[i+CIRCLE_SLIDES_MAX+1].x = center.x + m_fSin[i] * r2;
			vb[i+CIRCLE_SLIDES_MAX+1].y = center.y + m_fCos[i] * r2;
			vb[i+CIRCLE_SLIDES_MAX+1].z = z2;
			vb[i+CIRCLE_SLIDES_MAX+1].VertexColor[0] = color_r;
			vb[i+CIRCLE_SLIDES_MAX+1].VertexColor[1] = color_g;
			vb[i+CIRCLE_SLIDES_MAX+1].VertexColor[2] = color_b;
			vb[i+CIRCLE_SLIDES_MAX+1].VertexColor[3] = color_a;

			ib[i*6  ] = i;
			ib[i*6+1] = i+CIRCLE_SLIDES_MAX+1;
			ib[i*6+2] = i+1;

			ib[i*6+3] = i+CIRCLE_SLIDES_MAX+1;
			ib[i*6+4] = i+CIRCLE_SLIDES_MAX+2;
			ib[i*6+5] = i+1;
		}

		vb[i] = vb[0];
		vb[CIRCLE_SLIDES_MAX*2+1] = vb[i+1];

		// Only Support SGPVT_UPOS_VERTEXCOLOR Type
		RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, sphereSkin, (CIRCLE_SLIDES_MAX+1)*2, CIRCLE_SLIDES_MAX*6, vb, ib);
	}
}

void COpenGLVertexCacheManager::RenderCylinder( const Vector3D& base, float radius, float height, int segments, const Colour& vColor )
{
	Vector3D start;
	Vector3D end;

    float rDeltaSegAngle = (2.0f * float_Pi / (float)segments);

    for(int nCurrentSegment = 0; nCurrentSegment <= segments; nCurrentSegment++)
    {
        float x0 = radius * sinf(nCurrentSegment * rDeltaSegAngle);
        float z0 = radius * cosf(nCurrentSegment * rDeltaSegAngle);

		start = base + Vector3D(x0, 0, z0);
		end = base + Vector3D(x0, height, z0);
		RenderLine((float*)&start, (float*)&end, &vColor);

		start = base + Vector3D(0,height,0);
		RenderLine((float*)&start, (float*)&end, &vColor);

		start = base;
		end = base + Vector3D(x0,0,z0);
		RenderLine((float*)&start, (float*)&end, &vColor);
    }

	RenderCircle( base, radius, 1, vColor );
	RenderCircle( base + Vector3D(0,height,0), radius, 1, vColor );
}

void COpenGLVertexCacheManager::RenderCylinder( const Matrix4x4& matrix, float radius, float height, int segments, const Colour& vColor )
{
	Vector3D axis = Vector3D(matrix._21, matrix._22, matrix._23);
	Vector3D base = Vector3D(matrix._41, matrix._42, matrix._43);
	Vector3D top = base + axis * height;
	Vector3D side;// = Vector3D(matrix._11, matrix._12, matrix._13) * radius;

	Vector3D start;
	Vector3D end;

	float rDeltaSegAngle = (2.0f * float_Pi / (float)segments);

    for(int nCurrentSegment = 0; nCurrentSegment <= segments; nCurrentSegment++)
    {
        side.x = radius * sinf(nCurrentSegment * rDeltaSegAngle);
        side.z = radius * cosf(nCurrentSegment * rDeltaSegAngle);
		side.y = 0;

		side.RotateWith(matrix);
		start = base + side;
		end = start + axis * height;
		RenderLine((float*)&start, (float*)&end, &vColor);

		start = base + axis * height;
		RenderLine((float*)&start, (float*)&end, &vColor);

		start = base;
		end = base + side;
		RenderLine((float*)&start, (float*)&end, &vColor);
    }

	RenderCircle( base, radius, axis, vColor );
	RenderCircle( top, radius, axis, vColor );
}

void COpenGLVertexCacheManager::RenderCone( const Vector3D& base, float radius, float height, int segments, const Colour& vColor )
{
	Vector3D start;
	Vector3D end;

    float rDeltaSegAngle = (2.0f * float_Pi / (float)segments);

    for(int nCurrentSegment = 0; nCurrentSegment <= segments; nCurrentSegment++)
    {
        float x0 = radius * sinf(nCurrentSegment * rDeltaSegAngle);
        float z0 = radius * cosf(nCurrentSegment * rDeltaSegAngle);

		start = base + Vector3D(x0, 0, z0);
		end = base + Vector3D(0, height, 0);
		RenderLine((float*)&start, (float*)&end, &vColor);

		start = base;
		end = base + Vector3D(x0,0,z0);
		RenderLine((float*)&start, (float*)&end, &vColor);
    }

	RenderCircle( base, radius, 1, vColor );

}

void COpenGLVertexCacheManager::RenderCone( const Matrix4x4& matrix, float radius, float height, int segments, const Colour& vColor )
{
	Vector3D axis = Vector3D(matrix._21, matrix._22, matrix._23);
	Vector3D base = Vector3D(matrix._41, matrix._42, matrix._43);
	Vector3D top = base + axis * height;
	Vector3D side;// = Vector3D(matrix._11, matrix._12, matrix._13) * radius;

	Vector3D start;
	Vector3D end;

	float rDeltaSegAngle = (2.0f * float_Pi / (float)segments);

    for(int nCurrentSegment = 0; nCurrentSegment <= segments; nCurrentSegment++)
    {
        side.x = radius * sinf(nCurrentSegment * rDeltaSegAngle);
        side.z = radius * cosf(nCurrentSegment * rDeltaSegAngle);
		side.y = 0;

		side.RotateWith(matrix);
		start = base + side;
		end = base + axis * height;
		RenderLine((float*)&start, (float*)&end, &vColor);

		start = base;
		end = base + side;
		RenderLine((float*)&start, (float*)&end, &vColor);
    }

	RenderCircle( base, radius, axis, vColor );
}

void COpenGLVertexCacheManager::RenderEllipsoid( const Vector3D& center, const Vector3D& radius, const Colour& vColor )
{
	RenderEllipse(center, radius, 0, vColor);
	RenderEllipse(center, radius, 2, vColor);
}

void COpenGLVertexCacheManager::RenderEllipsoid( const Matrix4x4& matrix, const Vector3D& radius, const Colour& vColor )
{
	RenderEllipse(Vector3D(matrix._41, matrix._42, matrix._43), Vector3D(matrix._11, matrix._12, matrix._13), radius.x, Vector3D(matrix._21, matrix._22, matrix._23), radius.y, vColor);
	RenderEllipse(Vector3D(matrix._41, matrix._42, matrix._43), Vector3D(matrix._21, matrix._22, matrix._23), radius.y, Vector3D(matrix._31, matrix._32, matrix._33), radius.z, vColor);
}

void COpenGLVertexCacheManager::RenderCapsule( const Vector3D& p0, const Vector3D& p1, float radius, const Colour& vColor )
{
	Vector3D xaxis = p0 - p1;
	if( xaxis.GetLengthSquared() < 0.00001f )
	{
		RenderSphere(p0, radius, vColor);
		return;
	}
	
	Vector3D yaxis(0,1,0);
	Vector3D zaxis;
	zaxis.Cross(xaxis, yaxis);
	if( zaxis.GetLengthSquared() < 0.00001f )
	{
		yaxis.Set(0, 0, 1);
		zaxis.Cross(xaxis, yaxis);
	}
	zaxis.Normalize();
	yaxis.Cross(zaxis, xaxis);
	yaxis.Normalize();

	Vector3D start = p0 + yaxis * radius;
	Vector3D end = p1 + yaxis * radius;
	RenderLine((float*)&start, (float*)&end, &vColor);

	start = p0 - yaxis * radius;
	end = p1 - yaxis * radius;
	RenderLine((float*)&start, (float*)&end, &vColor);

	RenderCircle( p0, radius, xaxis, vColor );
	RenderCircle( p1, radius, xaxis, vColor );
	RenderCircle( p0, radius, zaxis, vColor );
	RenderCircle( p1, radius, zaxis, vColor );
	RenderCircle( p0, radius, yaxis, vColor );
	RenderCircle( p1, radius, yaxis, vColor );
}

void COpenGLVertexCacheManager::RenderFrustum( const Frustum& f, const Colour& vColor )
{
	SGPVertex_UPOS_VERTEXCOLOR vb[8];
	uint16 ib[24] = { 0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

	float color_r = vColor.getFloatRed();
	float color_g = vColor.getFloatGreen();
	float color_b = vColor.getFloatBlue();
	float color_a = vColor.getFloatAlpha();

	Vector3D CornerPoint;
	CornerPoint = f.getNearLeftUp();
	vb[0].x = CornerPoint.x;	vb[0].y = CornerPoint.y;	vb[0].z = CornerPoint.z;
	vb[0].VertexColor[0] = color_r;	vb[0].VertexColor[1] = color_g;	vb[0].VertexColor[2] = color_b;	vb[0].VertexColor[3] = color_a;
	CornerPoint = f.getNearRightUp();
	vb[1].x = CornerPoint.x;	vb[1].y = CornerPoint.y;	vb[1].z = CornerPoint.z;
	vb[1].VertexColor[0] = color_r;	vb[1].VertexColor[1] = color_g;	vb[1].VertexColor[2] = color_b;	vb[1].VertexColor[3] = color_a;
	CornerPoint = f.getNearLeftDown();
	vb[2].x = CornerPoint.x;	vb[2].y = CornerPoint.y;	vb[2].z = CornerPoint.z;
	vb[2].VertexColor[0] = color_r;	vb[2].VertexColor[1] = color_g;	vb[2].VertexColor[2] = color_b;	vb[2].VertexColor[3] = color_a;
	CornerPoint = f.getNearRightDown();
	vb[3].x = CornerPoint.x;	vb[3].y = CornerPoint.y;	vb[3].z = CornerPoint.z;
	vb[3].VertexColor[0] = color_r;	vb[3].VertexColor[1] = color_g;	vb[3].VertexColor[2] = color_b;	vb[3].VertexColor[3] = color_a;
	
	CornerPoint = f.getFarLeftUp();
	vb[4].x = CornerPoint.x;	vb[4].y = CornerPoint.y;	vb[4].z = CornerPoint.z;
	vb[4].VertexColor[0] = color_r;	vb[4].VertexColor[1] = color_g;	vb[4].VertexColor[2] = color_b;	vb[4].VertexColor[3] = color_a;
	CornerPoint = f.getFarRightUp();
	vb[5].x = CornerPoint.x;	vb[5].y = CornerPoint.y;	vb[5].z = CornerPoint.z;
	vb[5].VertexColor[0] = color_r;	vb[5].VertexColor[1] = color_g;	vb[5].VertexColor[2] = color_b;	vb[5].VertexColor[3] = color_a;
	CornerPoint = f.getFarLeftDown();
	vb[6].x = CornerPoint.x;	vb[6].y = CornerPoint.y;	vb[6].z = CornerPoint.z;
	vb[6].VertexColor[0] = color_r;	vb[6].VertexColor[1] = color_g;	vb[6].VertexColor[2] = color_b;	vb[6].VertexColor[3] = color_a;
	CornerPoint = f.getFarRightDown();
	vb[7].x = CornerPoint.x;	vb[7].y = CornerPoint.y;	vb[7].z = CornerPoint.z;
	vb[7].VertexColor[0] = color_r;	vb[7].VertexColor[1] = color_g;	vb[7].VertexColor[2] = color_b;	vb[7].VertexColor[3] = color_a;

	SGPSkin lineSkin;
	lineSkin.bAlpha = false;
	lineSkin.nShaderType = SGPST_VERTEXCOLOR;	
	lineSkin.nPrimitiveType = SGPPT_LINES;

	// Only Support SGPVT_UPOS_VERTEXCOLOR Type
	RenderDynamicBuffer( SGPVT_UPOS_VERTEXCOLOR, lineSkin, 8, 24, vb, ib);

}

void COpenGLVertexCacheManager::RenderFullScreenQuad()
{
	if( m_pFullScreenQuadVAO )
	{
		ISGPMaterialSystem::MaterialList &Mat_List = GetDevice()->GetMaterialSystem()->GetMaterialList();
		const ISGPMaterialSystem::SGPMaterialInfo &QuadMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_fullscreenquad);
		GetDevice()->getOpenGLMaterialRenderer()->PushMaterial( QuadMaterial_info.m_material, MM_Add );
		GetDevice()->getOpenGLMaterialRenderer()->ComputeMaterialPass();
		GetDevice()->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

		COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(GetDevice()->GetShaderManager());
		pShaderManager->GetGLSLShaderProgram(SGPST_UI)->useProgram();
		pShaderManager->GetGLSLShaderProgram(SGPST_UI)->setShaderUniform("gSampler0", 0);

		m_pFullScreenQuadVAO->bindVAO();
		glDrawElements( GL_TRIANGLES,
						6, 
						GL_UNSIGNED_SHORT,
						(void*)0 );
		m_pFullScreenQuadVAO->unBindVAO();

		GetDevice()->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
		GetDevice()->getOpenGLMaterialRenderer()->PopMaterial();
	}
}
void COpenGLVertexCacheManager::RenderFullScreenQuadWithoutMaterial()
{
	if( m_pFullScreenQuadVAO )
	{		
		m_pFullScreenQuadVAO->bindVAO();
		glDrawElements( GL_TRIANGLES,
						6, 
						GL_UNSIGNED_SHORT,
						(void*)0 );
		m_pFullScreenQuadVAO->unBindVAO();
	}
}
