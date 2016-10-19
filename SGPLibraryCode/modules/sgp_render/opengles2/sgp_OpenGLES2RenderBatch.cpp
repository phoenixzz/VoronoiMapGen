int32 ISGPRenderBatch::m_ShaderProgramIdx = -1;
bool ISGPRenderBatch::m_SkinAnimShaderLightmapTexSetted = false;

void ISGPRenderBatch::PreRender()
{
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());

	if( m_pSB )
	{
		if( m_ShaderProgramIdx != m_pSB->MaterialSkin.nShaderType )
		{
			pShaderManager->GetGLSLShaderProgram(m_pSB->MaterialSkin.nShaderType)->useProgram();
			m_ShaderProgramIdx = m_pSB->MaterialSkin.nShaderType;
		}
		//	m_pSB->MaterialModifier.m_material->PreRender(0);
	}
	else if( m_pVC )
	{
		if( m_ShaderProgramIdx != m_pVC->m_MaterialSkin.nShaderType )
		{
			pShaderManager->GetGLSLShaderProgram(m_pVC->m_MaterialSkin.nShaderType)->useProgram();
			m_ShaderProgramIdx = m_pVC->m_MaterialSkin.nShaderType;
		}
		//	m_pVC->m_MaterialModifier.m_material->PreRender(0);
	}
}

void ISGPRenderBatch::PostRender()
{
	//if( m_pSB )
	//{
	//	m_pSB->MaterialModifier.m_material->PostRender(0);
	//}
	//else if( m_pVC )
	//{
	//	m_pVC->m_MaterialModifier.m_material->PostRender(0);
	//}
}

