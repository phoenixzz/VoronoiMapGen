
COpenGLRenderDevice* COpenGLGrassRenderer::Sorter::m_pRD = NULL;

int COpenGLGrassRenderer::Sorter::compareElements( SGPVertex_GRASS_Cluster first, SGPVertex_GRASS_Cluster second ) noexcept
{
	float firstDistance = -(m_pRD->getOpenGLCamera()->m_CameraPos - Vector4D(first.vPosition[0], first.vPosition[1], first.vPosition[2])).GetLengthSquared();
	float secondDistance = -(m_pRD->getOpenGLCamera()->m_CameraPos - Vector4D(second.vPosition[0], second.vPosition[1], second.vPosition[2])).GetLengthSquared();
	if( firstDistance < secondDistance )
		return -1;
	if( firstDistance > secondDistance )
		return 1;
	return 0;
}



COpenGLGrassRenderer::COpenGLGrassRenderer(COpenGLRenderDevice *pRenderDevice)
	:	m_pRenderDevice(pRenderDevice), m_GrassTextureID(0),
		m_vDefaultGrassSize(0.5f, 1.0f, 0.0f),
		m_nGrassClusterVBOID(0), m_nGrassClusterIndexVBOID(0), m_nGrassClusterVAOID(0),
		m_GrassClusterInstanceVBID(0)
{
	m_GrassClusterInstanceArray.ensureStorageAllocated(INIT_GRASSCLUSTERINSTANCE_NUM);

	// Local Grass Cluster Vertex and Index
	/*
	0			1
	|-----------|
	| 			|
	|			|
	|			|
	|-----------|
	3			2
	*/
	uint16 grassIndex[6*3] = { 0, 2, 1, 0, 3, 2,	4, 6, 5, 4, 7, 6,	8, 10, 9, 8, 11, 10 };

	SGPVertex_GRASS grassLocalVertex[4*3];
	Vector3D OffsetVector(m_vDefaultGrassSize.x, 0, 0);
	Matrix4x4 RotMat;

	for( int i=-1; i<=1; i++ )
	{
		RotMat.RotationY( i * 60.0f * pi_over_180 );
		OffsetVector.RotateWith(RotMat);

		if( i == 0 )
			OffsetVector.Set(m_vDefaultGrassSize.x, 0, 0);

		grassLocalVertex[(i+1)*4 + 0].x = -OffsetVector.x;
		grassLocalVertex[(i+1)*4 + 0].y = -OffsetVector.y;
		grassLocalVertex[(i+1)*4 + 0].z = -OffsetVector.z;
		grassLocalVertex[(i+1)*4 + 0].tu = 0; grassLocalVertex[(i+1)*4 + 0].tv = 0;
		grassLocalVertex[(i+1)*4 + 1].x =  OffsetVector.x;
		grassLocalVertex[(i+1)*4 + 1].y =  OffsetVector.y;
		grassLocalVertex[(i+1)*4 + 1].z =  OffsetVector.z;
		grassLocalVertex[(i+1)*4 + 1].tu = 1; grassLocalVertex[(i+1)*4 + 1].tv = 0;
		grassLocalVertex[(i+1)*4 + 2].x =  OffsetVector.x;
		grassLocalVertex[(i+1)*4 + 2].y =  OffsetVector.y;
		grassLocalVertex[(i+1)*4 + 2].z =  OffsetVector.z;
		grassLocalVertex[(i+1)*4 + 2].tu = 1; grassLocalVertex[(i+1)*4 + 2].tv = 1;
		grassLocalVertex[(i+1)*4 + 3].x = -OffsetVector.x;
		grassLocalVertex[(i+1)*4 + 3].y = -OffsetVector.y;
		grassLocalVertex[(i+1)*4 + 3].z = -OffsetVector.z;
		grassLocalVertex[(i+1)*4 + 3].tu = 0; grassLocalVertex[(i+1)*4 + 3].tv = 1;
	}

	GLsizei nStride = sizeof(SGPVertex_GRASS);

	// create one GrassCluster VAO and VBO
	m_pRenderDevice->extGlGenVertexArray(1, &m_nGrassClusterVAOID);
	m_pRenderDevice->extGlBindVertexArray(m_nGrassClusterVAOID);

	m_pRenderDevice->extGlGenBuffers(1, &m_nGrassClusterVBOID);
	m_pRenderDevice->extGlBindBuffer(GL_ARRAY_BUFFER, m_nGrassClusterVBOID);
	m_pRenderDevice->extGlBufferData(GL_ARRAY_BUFFER, 4*3*nStride, grassLocalVertex, GL_STATIC_DRAW);

	m_pRenderDevice->extGlEnableVertexAttribArray(0);
	m_pRenderDevice->extGlVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
	m_pRenderDevice->extGlEnableVertexAttribArray(1);
	m_pRenderDevice->extGlVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(3*sizeof(float)));

	// Instance VertexBuffer
	nStride = sizeof(SGPVertex_GRASS_Cluster);
	m_pRenderDevice->extGlGenBuffers(1, &m_GrassClusterInstanceVBID);
	m_pRenderDevice->extGlBindBuffer(GL_ARRAY_BUFFER, m_GrassClusterInstanceVBID);
	m_pRenderDevice->extGlBufferData(GL_ARRAY_BUFFER, INIT_GRASSCLUSTERINSTANCE_NUM * nStride, NULL, GL_STREAM_DRAW);
    m_pRenderDevice->extGlEnableVertexAttribArray(2);
    m_pRenderDevice->extGlVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
    m_pRenderDevice->extGlVertexAttribDivisor(2, 1);
    m_pRenderDevice->extGlEnableVertexAttribArray(3);
    m_pRenderDevice->extGlVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
    m_pRenderDevice->extGlVertexAttribDivisor(3, 1);
    m_pRenderDevice->extGlEnableVertexAttribArray(4);
    m_pRenderDevice->extGlVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)+4*sizeof(uint8)));
    m_pRenderDevice->extGlVertexAttribDivisor(4, 1);
    m_pRenderDevice->extGlEnableVertexAttribArray(5);
    m_pRenderDevice->extGlVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(8*sizeof(float)+4*sizeof(uint8)));
    m_pRenderDevice->extGlVertexAttribDivisor(5, 1);


	m_pRenderDevice->extGlGenBuffers(1, &m_nGrassClusterIndexVBOID);
	m_pRenderDevice->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nGrassClusterIndexVBOID);
	m_pRenderDevice->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*3*sizeof(uint16), grassIndex, GL_STATIC_DRAW);
	
	m_pRenderDevice->extGlBindVertexArray(0);
}

