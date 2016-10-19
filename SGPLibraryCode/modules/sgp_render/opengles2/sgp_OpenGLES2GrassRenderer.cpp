
COpenGLES2RenderDevice* COpenGLES2GrassRenderer::Sorter::m_pRD = NULL;

int COpenGLES2GrassRenderer::Sorter::compareElements( SGPVertex_GRASS_Cluster first, SGPVertex_GRASS_Cluster second ) noexcept
{
	float firstDistance = -(m_pRD->getOpenGLCamera()->m_CameraPos - Vector4D(first.vPosition[0], first.vPosition[1], first.vPosition[2])).GetLengthSquared();
	float secondDistance = -(m_pRD->getOpenGLCamera()->m_CameraPos - Vector4D(second.vPosition[0], second.vPosition[1], second.vPosition[2])).GetLengthSquared();
	if( firstDistance < secondDistance )
		return -1;
	if( firstDistance > secondDistance )
		return 1;
	return 0;
}



COpenGLES2GrassRenderer::COpenGLES2GrassRenderer(COpenGLES2RenderDevice *pRenderDevice)
	:	m_pRenderDevice(pRenderDevice), m_GrassTextureID(0),
		m_vDefaultGrassSize(0.5f, 1.0f, 0.0f),
		m_nGrassClusterVBOID(0), m_nGrassClusterIndexVBOID(0), m_nGrassClusterVAOID(0)
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



	Vector3D OffsetVector(m_vDefaultGrassSize.x, 0, 0);
	Matrix4x4 RotMat;

	for( int i=-1; i<=1; i++ )
	{
		RotMat.RotationY( i * 60.0f * pi_over_180 );
		OffsetVector.RotateWith(RotMat);

		if( i == 0 )
			OffsetVector.Set(m_vDefaultGrassSize.x, 0, 0);

		m_grassVertex[(i+1)*4 + 0].x = -OffsetVector.x;
		m_grassVertex[(i+1)*4 + 0].y = -OffsetVector.y;
		m_grassVertex[(i+1)*4 + 0].z = -OffsetVector.z;
		m_grassVertex[(i+1)*4 + 0].tu = 0; m_grassVertex[(i+1)*4 + 0].tv = 0;
		m_grassVertex[(i+1)*4 + 1].x =  OffsetVector.x;
		m_grassVertex[(i+1)*4 + 1].y =  OffsetVector.y;
		m_grassVertex[(i+1)*4 + 1].z =  OffsetVector.z;
		m_grassVertex[(i+1)*4 + 1].tu = 1; m_grassVertex[(i+1)*4 + 1].tv = 0;
		m_grassVertex[(i+1)*4 + 2].x =  OffsetVector.x;
		m_grassVertex[(i+1)*4 + 2].y =  OffsetVector.y;
		m_grassVertex[(i+1)*4 + 2].z =  OffsetVector.z;
		m_grassVertex[(i+1)*4 + 2].tu = 1; m_grassVertex[(i+1)*4 + 2].tv = 1;
		m_grassVertex[(i+1)*4 + 3].x = -OffsetVector.x;
		m_grassVertex[(i+1)*4 + 3].y = -OffsetVector.y;
		m_grassVertex[(i+1)*4 + 3].z = -OffsetVector.z;
		m_grassVertex[(i+1)*4 + 3].tu = 0; m_grassVertex[(i+1)*4 + 3].tv = 1;
	}

	GLsizei nStride = sizeof(SGPVertex_GRASS_GLES2);


	// create one GrassCluster VAO and VBO
	m_pRenderDevice->extGlGenVertexArray(1, &m_nGrassClusterVAOID);
	m_pRenderDevice->extGlBindVertexArray(m_nGrassClusterVAOID);

	glGenBuffers(1, &m_nGrassClusterVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, m_nGrassClusterVBOID);
	glBufferData(GL_ARRAY_BUFFER, INIT_GRASSCLUSTERINSTANCE_NUM*4*3*nStride, NULL, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(4*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(8*sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, nStride, (GLvoid *)BUFFER_OFFSET(12*sizeof(float)));


	glGenBuffers(1, &m_nGrassClusterIndexVBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nGrassClusterIndexVBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INIT_GRASSCLUSTERINSTANCE_NUM*6*3*sizeof(uint16), NULL, GL_STATIC_DRAW);
	
	uint16* pGrassIndex = (uint16*)m_pRenderDevice->extGlMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
	for( uint16 j=0; j<INIT_GRASSCLUSTERINSTANCE_NUM; j++ )
	{
		for( int k=0; k<6*3; k++ )
			pGrassIndex[ j*(6*3) + k ] = grassIndex[k] + j*(4*3);
	}
	m_pRenderDevice->extGlUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	m_pRenderDevice->extGlBindVertexArray(0);
}

COpenGLES2GrassRenderer::~COpenGLES2GrassRenderer()
{
	// Delete GrassCluster VAO and VBO
	glDeleteBuffers(1, &m_nGrassClusterVBOID);
	glDeleteBuffers(1, &m_nGrassClusterIndexVBOID);
	m_pRenderDevice->extGlDeleteVertexArray(1, &m_nGrassClusterVAOID);

	releaseGrassTexture();
}

void COpenGLES2GrassRenderer::initializeFromGrass(CSGPGrass* pGrass)
{
	releaseGrassTexture();

	createGrassTexture( pGrass->GetGrassTextureName() );
}



void COpenGLES2GrassRenderer::update(float fDeltaTimeInSecond, const Vector4D& camPos, const Frustum& viewFrustum, CSGPGrass* pGrass)
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
	m_vWindDirForce = pGrass->m_vWindDirectionAndStrength;
}

