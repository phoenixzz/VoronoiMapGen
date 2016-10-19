

COpenGLSkydomeRenderer::COpenGLSkydomeRenderer(COpenGLRenderDevice *pRenderDevice)
	: m_pRenderDevice(pRenderDevice), m_SkydomeStaticBuffer(NULL), m_SunColorAndIntensity(1.0f, 1.0f, 1.0f, 0.1f)
{
}

COpenGLSkydomeRenderer::~COpenGLSkydomeRenderer()
{
}

void COpenGLSkydomeRenderer::updateSkydomeMaterialSkin(const CSGPSkyDome* pSkyDome)
{
	m_SkydomeMaterialSkin.nShaderType = SGPST_SKYDOMESCATTERING;

	if( pSkyDome->m_nSkydomeTextureID.size() > 1 )
	{
		m_SkydomeMaterialSkin.nTextureNum = 2;
		m_SkydomeMaterialSkin.nTextureID[0] = pSkyDome->m_nSkydomeTextureID[0];
		m_SkydomeMaterialSkin.nTextureID[1] = pSkyDome->m_nSkydomeTextureID[1];
	}
	else if( pSkyDome->m_nSkydomeTextureID.size() > 0 )
	{
		m_SkydomeMaterialSkin.nTextureNum = 1;
		m_SkydomeMaterialSkin.nTextureID[0] = pSkyDome->m_nSkydomeTextureID[0];
	}
}

void COpenGLSkydomeRenderer::update(float fDeltaTimeInSecond, CSGPSkyDome* pSkyDome, const Vector4D& campos)
{
	if( !pSkyDome )
		return;

	// Update SkyDome position
	pSkyDome->UpdateWorldMatrix(campos);

	// Update SkyDome UV param
	m_SkydomeCloudParam.x += pSkyDome->m_coludMoveSpeed_x * fDeltaTimeInSecond;
	m_SkydomeCloudParam.y += pSkyDome->m_coludMoveSpeed_z * fDeltaTimeInSecond;

	if( m_SkydomeCloudParam.x > 1.0f )
		m_SkydomeCloudParam.x -= 1.0f;
	if( m_SkydomeCloudParam.y > 1.0f )
		m_SkydomeCloudParam.y -= 1.0f;

	// Update SkyDome StaticBuffer
	if( pSkyDome->m_bUpdateModel )
	{
		CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(pSkyDome->m_SkydomeMF1ModelResourceID);
		for( int i=0; i<pMF1Res->StaticMeshIDArray.size(); i++ )
		{
			m_SkydomeStaticBuffer = (COpenGLStaticBuffer*)
				m_pRenderDevice->GetVertexCacheManager()->GetStaticBufferByID(pMF1Res->StaticMeshIDArray[i]);
		}
		pSkyDome->m_bUpdateModel = false;
	}
}

void COpenGLSkydomeRenderer::render(CSGPSkyDome* pSkyDome)
{
	// Set skydome rendering param
	float fReflectance = 0.1f;
	m_SkydomeMaterialSkin.nShaderType = SGPST_SKYDOMESCATTERING;
	this->m_SunDir = m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getSunDirection();
	Vector3D vecBetaR = pSkyDome->GetScatter().m_BetaRay * pSkyDome->GetScatter().m_fBetaRayMultiplier;
    Vector3D vecBetaDashR = pSkyDome->GetScatter().m_BetaDashRay * pSkyDome->GetScatter().m_fBetaRayMultiplier;
    Vector3D vecBetaM = pSkyDome->GetScatter().m_BetaMie * pSkyDome->GetScatter().m_fBetaMieMultiplier;
	Vector3D vecBetaDashM = pSkyDome->GetScatter().m_BetaDashMie * pSkyDome->GetScatter().m_fBetaMieMultiplier;
	Vector3D vecBetaRM = vecBetaR + vecBetaM;
	this->m_BetaRPlusBetaM = vecBetaRM;
    Vector3D vecOneOverBetaRM( 1.0f / vecBetaRM.x, 1.0f / vecBetaRM.y, 1.0f / vecBetaRM.z );
    Vector3D vecG( 1.0f - pSkyDome->GetScatter().m_fHGgFunction * pSkyDome->GetScatter().m_fHGgFunction, 1.0f + pSkyDome->GetScatter().m_fHGgFunction * pSkyDome->GetScatter().m_fHGgFunction, 2.0f * pSkyDome->GetScatter().m_fHGgFunction );
    Vector4D vecTermMulitpliers(pSkyDome->GetScatter().m_fInscatteringMultiplier, 0.138f * fReflectance, 0.113f * fReflectance, 0.08f * fReflectance);
	this->m_HGg = vecG;
	this->m_Multipliers = vecTermMulitpliers;
	this->m_BetaDashR = vecBetaDashR;
	this->m_BetaDashM = vecBetaDashM;
	this->m_OneOverBetaRPlusBetaM = vecOneOverBetaRM;
	this->m_SunColorAndIntensity = pSkyDome->GetScatter().m_SunColorAndIntensity;





	m_SkydomeWorldMatrix = pSkyDome->GetWorldMatrix();

	m_SkydomeCloudParam.z = pSkyDome->m_coludNoiseScale;
	m_SkydomeCloudParam.w = pSkyDome->m_coludBrightness;
}