void ISGPRenderBatch::Render()
{
	int ShaderProgramIdx = 0;
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	
	Matrix4x4 MVP;
	if( m_RenderDevice->getCurrentRenderStage() == SGPRS_WATERREFLECTION )
		MVP = m_MatWorld * m_RenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_RenderDevice->getOpenGLWaterRenderer()->m_ObliqueNearPlaneReflectionProjMatrix;
	else
		MVP = m_MatWorld * m_RenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

	if( m_pSB )
	{
		ShaderProgramIdx = m_pSB->MaterialSkin.nShaderType;
		//pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->useProgram();
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("SunColor", m_RenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());
		
		switch( m_pSB->MaterialSkin.nTextureNum )
		{
		case 2:
			// Lightmap texture
			m_RenderDevice->GetTextureManager()->getTextureByID(m_BatchConfig.m_nLightMapTextureID)->pSGPTexture->BindTexture2D(1);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler1", 1);			
		
		case 1:
			// Diffuse texture
			if( m_BatchConfig.m_nReplacedTextureID != 0 )
				m_RenderDevice->GetTextureManager()->getTextureByID(m_BatchConfig.m_nReplacedTextureID)->pSGPTexture->BindTexture2D(0);
			else
				m_RenderDevice->GetTextureManager()->getTextureByID(m_pSB->MaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler0", 0);

		// Texture coord UV anim and Material Diffuse color
		{
			Vector4D TextureAtlas((float)m_pSB->MaterialSkin.vUVTile[0], (float)m_pSB->MaterialSkin.vUVTile[1], 1.0f/m_pSB->MaterialSkin.vUVTile[0], 1.0f/m_pSB->MaterialSkin.vUVTile[1]);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TextureAtlas", TextureAtlas);

			uint32 nTileFrameID = m_pSB->MaterialSkin.vUVTile[2] + uint32( m_BatchConfig.m_fTimePassedFromCreated / (m_pSB->MaterialSkin.vUVTile[4] * 0.001f) );
			uint32 nUVTileNum = m_pSB->MaterialSkin.vUVTile[0] * m_pSB->MaterialSkin.vUVTile[1];
			switch( m_pSB->MaterialSkin.vUVTile[3] )	
			{
			case 0:		// Loop
				nTileFrameID %= nUVTileNum;
				break;
			case 1:		// Ping Pong
				nTileFrameID %= (nUVTileNum * 2);
				nTileFrameID = (nTileFrameID < nUVTileNum) ? nTileFrameID : (2*nUVTileNum-nTileFrameID-1);
				break;
			case 2:		// Hold
				if( nTileFrameID >= nUVTileNum )
					nTileFrameID = nUVTileNum - 1;
				break;
			default:	// Loop
				nTileFrameID %= nUVTileNum;
				break;
			}

			Vector2D UVOffset = m_pSB->MaterialSkin.getMatKeyFrameUVOffset(m_BatchConfig.m_fTimePassedFromCreated);
			Vector3D TexIndexUVOffset(
				(float)nTileFrameID, 
				UVOffset.x + m_pSB->MaterialSkin.vUVSpeed[0] * m_BatchConfig.m_fTimePassedFromCreated,
				UVOffset.y + m_pSB->MaterialSkin.vUVSpeed[1] * m_BatchConfig.m_fTimePassedFromCreated);
			
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TexIndexUVOffset", TexIndexUVOffset);
		}

		default:
			break;
		}

		Vector4D DiffColor = m_pSB->MaterialSkin.getMatKeyFrameColor(m_BatchConfig.m_fTimePassedFromCreated);
		DiffColor.w = m_BatchConfig.m_fBatchAlpha;
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("vMaterialColor", DiffColor);


		m_pSB->pVBO->bindVAO();

		glDrawElements( m_RenderDevice->primitiveTypeToGL( m_pSB->MaterialSkin.nPrimitiveType ),
						m_pSB->nNumIndis, 
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pSB->pVBO->unBindVAO();
	}
	else if( m_pVC )
	{
		ShaderProgramIdx = m_pVC->m_MaterialSkin.nShaderType;
		//pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->useProgram();
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("SunColor", m_RenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());

		switch( m_pVC->m_MaterialSkin.nTextureNum )
		{
		//case 2:
		//	m_RenderDevice->GetTextureManager()->getTextureByID(m_pVC->m_MaterialSkin.nTextureID[1])->pSGPTexture->BindTexture2D(1);
		//	pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler1", 1);			
	
		case 1:
			// Diffuse texture
			if( m_BatchConfig.m_nReplacedTextureID != 0 )
				m_RenderDevice->GetTextureManager()->getTextureByID(m_BatchConfig.m_nReplacedTextureID)->pSGPTexture->BindTexture2D(0);
			else
				m_RenderDevice->GetTextureManager()->getTextureByID(m_pVC->m_MaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler0", 0);

		// Texture coord UV anim
		{
			Vector4D TextureAtlas((float)m_pVC->m_MaterialSkin.vUVTile[0], (float)m_pVC->m_MaterialSkin.vUVTile[1], 1.0f/m_pVC->m_MaterialSkin.vUVTile[0], 1.0f/m_pVC->m_MaterialSkin.vUVTile[1]);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TextureAtlas", TextureAtlas);

			uint32 nTileFrameID = m_pVC->m_MaterialSkin.vUVTile[2] + uint32( m_BatchConfig.m_fTimePassedFromCreated / (m_pVC->m_MaterialSkin.vUVTile[4] * 0.001f) );
			uint32 nUVTileNum = m_pVC->m_MaterialSkin.vUVTile[0] * m_pVC->m_MaterialSkin.vUVTile[1];
			switch( m_pVC->m_MaterialSkin.vUVTile[3] )	
			{
			case 0:		// Loop
				nTileFrameID %= nUVTileNum;
				break;
			case 1:		// Ping Pong
				nTileFrameID %= (nUVTileNum * 2);
				nTileFrameID = (nTileFrameID < nUVTileNum) ? nTileFrameID : (2*nUVTileNum-nTileFrameID-1);
				break;
			case 2:		// Hold
				if( nTileFrameID >= nUVTileNum )
					nTileFrameID = nUVTileNum - 1;
				break;
			default:	// Loop
				nTileFrameID %= nUVTileNum;
				break;
			}

			Vector2D UVOffset = m_pVC->m_MaterialSkin.getMatKeyFrameUVOffset(m_BatchConfig.m_fTimePassedFromCreated);
			Vector3D TexIndexUVOffset(
				(float)nTileFrameID,
				UVOffset.x + m_pVC->m_MaterialSkin.vUVSpeed[0] * m_BatchConfig.m_fTimePassedFromCreated,
				UVOffset.y + m_pVC->m_MaterialSkin.vUVSpeed[1] * m_BatchConfig.m_fTimePassedFromCreated);
			
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TexIndexUVOffset", TexIndexUVOffset);
		}
		default:
			break;
		}

		Vector4D DiffColor = m_pVC->m_MaterialSkin.getMatKeyFrameColor(m_BatchConfig.m_fTimePassedFromCreated);
		DiffColor.w = m_BatchConfig.m_fBatchAlpha;
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("vMaterialColor", DiffColor);

		m_pVC->m_pVBO->bindVAO();

		glDrawElements( m_RenderDevice->primitiveTypeToGL( m_pVC->m_MaterialSkin.nPrimitiveType ),
						m_pVC->GetIndexNum(), 
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pVC->m_pVBO->unBindVAO();
	}
}

// opaque
void COpaqueRenderBatch::BuildQueueValue()
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	if( m_pSB )
	{
		stQueueValue.ulpriority = m_pSB->MaterialSkin.nTextureID[0];
		stQueueValue.ulshader = m_pSB->MaterialSkin.nShaderType;
	}
	else if( m_pVC )
	{
		stQueueValue.ulpriority = m_pVC->m_MaterialSkin.nTextureID[0];
		stQueueValue.ulshader = m_pVC->m_MaterialSkin.nShaderType;
	}
	else
	{
		stQueueValue.ulpriority = 0;
		stQueueValue.ulshader = 0;
	}
	
	return;
}

// AlphaTest
void CAlphaTestRenderBatch::BuildQueueValue()
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	if( m_pSB )
	{
		stQueueValue.ulpriority = m_pSB->MaterialSkin.nTextureID[0];
		stQueueValue.ulshader = m_pSB->MaterialSkin.nShaderType;
	}
	else if( m_pVC )
	{
		stQueueValue.ulpriority = m_pVC->m_MaterialSkin.nTextureID[0];
		stQueueValue.ulshader = m_pVC->m_MaterialSkin.nShaderType;
	}
	else
	{
		stQueueValue.ulpriority = 0;
		stQueueValue.ulshader = 0;
	}
}

// Transparent
void CTransparentRenderBatch::BuildQueueValue()
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	if( m_pSB )
	{
		stQueueValue.ulpriority = m_pSB->MaterialSkin.nTextureID[0];
		stQueueValue.ulshader = m_pSB->MaterialSkin.nShaderType;
	}
	else if( m_pVC )
	{
		stQueueValue.ulpriority = m_pVC->m_MaterialSkin.nTextureID[0];
		stQueueValue.ulshader = m_pVC->m_MaterialSkin.nShaderType;
	}
	else
	{
		stQueueValue.ulpriority = 0;
		stQueueValue.ulshader = 0;
	}

	Vector4D BoundingBOXCenter = m_BBOXCenter * m_MatWorld;
	Vector4D dis = BoundingBOXCenter - m_RenderDevice->getOpenGLCamera()->GetPos();
	
	stQueueValue.uldistance = int32( -dis.GetLength() );

	return;
}

// Line
void CLineRenderBatch::BuildQueueValue()
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	stQueueValue.ulshader = SGPST_VERTEXCOLOR;

	return;
}