void COpenGLES2GrassRenderer::render(CSGPGrass* pGrass)
{	
	SGPVertex_GRASS_GLES2 tempData;
	if( m_GrassClusterInstanceArray.size() > 0 )
	{
		COpenGLES2GrassRenderer::Sorter CompareGrassInstance(m_pRenderDevice);
		m_GrassClusterInstanceArray.sort(CompareGrassInstance);
		
		uint32 nGrassClusterNum = jmin(m_GrassClusterInstanceArray.size(), INIT_GRASSCLUSTERINSTANCE_NUM);
		uint32 nSizeData = sizeof(SGPVertex_GRASS_GLES2) * nGrassClusterNum * (4*3);
		uint32 nVBOffset = (m_GrassClusterInstanceArray.size() <= INIT_GRASSCLUSTERINSTANCE_NUM) ? 0 : m_GrassClusterInstanceArray.size() - INIT_GRASSCLUSTERINSTANCE_NUM;
		SGPVertex_GRASS_Cluster* pGrassClusterSrc = m_GrassClusterInstanceArray.getRawDataPointer() + nVBOffset;
		
		// update Dynamic Grass Instance Buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_nGrassClusterVBOID);
		glBufferData(GL_ARRAY_BUFFER, nSizeData, NULL, GL_STREAM_DRAW);
		SGPVertex_GRASS_GLES2* pGrassVertex = (SGPVertex_GRASS_GLES2*)m_pRenderDevice->extGlMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);

		for( uint32 i=0; i<nGrassClusterNum; i++ )
		{
			tempData.vNormal[0] = ( pGrassClusterSrc[i].vPackedNormal[0] / 255.0f - 0.5f ) * 2.0f;
			tempData.vNormal[1] = ( pGrassClusterSrc[i].vPackedNormal[1] / 255.0f - 0.5f ) * 2.0f;
			tempData.vNormal[2] = ( pGrassClusterSrc[i].vPackedNormal[2] / 255.0f - 0.5f ) * 2.0f;
			tempData.vNormal[3] = pGrassClusterSrc[i].vPackedNormal[3] / 255.0f;
			tempData.vColor[0] = pGrassClusterSrc[i].vColor[0];
			tempData.vColor[1] = pGrassClusterSrc[i].vColor[1];
			tempData.vColor[2] = pGrassClusterSrc[i].vColor[2];
			tempData.vColor[3] = pGrassClusterSrc[i].vColor[3];
			tempData.LMtu = pGrassClusterSrc[i].vPosition[0] / m_pRenderDevice->GetWorldSystemManager()->getTerrain()->GetTerrainWidth();
			tempData.LMtv = 1.0f - pGrassClusterSrc[i].vPosition[2] / m_pRenderDevice->GetWorldSystemManager()->getTerrain()->GetTerrainWidth();


			Matrix4x4 RotMatrix;
			RotMatrix.Identity();
			if( pGrassClusterSrc[i].vPackedNormal[1] < 250 )
			{
				Vector3D vTerrainNormal(tempData.vNormal[0], tempData.vNormal[1], tempData.vNormal[2]);
				vTerrainNormal.Normalize();

				float cost = tempData.vNormal[1];
				Vector3D v;
				v.Cross( Vector3D(0,1,0), vTerrainNormal );
				v.Normalize();
				float sint = std::sqrt(1-cost*cost);
				float one_sub_cost = 1 - cost;

				RotMatrix._11 = v.x * v.x * one_sub_cost + cost;
				RotMatrix._12 = v.x * v.y * one_sub_cost + v.z * sint;
				RotMatrix._13 = v.x * v.z * one_sub_cost - v.y * sint;
				RotMatrix._21 = v.x * v.y * one_sub_cost - v.z * sint;
				RotMatrix._22 = v.y * v.y * one_sub_cost + cost;
				RotMatrix._23 = v.y * v.z * one_sub_cost + v.x * sint;
				RotMatrix._31 = v.x * v.z * one_sub_cost + v.y * sint;
				RotMatrix._32 = v.y * v.z * one_sub_cost - v.x * sint;
				RotMatrix._33 = v.z * v.z * one_sub_cost + cost;
			}


			for( uint32 j=0; j<4*3; j++ )
			{
				tempData.tu = m_grassVertex[j].tu;
				tempData.tv = m_grassVertex[j].tv;

				Vector3D modelPos = Vector3D(m_grassVertex[j].x, m_grassVertex[j].y, m_grassVertex[j].z) * tempData.vNormal[3] * RotMatrix;
				modelPos += Vector3D(pGrassClusterSrc[i].vPosition[0], pGrassClusterSrc[i].vPosition[1], pGrassClusterSrc[i].vPosition[2]);

				float windAngle = m_vTimeParams.x * m_vTimeParams.y * Vector3D(pGrassClusterSrc[i].vPosition[0], pGrassClusterSrc[i].vPosition[1], pGrassClusterSrc[i].vPosition[2]).GetLength();
				Vector3D vCosSin = Vector3D( std::cos(windAngle), 0, std::sin(windAngle) );
				Vector3D vOffset(	pGrassClusterSrc[i].vWindParams[0] * m_vWindDirForce.x * m_vWindDirForce.w,
									pGrassClusterSrc[i].vWindParams[1] * m_vWindDirForce.y * m_vWindDirForce.w,
									pGrassClusterSrc[i].vWindParams[2] * m_vWindDirForce.z * m_vWindDirForce.w );

				vOffset.x += vCosSin.x * pGrassClusterSrc[i].vWindParams[0] * m_vWindDirForce.w;
				vOffset.y += vCosSin.y * pGrassClusterSrc[i].vWindParams[1] * m_vWindDirForce.w;
				vOffset.z += vCosSin.z * pGrassClusterSrc[i].vWindParams[2] * m_vWindDirForce.w;

				Vector3D vv = Vector3D(tempData.vNormal[0], tempData.vNormal[1], tempData.vNormal[2]) + vOffset;
				vv.Normalize();
				modelPos += vv * tempData.vNormal[3] * (1.0f - tempData.tv);	

				tempData.vPosition[0] = modelPos.x;
				tempData.vPosition[1] = modelPos.y;
				tempData.vPosition[2] = modelPos.z;
				tempData.vPosition[3] = pGrassClusterSrc[i].vPosition[3];

				(*pGrassVertex) = tempData;

				pGrassVertex++;
			}
		}

		m_pRenderDevice->extGlUnmapBuffer(GL_ARRAY_BUFFER);

	}
	
	m_vTextureAtlas.Set( pGrass->m_fTextureAtlasNbX, pGrass->m_fTextureAtlasNbY, pGrass->m_fTextureAtlasW, pGrass->m_fTextureAtlasH );
}