void COpenGLSkydomeRenderer::DoDrawSkydomeRenderBatch()
{
	if( m_SkydomeStaticBuffer == NULL )
		return;

	COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());
	Matrix4x4 MVP = m_SkydomeWorldMatrix * m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix;
	Matrix4x4 MV = m_SkydomeWorldMatrix * m_pRenderDevice->getOpenGLCamera()->m_mViewMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &SkydomeMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_skydome);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( SkydomeMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);



#if 0
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->useProgram();

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("modelViewProjMatrix", MVP);

	m_pRenderDevice->GetTextureManager()->getTextureByID(m_SkydomeMaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("gSampler0", 0);
	if( m_SkydomeMaterialSkin.nTextureNum == 2 )
	{
		m_pRenderDevice->GetTextureManager()->getTextureByID(m_SkydomeMaterialSkin.nTextureID[1])->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("gSampler1", 1);
	}

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("cloudParam", m_SkydomeCloudParam);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("apexColor", m_SkydomeApexColor);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("centerColor", m_SkydomeCenterColor);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("maxSkyHeight", m_SkydomeMaxHeight);
#endif




	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->useProgram();

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("modelViewProjMatrix", MVP);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("modelViewMatrix", MV);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("InvSunDir", m_SunDir);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("HGg", m_HGg);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("BetaRPlusBetaM", m_BetaRPlusBetaM);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("Multipliers", m_Multipliers);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("BetaDashR", m_BetaDashR);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("BetaDashM", m_BetaDashM);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("OneOverBetaRPlusBetaM", m_OneOverBetaRPlusBetaM);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("SunColorAndIntensity", m_SunColorAndIntensity);

	m_pRenderDevice->GetTextureManager()->getTextureByID(m_SkydomeMaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("gSampler0", 0);
	if( m_SkydomeMaterialSkin.nTextureNum == 2 )
	{
		m_pRenderDevice->GetTextureManager()->getTextureByID(m_SkydomeMaterialSkin.nTextureID[1])->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("gSampler1", 1);
	}

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("cloudParam", m_SkydomeCloudParam);


	
	m_SkydomeStaticBuffer->pVBO->bindVAO();

	glDrawElements( m_pRenderDevice->primitiveTypeToGL( m_SkydomeMaterialSkin.nPrimitiveType ),
					m_SkydomeStaticBuffer->nNumIndis, 
					GL_UNSIGNED_SHORT,
					(void*)0 );

	m_SkydomeStaticBuffer->pVBO->unBindVAO();



	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();
}


void COpenGLSkydomeRenderer::DoDrawReflectionSkydomeRenderBatch()
{
	if( m_SkydomeStaticBuffer == NULL )
		return;

	COpenGLShaderManager *pShaderManager = static_cast<COpenGLShaderManager*>(m_pRenderDevice->GetShaderManager());

	Matrix4x4 SkydomeWorldMatrix;
	Plane ReflectionPlane;
	ReflectionPlane.Set( Vector3D(0, 1, 0), -m_pRenderDevice->GetWorldSystemManager()->getWater()->m_fWaterHeight );
	SkydomeWorldMatrix.ReflectPlane( ReflectionPlane );

	Vector4D newCamPos = m_pRenderDevice->getOpenGLCamera()->m_CameraPos * SkydomeWorldMatrix;
	SkydomeWorldMatrix = m_SkydomeWorldMatrix;

	SkydomeWorldMatrix.Translate( newCamPos.x, newCamPos.y, newCamPos.z );

	Matrix4x4 MVP = SkydomeWorldMatrix * m_pRenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_pRenderDevice->getOpenGLWaterRenderer()->m_ObliqueNearPlaneReflectionProjMatrix;
	Matrix4x4 MV = SkydomeWorldMatrix * m_pRenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix;

	ISGPMaterialSystem::MaterialList &Mat_List = m_pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &SkydomeMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_skydome);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PushMaterial( SkydomeMaterial_info.m_material, MM_Add );

	m_pRenderDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->useProgram();

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("modelViewProjMatrix", MVP);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("modelViewMatrix", MV);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("InvSunDir", m_SunDir);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("HGg", m_HGg);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("BetaRPlusBetaM", m_BetaRPlusBetaM);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("Multipliers", m_Multipliers);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("BetaDashR", m_BetaDashR);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("BetaDashM", m_BetaDashM);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("OneOverBetaRPlusBetaM", m_OneOverBetaRPlusBetaM);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("SunColorAndIntensity", m_SunColorAndIntensity);

	m_pRenderDevice->GetTextureManager()->getTextureByID(m_SkydomeMaterialSkin.nTextureID[0])->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("gSampler0", 0);
	if( m_SkydomeMaterialSkin.nTextureNum == 2 )
	{
		m_pRenderDevice->GetTextureManager()->getTextureByID(m_SkydomeMaterialSkin.nTextureID[1])->pSGPTexture->BindTexture2D(1);
		pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("gSampler1", 1);
	}

	pShaderManager->GetGLSLShaderProgram(m_SkydomeMaterialSkin.nShaderType)->setShaderUniform("cloudParam", m_SkydomeCloudParam);


	
	m_SkydomeStaticBuffer->pVBO->bindVAO();

	glDrawElements( m_pRenderDevice->primitiveTypeToGL( m_SkydomeMaterialSkin.nPrimitiveType ),
					m_SkydomeStaticBuffer->nNumIndis, 
					GL_UNSIGNED_SHORT,
					(void*)0 );

	m_SkydomeStaticBuffer->pVBO->unBindVAO();



	m_pRenderDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	m_pRenderDevice->getOpenGLMaterialRenderer()->PopMaterial();

}