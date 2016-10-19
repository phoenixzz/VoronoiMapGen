

COpenGLWaterRenderer::COpenGLWaterRenderer(COpenGLRenderDevice *pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_BumpWaveTextureID(0)
{
	m_VisibleWaterChunks.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE / 3);
}

COpenGLWaterRenderer::~COpenGLWaterRenderer()
{
	releaseWaterWaveTexture();
}

void COpenGLWaterRenderer::update(float fDeltaTimeInSecond, CSGPWater* pWater)
{
	// First get all visible Water Terrain Chunks
	m_VisibleWaterChunks.clearQuick();

	if( !pWater )
		return;

	if( pWater->m_TerrainWaterChunks.size() > 0 )
	{
		CSGPTerrainChunk** pEnd = pWater->m_TerrainWaterChunks.end();
		for( CSGPTerrainChunk** pBegin = pWater->m_TerrainWaterChunks.begin(); pBegin < pEnd; pBegin++ )
		{
			if( m_pRenderDevice->GetWorldSystemManager()->isTerrainChunkVisible( *pBegin ) )
				m_VisibleWaterChunks.add( *pBegin );
		}
	}

	// update Reflection View Matrix and Project Matrix
	createReflectionMatrix(pWater->m_fWaterHeight);

	// update water rendering params
	m_vWaveParams.x += fDeltaTimeInSecond;

	Vector2D WaveDir = pWater->m_vWaveDir;
	WaveDir.Normalize();
	m_vWaveDir.Set( WaveDir.x * pWater->m_fWaveSpeed, WaveDir.y * pWater->m_fWaveSpeed );
}

void COpenGLWaterRenderer::render(CSGPWater* pWater)
{
	// prepare water rendering params for rendering
	m_vWaveParams.y = pWater->m_fWaterHeight;
	m_vWaveParams.z = 1.0f;
	m_vWaveParams.w = pWater->m_fWaveRate;

	m_vWaterColor.Set(pWater->m_vWaterSurfaceColor.x, pWater->m_vWaterSurfaceColor.y, pWater->m_vWaterSurfaceColor.z, pWater->m_fWaterSunSpecularPower);
	m_vWaterBumpFresnel.Set(pWater->m_fRefractionBumpScale, pWater->m_fReflectionBumpScale, pWater->m_fFresnelBias, pWater->m_fFresnelPow);
	m_vWaterDepthBlend.Set(pWater->m_fWaterDepthScale, pWater->m_fWaterBlendScale);
}

void COpenGLWaterRenderer::DoDrawWaterReflectionMap()
{
	// Render Scene in Mirror View

	// Skydome
	if( CSGPWorldConfig::getInstance()->m_bShowSkyDome )
		m_pRenderDevice->getOpenGLSkydomeRenderer()->DoDrawReflectionSkydomeRenderBatch();

	// Terrain
	if( CSGPWorldConfig::getInstance()->m_bShowTerrain )
		m_pRenderDevice->getOpenGLTerrainRenderer()->DoDrawReflectionTerrainRenderBatch();

	// Opaque Renderbatch
	if( CSGPWorldConfig::getInstance()->m_bShowStaticObject )
		m_pRenderDevice->getOpenGLMaterialRenderer()->DoDrawRenderBatch_Opaque();

	// Skin Animination
	if( CSGPWorldConfig::getInstance()->m_bShowSkeleton )
		m_pRenderDevice->getOpenGLMaterialRenderer()->DoDrawRenderBatch_SkinAnim();

}

void COpenGLWaterRenderer::DoDrawWaterRefractionMap()
{
	COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &RefractionTerrainMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_waterrefraction);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( RefractionTerrainMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_REFRACTION)->useProgram();
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_REFRACTION)->setShaderUniform("worldViewProjMatrix", MVP);	
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_REFRACTION)->setShaderUniform("waterHeight", m_pRenderDevice->GetWorldSystemManager()->getWater()->m_fWaterHeight);


	CSGPTerrainChunk** pEnd = m_VisibleWaterChunks.end();	
	for( CSGPTerrainChunk** pBegin = m_VisibleWaterChunks.begin(); pBegin < pEnd; pBegin++ )
	{
		m_pRenderDevice->extGlBindVertexArray(m_pRenderDevice->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray[(*pBegin)->GetTerrainChunkIndex()]->nVAOID);
		glDrawElements( GL_TRIANGLES,
						m_pRenderDevice->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray[(*pBegin)->GetTerrainChunkIndex()]->nIndexCount, 
						GL_UNSIGNED_SHORT,
						(GLvoid*)(sizeof(GLushort)*m_pRenderDevice->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray[(*pBegin)->GetTerrainChunkIndex()]->nIndexOffset) );
	}
	m_pRenderDevice->extGlBindVertexArray(0);

	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();
}