void COpenGLES2GrassRenderer::DoDrawGrass()
{
	if( m_GrassClusterInstanceArray.size() > 0 )
	{
		COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_pRenderDevice->GetShaderManager());
		Matrix4x4 MVP = m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

		ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
		const ISGPMaterialSystem::SGPMaterialInfo &GrassMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_grass);
		m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( GrassMaterial_info.m_material, MM_Add );

		m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
		m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->useProgram();
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("ViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("TextureAtlas", m_vTextureAtlas);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("SunDirection", -m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("SunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());


		m_pRenderDevice->GetTextureManager()->getTextureByID(m_GrassTextureID)->pSGPTexture->BindTexture2D(0);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("gSampler0", 0);
		m_pRenderDevice->GetTextureManager()->getTextureByID( m_pRenderDevice->getOpenGLTerrainRenderer()->getLightmapTextureID() )->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(SGPST_GRASS_INSTANCING)->setShaderUniform("gSamplerLightmap", 1);


		m_pRenderDevice->extGlBindVertexArray(m_nGrassClusterVAOID);

		uint32 indexNum = jmin(m_GrassClusterInstanceArray.size(), INIT_GRASSCLUSTERINSTANCE_NUM) * (6*3);

		glDrawElements( GL_TRIANGLES,
						indexNum,
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pRenderDevice->extGlBindVertexArray(0);

		m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
		m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();
	}
}


void COpenGLES2GrassRenderer::createGrassTexture(const String& GrassTextureName)
{
	// register Grass texture
	m_GrassTextureID = m_pRenderDevice->GetTextureManager()->registerTexture(GrassTextureName);
}

void COpenGLES2GrassRenderer::releaseGrassTexture()
{
	// unregister Grass texture
	if( m_GrassTextureID != 0 )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_GrassTextureID);
	m_GrassTextureID = 0;
}