// SkinAnim
void CSkinAnimRenderBatch::BuildQueueValue()
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	stQueueValue.ulshader = SGPST_SKELETONANIM;
	if( m_pSB )
	{
		stQueueValue.ulpriority = m_pSB->MaterialSkin.nTextureID[0];
	}
	else
	{
		stQueueValue.ulpriority = 0;
	}
	return;
}

void CSkinAnimRenderBatch::PreRender()
{
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());

	if( m_pSB )
	{
		if( m_ShaderProgramIdx != m_pSB->MaterialSkin.nShaderType )
		{
			pShaderManager->GetGLSLShaderProgram(m_pSB->MaterialSkin.nShaderType)->useProgram();
			m_ShaderProgramIdx = m_pSB->MaterialSkin.nShaderType;
		}
		if( !m_SkinAnimShaderLightmapTexSetted )
		{
			m_RenderDevice->GetTextureManager()->getTextureByID( m_RenderDevice->getOpenGLTerrainRenderer()->getLightmapTextureID() )->pSGPTexture->BindTexture2D(1);
			pShaderManager->GetGLSLShaderProgram(m_ShaderProgramIdx)->setShaderUniform("gSamplerLightmap", 1);			

			m_SkinAnimShaderLightmapTexSetted = true;
		}
		
	}
}