COpenGLGrassRenderer::~COpenGLGrassRenderer()
{
	// Delete GrassCluster Dynamic instance VB
	m_pRenderDevice->extGlDeleteBuffers(1, &m_GrassClusterInstanceVBID);

	// Delete GrassCluster VAO and VBO
	m_pRenderDevice->extGlDeleteBuffers(1, &m_nGrassClusterVBOID);
	m_pRenderDevice->extGlDeleteBuffers(1, &m_nGrassClusterIndexVBOID);
	m_pRenderDevice->extGlDeleteVertexArray(1, &m_nGrassClusterVAOID);

	releaseGrassTexture();
}

void COpenGLGrassRenderer::initializeFromGrass(CSGPGrass* pGrass)
{
	releaseGrassTexture();

	createGrassTexture( pGrass->GetGrassTextureName() );
}



void COpenGLGrassRenderer::update(float fDeltaTimeInSecond, const Vector4D& camPos, const Frustum& viewFrustum, CSGPGrass* pGrass)
{
	m_vCameraPos = camPos;

	m_GrassClusterInstanceArray.clearQuick();

	if( !pGrass )
		return;

	SGPVertex_GRASS_Cluster tempData;

	CSGPTerrainChunk** pChunkEnd = pGrass->m_TerrainGrassChunks.end();
	for( CSGPTerrainChunk** pChunkStart = pGrass->m_TerrainGrassChunks.begin(); pChunkStart < pChunkEnd; pChunkStart++ )
	{
		if( !m_pRenderDevice->GetWorldSystemManager()->isTerrainChunkVisible( *pChunkStart ) )
			continue;

		for(uint32 i=0; i<(*pChunkStart)->GetGrassClusterDataCount(); i++ )
		{
			// None Flag, skip this Cluster
			uint32 nGrassSetFlag = (*pChunkStart)->GetGrassClusterData()[i].nData;
			if( nGrassSetFlag == 0 )
				continue;

			tempData.vPosition[0] = (*pChunkStart)->GetGrassClusterData()[i].fPositionX;
			tempData.vPosition[1] = (*pChunkStart)->GetGrassClusterData()[i].fPositionY;
			tempData.vPosition[2] = (*pChunkStart)->GetGrassClusterData()[i].fPositionZ;
			tempData.vPosition[3] = float( (nGrassSetFlag & 0x00FF0000) >> 16 );

			// GrassCluster is not inside the camera Frustum, skip this Cluster
			AABBox GrassClusterAABB;
			GrassClusterAABB += Vector3D(tempData.vPosition[0] - m_vDefaultGrassSize.x, tempData.vPosition[1], tempData.vPosition[2] - m_vDefaultGrassSize.x);
			GrassClusterAABB += Vector3D(tempData.vPosition[0] + m_vDefaultGrassSize.x, tempData.vPosition[1] + m_vDefaultGrassSize.y, tempData.vPosition[2] + m_vDefaultGrassSize.x);
			if( !GrassClusterAABB.Intersects(viewFrustum) )
				continue;

			// GrassCluster is too far from the Grass Far Fading distance, skip this Cluster
			float fGrassDis = (m_vCameraPos - Vector4D(tempData.vPosition[0], tempData.vPosition[1], tempData.vPosition[2])).GetLength();
			if( fGrassDis > CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd )
				continue;

			// Too many grass Cluster
			if( m_GrassClusterInstanceArray.size() + 1 > INIT_GRASSCLUSTERINSTANCE_NUM )
				continue;

			tempData.vPackedNormal[0] = (uint8)(((*pChunkStart)->GetGrassClusterData()[i].nPackedNormal & 0xFF000000) >> 24);
			tempData.vPackedNormal[1] = (uint8)(((*pChunkStart)->GetGrassClusterData()[i].nPackedNormal & 0x00FF0000) >> 16);
			tempData.vPackedNormal[2] = (uint8)(((*pChunkStart)->GetGrassClusterData()[i].nPackedNormal & 0x0000FF00) >> 8);
			tempData.vPackedNormal[3] = (uint8)((nGrassSetFlag & 0xFF000000) >> 24);

			tempData.vColor[0] = tempData.vColor[1] = tempData.vColor[2] = 1.0f;
			tempData.vColor[3] = 1.0f - jlimit(0.0f, 1.0f, (fGrassDis - CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart) / (CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd - CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart));
			
			tempData.vWindParams[0] = ((nGrassSetFlag & 0x0000FF00) >> 8) / 255.0f;
			tempData.vWindParams[1] = 0.0f;
			tempData.vWindParams[2] = (nGrassSetFlag & 0x000000FF) / 255.0f;
			tempData.vWindParams[3] = 0.0f;

			m_GrassClusterInstanceArray.add( tempData );
		}
	}

	// update grass rendering params
	m_vTimeParams.x += fDeltaTimeInSecond;
	m_vTimeParams.y = pGrass->m_fGrassPeriod;
	m_vLightMapTextureDimision.Set(
		1.0f / m_pRenderDevice->GetWorldSystemManager()->getTerrain()->GetTerrainWidth(),
		1.0f / m_pRenderDevice->GetWorldSystemManager()->getTerrain()->GetTerrainWidth() );
	m_vWindDirForce = pGrass->m_vWindDirectionAndStrength;
}