void COpenGLWaterRenderer::DoDrawWaterSimulation(uint32 ReflectionMapID, uint32 RefractionMapID)
{
	COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &WaterTerrainMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_waterrender);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( WaterTerrainMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

	Vector4D CamPos;
	m_pRenderDevice->getCamreaPosition( &CamPos );

	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->useProgram();
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("worldViewProjMatrix", MVP);	
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vSunDir", m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getSunDirection());
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vCameraPos", CamPos);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vWaveParams", m_vWaveParams);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vWaveDir", m_vWaveDir);

	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vWaterBumpFresnel", m_vWaterBumpFresnel);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vWaterColor", m_vWaterColor);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vWaterDepthBlend", m_vWaterDepthBlend);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("vSunColor", m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity());

	m_pRenderDevice->GetTextureManager()->getTextureByID(m_BumpWaveTextureID)->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("gSampler0", 0);
	m_pRenderDevice->GetTextureManager()->getTextureByID(RefractionMapID)->pSGPTexture->BindTexture2D(1);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("gSampler1", 1);
	m_pRenderDevice->GetTextureManager()->getTextureByID(ReflectionMapID)->pSGPTexture->BindTexture2D(2);
	pShaderManager->GetGLSLShaderProgram(SGPST_WATER_RENDER)->setShaderUniform("gSampler2", 2);


	CSGPTerrainChunk** pEnd = m_VisibleWaterChunks.end();	
	for( CSGPTerrainChunk** pBegin = m_VisibleWaterChunks.begin(); pBegin < pEnd; pBegin++ )
	{
		m_pRenderDevice->extGlBindVertexArray(m_pRenderDevice->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray[(*pBegin)->GetTerrainChunkIndex()]->nVAOID);
		glDrawElements( GL_TRIANGLES,
						m_pRenderDevice->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray[(*pBegin)->GetTerrainChunkIndex()]->nIndexCount, 
						GL_UNSIGNED_SHORT,
						(GLvoid*)(sizeof(GLushort)*m_pRenderDevice->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray[(*pBegin)->GetTerrainChunkIndex()]->nIndexOffset) );
	}


	m_pRenderDevice->extGlBindVertexArray(0);

	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();

}







void COpenGLWaterRenderer::createWaterWaveTexture(const String& WaveTextureName)
{
	// register Water wave Bump texture
	m_BumpWaveTextureID = m_pRenderDevice->GetTextureManager()->registerTexture(WaveTextureName);
	COpenGLTexture *pOpenGLTexture = static_cast<COpenGLTexture*>( m_pRenderDevice->GetTextureManager()->getTextureByID(m_BumpWaveTextureID)->pSGPTexture );

	pOpenGLTexture->setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
	pOpenGLTexture->setWrapMode(TEXTURE_ADDRESS_MIRRORED_REPEAT, TEXTURE_ADDRESS_MIRRORED_REPEAT);
	pOpenGLTexture->unBindTexture2D(0);
}

void COpenGLWaterRenderer::releaseWaterWaveTexture()
{
	// unregister Water wave Bump texture
	if( m_BumpWaveTextureID != 0 )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_BumpWaveTextureID);
	m_BumpWaveTextureID = 0;
}






void COpenGLWaterRenderer::createReflectionMatrix( float fWaterHeight )
{
	Matrix4x4 ReflectModelViewProj;
	Plane WaterPlane;
	WaterPlane.Set( Vector3D(0, 1, 0), -fWaterHeight );

	ReflectModelViewProj.ReflectPlane(WaterPlane);
	m_MirrorViewMatrix = ReflectModelViewProj * m_pRenderDevice->getOpenGLCamera()->m_mViewMatrix;

	// Check if camera is below water surface, invert clip plane
	Vector4D pEye = m_pRenderDevice->getOpenGLCamera()->m_CameraPos;
	if(pEye.y < fWaterHeight)
	{
		WaterPlane.Set( Vector3D(0, -1, 0), -fWaterHeight );
	}
	WaterPlane.Normalize();


	// Build Oblique Near Plane Reflection Project Matrix
	Matrix4x4 matProj = m_pRenderDevice->getOpenGLCamera()->m_mProjMatrix;
	Matrix4x4 WorldToProjection = m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix * m_pRenderDevice->getOpenGLCamera()->m_mOpenGLProjTransInv;

	Matrix4x4 InvWVP;
	InvWVP.InverseOf(WorldToProjection);
	WorldToProjection.TransposeOf(InvWVP);


	// Transform clip plane into projection space
	Vector4D clipPlane_new( WaterPlane.m_vcNormal.x, WaterPlane.m_vcNormal.y, WaterPlane.m_vcNormal.z, WaterPlane.m_fDistance );
	Vector4D projClipPlane = clipPlane_new * WorldToProjection;

	if( projClipPlane.w == 0 )
	{
		m_ObliqueNearPlaneReflectionProjMatrix = matProj;
		return;
	}
	if( projClipPlane.w > 0 )
		projClipPlane = Vector4D( -WaterPlane.m_vcNormal.x, -WaterPlane.m_vcNormal.y, -WaterPlane.m_vcNormal.z, -WaterPlane.m_fDistance ) * WorldToProjection;



	Matrix4x4 matClipProj;
	matClipProj.Identity();

	matClipProj._13 = projClipPlane.x;
	matClipProj._23 = projClipPlane.y;
	matClipProj._33 = projClipPlane.z;
	matClipProj._43 = projClipPlane.w;

	m_ObliqueNearPlaneReflectionProjMatrix = matProj * m_pRenderDevice->getOpenGLCamera()->m_mOpenGLProjTransInv * matClipProj * m_pRenderDevice->getOpenGLCamera()->m_mOpenGLProjTrans;

}