void CSkinAnimRenderBatch::Render()
{
	int ShaderProgramIdx = 0;
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	Matrix4x4 MVP;

	if( m_RenderDevice->getCurrentRenderStage() == SGPRS_WATERREFLECTION )
	{
		MVP = m_MatWorld * m_RenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_RenderDevice->getOpenGLWaterRenderer()->m_ObliqueNearPlaneReflectionProjMatrix;
	}
	else
	{
		MVP = m_MatWorld * m_RenderDevice->getOpenGLCamera()->m_mViewProjMatrix;
	}
	
	if( m_pSB )
	{
		ShaderProgramIdx = m_pSB->MaterialSkin.nShaderType;
		//pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->useProgram();
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("worldMatrix", m_MatWorld);
		switch( m_pSB->MaterialSkin.nTextureNum )
		{
		//case 2:
		//	m_RenderDevice->GetTextureManager()->getTextureByID(m_pSB->MaterialSkin.nTextureID[1])->pSGPTexture->BindTexture2D(1);
		//	pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler1", 1);			
		
		case 1:
			// Diffuse Texture
			if( m_BatchConfig.m_nReplacedTextureID != 0 )
				m_RenderDevice->GetTextureManager()->getTextureByID(m_BatchConfig.m_nReplacedTextureID)->pSGPTexture->BindTexture2D(0);
			else
				m_RenderDevice->GetTextureManager()->getTextureByID(m_pSB->MaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler0", 0);
	
		// Texture coord UV anim
		{
			Vector4D TextureAtlas((float)m_pSB->MaterialSkin.vUVTile[0], (float)m_pSB->MaterialSkin.vUVTile[1], 1.0f/m_pSB->MaterialSkin.vUVTile[0], 1.0f/m_pSB->MaterialSkin.vUVTile[1]);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TextureAtlas", TextureAtlas);

			uint32 nTileFrameID = m_pSB->MaterialSkin.vUVTile[2] + uint32( m_BatchConfig.m_fTimePassedFromCreated / (m_pSB->MaterialSkin.vUVTile[4] * 0.001f) );
			uint32 nUVTileNum = m_pSB->MaterialSkin.vUVTile[0] * m_pSB->MaterialSkin.vUVTile[1];
			switch( m_pSB->MaterialSkin.vUVTile[3] )	
			{
			case 0:		// Loop
				nTileFrameID %= nUVTileNum;
				break;
			case 1:		// Ping Pong
				nTileFrameID %= (nUVTileNum * 2);
				nTileFrameID = (nTileFrameID < nUVTileNum) ? nTileFrameID : (2*nUVTileNum-nTileFrameID-1);
				break;
			case 2:		// Hold
				if( nTileFrameID >= nUVTileNum )
					nTileFrameID = nUVTileNum - 1;
				break;
			default:	// Loop
				nTileFrameID %= nUVTileNum;
				break;
			}

			Vector2D UVOffset = m_pSB->MaterialSkin.getMatKeyFrameUVOffset(m_BatchConfig.m_fTimePassedFromCreated);
			Vector3D TexIndexUVOffset(
				(float)nTileFrameID, 
				UVOffset.x + m_pSB->MaterialSkin.vUVSpeed[0] * m_BatchConfig.m_fTimePassedFromCreated,
				UVOffset.y + m_pSB->MaterialSkin.vUVSpeed[1] * m_BatchConfig.m_fTimePassedFromCreated);
			
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TexIndexUVOffset", TexIndexUVOffset);
		}
			
		default:
			break;
		}

		Vector4D DiffColor = m_pSB->MaterialSkin.getMatKeyFrameColor(m_BatchConfig.m_fTimePassedFromCreated);
		DiffColor.w = m_BatchConfig.m_fBatchAlpha;
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("vMaterialColor", DiffColor);


		Vector2D LightmapUV(m_MatWorld._41 / m_RenderDevice->getOpenGLTerrainRenderer()->getTerrainSize()*SGPTT_TILE_METER*SGPTT_TILENUM, 1.0f - m_MatWorld._43 / m_RenderDevice->getOpenGLTerrainRenderer()->getTerrainSize()*SGPTT_TILE_METER*SGPTT_TILENUM);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("vTexCoordLightMap", LightmapUV);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("SunColor", m_RenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("SunDirection", -m_RenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection());

		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("BoneMatrixArray[0]", (Vector4D*)m_BoneMatrixBuffer, m_BoneNum*3);

		m_pSB->pVBO->bindVAO();

		glDrawElements( m_RenderDevice->primitiveTypeToGL( m_pSB->MaterialSkin.nPrimitiveType ),
						m_pSB->nNumIndis, 
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pSB->pVBO->unBindVAO();

	}
}

void CSkinAnimRenderBatch::PostRender()
{
}


// Particle point sprites
void CParticlePSRenderBatch::BuildQueueValue(void)
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	stQueueValue.ulshader = SGPST_PARTICLE_POINTSPRITES;

	if( m_pParticleBuffer )
	{
		stQueueValue.ulpriority = m_pParticleBuffer->m_MaterialSkin.nTextureID[0];
	}
	else
	{
		stQueueValue.ulpriority = 0;
	}

	Vector4D BoundingBOXCenter = m_BBOXCenter * m_MatWorld;
	Vector4D dis = BoundingBOXCenter - m_RenderDevice->getOpenGLCamera()->GetPos();
	
	stQueueValue.uldistance = int32( -dis.GetLength() );


	return;
}

void CParticlePSRenderBatch::PreRender()
{
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	if( m_pParticleBuffer )
	{
		if( m_ShaderProgramIdx != m_pParticleBuffer->m_MaterialSkin.nShaderType )
		{
			pShaderManager->GetGLSLShaderProgram(m_pParticleBuffer->m_MaterialSkin.nShaderType)->useProgram();
			m_ShaderProgramIdx = m_pParticleBuffer->m_MaterialSkin.nShaderType;
		}
	}
}

void CParticlePSRenderBatch::PostRender()
{
}

void CParticlePSRenderBatch::Render()
{
	int ShaderProgramIdx = 0;
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_MatWorld * m_RenderDevice->getOpenGLCamera()->m_mViewProjMatrix;
	Matrix4x4 MV = m_MatWorld * m_RenderDevice->getOpenGLCamera()->m_mViewMatrix;

	if( m_pParticleBuffer )
	{
		ShaderProgramIdx = m_pParticleBuffer->m_MaterialSkin.nShaderType;
		//pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->useProgram();
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewMatrix", MV);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("Thickness", Vector4D(0.04f, 0.04f, 0.0f, m_pParticleBuffer->m_Thickness));
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("mode", m_pParticleBuffer->m_BlendMode);

		switch( m_pParticleBuffer->m_MaterialSkin.nTextureNum )
		{
		case 1:
			m_RenderDevice->GetTextureManager()->getTextureByID(m_pParticleBuffer->m_MaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler0", 0);
			break;
		default:
			break;
		}
		m_pParticleBuffer->getVBO()->bindVAO();

		glDrawElements( m_RenderDevice->primitiveTypeToGL( m_pParticleBuffer->m_MaterialSkin.nPrimitiveType ),
						m_pParticleBuffer->m_NumOfActiveParticles * m_pParticleBuffer->m_particleIndexSize, 
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pParticleBuffer->getVBO()->unBindVAO();
	}
}



// Particle Line
void CParticleLineRenderBatch::BuildQueueValue(void)
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	stQueueValue.ulshader = SGPST_PARTICLE_LINE;

	Vector4D BoundingBOXCenter = m_BBOXCenter * m_MatWorld;
	Vector4D dis = BoundingBOXCenter - m_RenderDevice->getOpenGLCamera()->GetPos();
	
	stQueueValue.uldistance = int32( -dis.GetLength() );

	return;
}

void CParticleLineRenderBatch::PreRender()
{
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	if( m_pParticleBuffer )
	{
		if( m_ShaderProgramIdx != m_pParticleBuffer->m_MaterialSkin.nShaderType )
		{
			pShaderManager->GetGLSLShaderProgram(m_pParticleBuffer->m_MaterialSkin.nShaderType)->useProgram();
			m_ShaderProgramIdx = m_pParticleBuffer->m_MaterialSkin.nShaderType;
		}
	}

	if(COpenGLES2Config::getInstance()->FullScreenAntiAlias)
	{
		glLineWidth(jlimit(COpenGLES2Config::getInstance()->DimAliasedLine[0], COpenGLES2Config::getInstance()->DimAliasedLine[1], m_pParticleBuffer->m_Thickness));
	}
	else
	{
		glLineWidth(jlimit(COpenGLES2Config::getInstance()->DimAliasedLine[0], COpenGLES2Config::getInstance()->DimAliasedLine[1], m_pParticleBuffer->m_Thickness));
	}
}

void CParticleLineRenderBatch::PostRender()
{
	glLineWidth( 1 );
}


void CParticleLineRenderBatch::Render()
{
	int ShaderProgramIdx = 0;
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_MatWorld * m_RenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

	if( m_pParticleBuffer )
	{
		ShaderProgramIdx = m_pParticleBuffer->m_MaterialSkin.nShaderType;
		//pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->useProgram();
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("mode", m_pParticleBuffer->m_BlendMode);

		m_pParticleBuffer->getVBO()->bindVAO();

		glDrawElements( m_RenderDevice->primitiveTypeToGL( m_pParticleBuffer->m_MaterialSkin.nPrimitiveType ),
						m_pParticleBuffer->m_NumOfActiveParticles * m_pParticleBuffer->m_particleIndexSize, 
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pParticleBuffer->getVBO()->unBindVAO();
	}
}

// Particle Quad
void CParticleQuadRenderBatch::BuildQueueValue(void)
{
	memset( &stQueueValue.ulQueueValue, 0x00, sizeof(uint64) );

	stQueueValue.ulshader = SGPST_PARTICLE_QUAD_TEXATLAS;

	if( m_pParticleBuffer )
	{
		stQueueValue.ulpriority = m_pParticleBuffer->m_MaterialSkin.nTextureID[0];
	}
	else
	{
		stQueueValue.ulpriority = 0;
	}

	Vector4D BoundingBOXCenter = m_BBOXCenter * m_MatWorld;
	Vector4D dis = BoundingBOXCenter - m_RenderDevice->getOpenGLCamera()->GetPos();
	
	stQueueValue.uldistance = int32( -dis.GetLength() );

	return;
}

void CParticleQuadRenderBatch::PreRender()
{
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	if( m_pParticleBuffer )
	{
		if( m_ShaderProgramIdx != m_pParticleBuffer->m_MaterialSkin.nShaderType )
		{
			pShaderManager->GetGLSLShaderProgram(m_pParticleBuffer->m_MaterialSkin.nShaderType)->useProgram();
			m_ShaderProgramIdx = m_pParticleBuffer->m_MaterialSkin.nShaderType;
		}
	}
}

void CParticleQuadRenderBatch::PostRender()
{
}

void CParticleQuadRenderBatch::Render()
{
	int ShaderProgramIdx = 0;
	COpenGLES2ShaderManager *pShaderManager = static_cast<COpenGLES2ShaderManager*>(m_RenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_MatWorld * m_RenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

	if( m_pParticleBuffer )
	{
		ShaderProgramIdx = m_pParticleBuffer->m_MaterialSkin.nShaderType;
		//pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->useProgram();
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("modelViewProjMatrix", MVP);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("TextureAtlas", m_TextureAtlas);
		pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("mode", m_pParticleBuffer->m_BlendMode);

		switch( m_pParticleBuffer->m_MaterialSkin.nTextureNum )
		{
		case 1:
			m_RenderDevice->GetTextureManager()->getTextureByID(m_pParticleBuffer->m_MaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
			pShaderManager->GetGLSLShaderProgram(ShaderProgramIdx)->setShaderUniform("gSampler0", 0);
			break;
		default:
			break;
		}
		m_pParticleBuffer->getVBO()->bindVAO();

		glDrawElements( m_RenderDevice->primitiveTypeToGL( m_pParticleBuffer->m_MaterialSkin.nPrimitiveType ),
						m_pParticleBuffer->m_NumOfActiveParticles * m_pParticleBuffer->m_particleIndexSize, 
						GL_UNSIGNED_SHORT,
						(void*)0 );

		m_pParticleBuffer->getVBO()->unBindVAO();
	}
}