void COpenGLGrassRenderer::render(CSGPGrass* pGrass)
{
	if( m_GrassClusterInstanceArray.size() > 0 )
	{
		COpenGLGrassRenderer::Sorter CompareGrassInstance(m_pRenderDevice);
		m_GrassClusterInstanceArray.sort(CompareGrassInstance);

		uint32 nSizeData = sizeof(SGPVertex_GRASS_Cluster) * m_GrassClusterInstanceArray.size();

		// update Dynamic Instance Buffer
		m_pRenderDevice->extGlBindBuffer(GL_ARRAY_BUFFER, m_GrassClusterInstanceVBID);
		SGPVertex_GRASS_Cluster* pData = (SGPVertex_GRASS_Cluster*)m_pRenderDevice->extGlMapBufferRange(GL_ARRAY_BUFFER, 0, nSizeData, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		// copy data into the buffer
		memcpy(pData, m_GrassClusterInstanceArray.getRawDataPointer(), nSizeData);
		m_pRenderDevice->extGlUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	m_vTextureAtlas.Set( pGrass->m_fTextureAtlasNbX, pGrass->m_fTextureAtlasNbY, pGrass->m_fTextureAtlasW, pGrass->m_fTextureAtlasH );
}

void COpenGLGrassRenderer::DoDrawGrassInstance()
{
	if( m_GrassClusterInstanceArray.size() > 0 )
	{
		COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());
		Matrix4x4 MVP = m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

		ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
		const ISGPMaterialSystem::SGPMaterialInfo &GrassMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_grass);
		m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( GrassMaterial_info.m_material, MM_Add );

		m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
		m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->useProgram();
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("ViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("TextureAtlas", m_vTextureAtlas);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("vTimeParams", m_vTimeParams);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("vLightMapTextureDimision", m_vLightMapTextureDimision);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("vWindDirAndStrength", m_vWindDirForce);
		//pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("fFarPlane", m_pRenderDevice->getOpenGLCamera()->m_fFar);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());


		m_pRenderDevice->GetTextureManager()->getTextureByID(m_GrassTextureID)->pSGPTexture->BindTexture2D(0);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("gSampler0", 0);
		m_pRenderDevice->GetTextureManager()->getTextureByID( m_pRenderDevice->getOpenGLTerrainRenderer()->getLightmapTextureID() )->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("gSamplerLightmap", 1);


		m_pRenderDevice->extGlBindVertexArray(m_nGrassClusterVAOID);

		m_pRenderDevice->extGlDrawElementsInstanced( GL_TRIANGLES,
						6*3, 
						GL_UNSIGNED_SHORT,
						(void*)0,
						m_GrassClusterInstanceArray.size() );
	
		m_pRenderDevice->extGlBindVertexArray(0);

		m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
		m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();
	}
}


void COpenGLGrassRenderer::createGrassTexture(const String& GrassTextureName)
{
	// register Grass texture
	m_GrassTextureID = m_pRenderDevice->GetTextureManager()->registerTexture(GrassTextureName);
}

void COpenGLGrassRenderer::releaseGrassTexture()
{
	// unregister Grass texture
	if( m_GrassTextureID != 0 )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_GrassTextureID);
	m_GrassTextureID = 0;
}