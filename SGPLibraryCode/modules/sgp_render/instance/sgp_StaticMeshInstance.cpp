

CStaticMeshInstance::CStaticMeshInstance(ISGPRenderDevice *pdevice) :
	m_pRenderDevice(pdevice), m_vPosition(0,0,0), m_vRotationXYZ(0,0,0), 
	m_fScale(1.0f),
	m_bNeedUpdate(true),
	m_bVisible(false),
	m_fInstanceRenderAlpha(1.0f),
	m_fOldInstanceRenderAlpha(1.0f),
	m_fInstanceRenderScale(1.0f),
	m_fOldInstanceRenderScale(1.0f),
	m_MF1ModelResourceID(0xFFFFFFFF),
	m_LightMapTextureID(0),
	m_pCurrentConfig(NULL)
{
	resetModel();
}

CStaticMeshInstance::~CStaticMeshInstance()
{
}

void CStaticMeshInstance::resetModel()
{
	m_ModelFileName = String::empty;
	m_MF1ModelResourceID = 0xFFFFFFFF;

	m_matModel.Identity();

	m_LightMapTextureID = 0;
	m_MF1ConfigIndex = 0;
	m_RenderFlagEx = 0;
	m_bVisible = false;

	m_fInstanceRenderAlpha = 1.0f;
	m_fOldInstanceRenderAlpha = 1.0f;
	m_fInstanceRenderScale = 1.0f;	
	m_fOldInstanceRenderScale = 1.0f;

	m_pCurrentConfig = NULL;

	m_InstanceBatchConfig.Reset();
}

void CStaticMeshInstance::destroyModel()
{
	setConfigSetting(NULL);

	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		if( m_pRenderDevice->isResLoadingMultiThread() )
			m_pRenderDevice->GetModelManager()->unRegisterModelByIDMT(m_MF1ModelResourceID);
		else
			m_pRenderDevice->GetModelManager()->unRegisterModelByID(m_MF1ModelResourceID);
	
		unregisterLightmapTexture();
	}


	resetModel();
}

void CStaticMeshInstance::changeModel( const String& MF1ModelFileName, uint32 ConfigIndex )
{
	// If currently have resource in this Instance, first unregister
	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		destroyModel();
	}

	m_ModelFileName = MF1ModelFileName;
	m_MF1ConfigIndex = ConfigIndex;

	if( m_pRenderDevice->isResLoadingMultiThread() )
		m_MF1ModelResourceID = m_pRenderDevice->GetModelManager()->registerModelMT(MF1ModelFileName, false);
	else
		m_MF1ModelResourceID = m_pRenderDevice->GetModelManager()->registerModel(MF1ModelFileName, false);
	
	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

		// Setting Config
		if( pMF1Res && pMF1Res->pModelMF1 && m_MF1ConfigIndex > 0 && m_MF1ConfigIndex <= pMF1Res->pModelMF1->m_Header.m_iNumConfigs )
			setConfigSetting(&pMF1Res->pModelMF1->m_pConfigSetting[m_MF1ConfigIndex-1]);

		buildOBB();
	}
}

void CStaticMeshInstance::render()
{
	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

		for( uint32 i=0; i<(uint32)pMF1Res->StaticMeshIDArray.size(); i++ )
		{
			if( isMeshVisible(i) )
			{
				// Update Replaced Texture
				m_InstanceBatchConfig.m_nReplacedTextureID = 0;
				if( m_pCurrentConfig )
				{
					for( uint32 j=0; j<m_pCurrentConfig->ReplaceTextureConfigNum; j++ )
					{
						if( m_pCurrentConfig->pReplaceTextureConfigList[j].MeshID == i )
							m_InstanceBatchConfig.m_nReplacedTextureID = m_pRenderDevice->GetTextureManager()->getTextureIDByName(String(m_pCurrentConfig->pReplaceTextureConfigList[j].TexFilename));
					}
				}

				if( pMF1Res->pModelMF1->m_pLOD0Meshes[i].m_nType >= static_cast<uint32>(SGPMESHCF_BBRD) &&
					pMF1Res->pModelMF1->m_pLOD0Meshes[i].m_nType <= static_cast<uint32>(SGPMESHCF_BBRD_VERTICALGROUND) )
					m_pRenderDevice->GetVertexCacheManager()->RenderStaticBuffer( pMF1Res->StaticMeshIDArray[i], getBBRDMeshMatrix(i), m_InstanceBatchConfig );
				else
					m_pRenderDevice->GetVertexCacheManager()->RenderStaticBuffer( pMF1Res->StaticMeshIDArray[i], m_matModel, m_InstanceBatchConfig );
			}
		}
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
		{
			if( isParticleVisible(i) )
				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->render();
		}
	}
}

bool CStaticMeshInstance::update( float deltaTimeinSeconds )
{
	if( m_pRenderDevice->isResLoadingMultiThread() && (m_MF1ModelResourceID == 0xFFFFFFFF) )
	{
		m_MF1ModelResourceID = m_pRenderDevice->GetModelManager()->getModelIDByName(m_ModelFileName);
		if( m_MF1ModelResourceID != 0xFFFFFFFF )
		{
			CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

			// Setting Config
			if( pMF1Res && pMF1Res->pModelMF1 && m_MF1ConfigIndex > 0 && m_MF1ConfigIndex <= pMF1Res->pModelMF1->m_Header.m_iNumConfigs )
				setConfigSetting(&pMF1Res->pModelMF1->m_pConfigSetting[m_MF1ConfigIndex-1]);

			buildOBB();
		}
	}

	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return false;

	// Update Instance Model Transform Matrix
	if( m_bNeedUpdate )
	{
		m_matModel.Identity();		
		m_matModel._11 = m_matModel._22 = m_matModel._33 = getScale();		

		Matrix4x4 matTemp;
		Quaternion qRotation;
		qRotation.MakeFromEuler(getRotationXYZ().x, getRotationXYZ().y, getRotationXYZ().z);
		//Convert the quaternion to a rotation matrix
		qRotation.GetMatrix(&matTemp);
		m_matModel = m_matModel * matTemp;

		m_matModel.SetTranslation( getPosition() );		

		// Update Instance OOBB boundingbox
		updateOBB(m_matModel);

		m_bNeedUpdate = false;
	}

	if( !m_bVisible )
		return true;

	///////////////////////////////////////////////////////////////////////
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

	// LightMap Texture
	if( (m_LightMapTextureID != 0) && (m_LightMapTextureID != 1) )
		m_InstanceBatchConfig.m_nLightMapTextureID = m_LightMapTextureID;

	// If UV Anim, Update StaticBuffer Skin Time
	if( pMF1Res->pModelMF1->m_Header.m_iUVAnim > 0 )
		m_InstanceBatchConfig.m_fTimePassedFromCreated += deltaTimeinSeconds;

	// If Instance Alpha changed, Update StaticBuffer Skin Instance alpha
	if( (m_fInstanceRenderAlpha >= 0.0f) && (m_fInstanceRenderAlpha <= 1.0f) && (m_fOldInstanceRenderAlpha != m_fInstanceRenderAlpha) )
	{
		m_InstanceBatchConfig.m_fBatchAlpha = m_fInstanceRenderAlpha;

		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
			m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->setInstanceAlpha(m_fInstanceRenderAlpha);
		
		m_fOldInstanceRenderAlpha = m_fInstanceRenderAlpha;
	}

	// If Instance Scale changed, Update particle Instance Scale
	if( m_fOldInstanceRenderScale != m_fInstanceRenderScale )
	{
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
			m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->setInstanceScale(m_fInstanceRenderScale);
		
		m_fOldInstanceRenderScale = m_fInstanceRenderScale;
	}

	for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
	{
		if( isParticleVisible(i) )
		{
			m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->
				updateAbsolutePosition(pMF1Res->pModelMF1->m_pParticleEmitter[i].m_AbsoluteMatrix * m_matModel);

			m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->update(deltaTimeinSeconds);
		}
	}

	return true;
}

void CStaticMeshInstance::resetParticleSystemByID(uint32 partID, const SGPMF1ParticleTag& newSetting, bool* pGroupVisibleArray)
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res )
	{
		Matrix4x4 ParticleWorldMatrix;
		ParticleWorldMatrix.Identity();

		m_pRenderDevice->GetParticleManager()->recreateParticleSystemByID( pMF1Res->ParticleSystemIDArray[partID], ParticleWorldMatrix );

		ISGPParticleSystem* pSystem = m_pRenderDevice->GetParticleManager()->getParticleSystemByID( pMF1Res->ParticleSystemIDArray[partID] );
		SPARK::Group** pParticleGroups = pSystem->createParticleGroups(newSetting);
		if( pParticleGroups )
		{
			for(uint32 i=0; i<newSetting.m_SystemParam.m_groupCount; ++i)
			{
				if( pGroupVisibleArray )
				{
					if( pGroupVisibleArray[i] )
						pSystem->addGroup( pParticleGroups[i] );
					else
						SPARK_Destroy( pParticleGroups[i] );
				}
				else
					pSystem->addGroup( pParticleGroups[i] );
			}
			delete [] pParticleGroups;
			pParticleGroups = NULL;
		}
		pSystem->enableAABBComputing(newSetting.m_SystemParam.m_bEnableAABBCompute);
	}
}

void CStaticMeshInstance::registerLightmapTexture(const String& WorldMapName, const String& LightmapTexName)
{
	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTexture(
		String(L"Lightmap/") + WorldMapName + File::separator + LightmapTexName,
		false);
	if( TexID == 0 )
		m_LightMapTextureID = 1;		// If no lightmap texture exist, using white texture by default
	else
		m_LightMapTextureID = TexID;
}

void CStaticMeshInstance::unregisterLightmapTexture()
{
	if( (m_LightMapTextureID != 0) && (m_LightMapTextureID != 1) )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_LightMapTextureID);
}

void CStaticMeshInstance::updateLightmapTexture(uint32 TexWidth, uint32 TexHeight, uint32 *pLightMapData)
{
	static uint32 tempLightmapNum = 0;

	unregisterLightmapTexture();

	ISGPImage* pLightMapImage = m_pRenderDevice->GetTextureManager()->createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(TexWidth, TexHeight),
		pLightMapData,
		false);

	tempLightmapNum++;
	String LightmapTexName = String( L"TEMP_ObjectLM_" ) + String( tempLightmapNum ) + String( L".dds" );

	uint32 TexID = m_pRenderDevice->GetTextureManager()->registerTextureFromImage(LightmapTexName, pLightMapImage, false);
	jassert( TexID != 0 );

	m_LightMapTextureID = TexID;

	delete pLightMapImage;
	pLightMapImage = NULL;
}



void CStaticMeshInstance::setConfigSetting(SGPMF1ConfigSetting* pConfigSetting)
{
	if( pConfigSetting == m_pCurrentConfig )
		return;

	CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;


	// UnRegister old Replaced textures
	if( m_pCurrentConfig )
	{
		if( m_pRenderDevice->isResLoadingMultiThread() )
		{
			for( uint32 i=0; i<m_pCurrentConfig->ReplaceTextureConfigNum; i++ )
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByNameMT( String( m_pCurrentConfig->pReplaceTextureConfigList[i].TexFilename ) );
		}
		else
		{
			for( uint32 i=0; i<m_pCurrentConfig->ReplaceTextureConfigNum; i++ )
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByName( String( m_pCurrentConfig->pReplaceTextureConfigList[i].TexFilename ) );
		}
	}

	m_pCurrentConfig = pConfigSetting;


	// Register new Replaced textures
	if( m_pCurrentConfig && pMF1Model )
	{
		for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
		{
			for( uint32 j=0; j<m_pCurrentConfig->ReplaceTextureConfigNum; j++ )
			{
				if( m_pCurrentConfig->pReplaceTextureConfigList[j].MeshID == i )
				{
					if( m_pRenderDevice->isResLoadingMultiThread() )
						m_pRenderDevice->GetTextureManager()->registerTextureMT( String( m_pCurrentConfig->pReplaceTextureConfigList[j].TexFilename ) );
					else
						m_pRenderDevice->GetTextureManager()->registerTexture( String( m_pCurrentConfig->pReplaceTextureConfigList[j].TexFilename ) );
					
					break;
				}
			}
		}
	}
}

OBBox CStaticMeshInstance::getStaticMeshOBBox()
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
		return OBBox(&pMF1Res->pModelMF1->m_MeshAABBox);
	return OBBox();
}


void CStaticMeshInstance::buildOBB()
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
	{
		m_InstanceOBBox.vcCenter = pMF1Res->pModelMF1->m_MeshAABBox.vcCenter;
		m_InstanceOBBox.vcA0.Set(1,0,0);
		m_InstanceOBBox.vcA1.Set(0,1,0);
		m_InstanceOBBox.vcA2.Set(0,0,1);
		Vector3D half_axis = pMF1Res->pModelMF1->m_MeshAABBox.vcMax - pMF1Res->pModelMF1->m_MeshAABBox.vcCenter;
		m_InstanceOBBox.fA0 = half_axis.x;
		m_InstanceOBBox.fA1 = half_axis.y;
		m_InstanceOBBox.fA2 = half_axis.z;
	}
}

void CStaticMeshInstance::updateOBB(const Matrix4x4& Matrix)
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
	{
		m_InstanceOBBox.DeTransform( OBBox(&pMF1Res->pModelMF1->m_MeshAABBox), Matrix );
	}
}

bool CStaticMeshInstance::isMeshVisible(int meshIndex)
{
	if( m_pCurrentConfig )
	{
		for( uint32 i=0; i<m_pCurrentConfig->MeshConfigNum; i++ )
			if( m_pCurrentConfig->pMeshConfigList[i].MeshID == (uint32)meshIndex )
				return m_pCurrentConfig->pMeshConfigList[i].bVisible;
	}
	return true;
}

bool CStaticMeshInstance::isParticleVisible(int partIndex)
{
	if( m_pCurrentConfig )
	{
		for( uint32 i=0; i<m_pCurrentConfig->ParticleConfigNum; i++ )
			if( m_pCurrentConfig->pParticleConfigList[i].ParticleID == (uint32)partIndex )
				return m_pCurrentConfig->pParticleConfigList[i].bVisible;
	}
	return true;
}

const uint32 CStaticMeshInstance::getMeshTriangleCount()
{
	CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;
	uint32 nTriangleCount = 0;

	jassert( pMF1Model );
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
	{
		nTriangleCount += pMF1Model->m_pLOD0Meshes[i].m_iNumIndices / 3;
	}
	return nTriangleCount;
}

const uint32 CStaticMeshInstance::getMeshVertexCount()
{
	CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;
	uint32 nVertexCount = 0;

	jassert( pMF1Model );
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
	{
		nVertexCount += pMF1Model->m_pLOD0Meshes[i].m_iNumVerts;
	}
	return nVertexCount;

}

Matrix4x4 CStaticMeshInstance::getBBRDMeshMatrix(int meshIndex)
{
	CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;
	
	jassert( pMF1Model );
	jassert( (meshIndex >= 0) && (meshIndex < (int)pMF1Model->m_Header.m_iNumMeshes) );


	Vector3D MeshCenter = pMF1Model->m_pLOD0Meshes[meshIndex].m_bbox.vcCenter;
	Matrix4x4 BBRDRotation, BBRDInvTrans, BBRDTrans;


	switch( pMF1Model->m_pLOD0Meshes[meshIndex].m_nType )
	{
	case SGPMESHCF_BBRD_CAMERAPOINT:
		{
			Matrix4x4 ViewMat;
			m_pRenderDevice->getViewMatrix(ViewMat);
			BBRDRotation.InverseOf(ViewMat);
			BBRDRotation._41 = 0;
			BBRDRotation._42 = 0;
			BBRDRotation._43 = 0;
		}
		break;
	case SGPMESHCF_BBRD:
		{
			Vector3D BBRDNormal(-pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertex[0].vNormal[0], -pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertex[0].vNormal[1], -pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertex[0].vNormal[2]);
			BBRDNormal.Normalize();

			Vector3D ViewDir;
			m_pRenderDevice->getCamreaViewDirection( &ViewDir );
			ViewDir.Normalize();

			float cost = ViewDir * BBRDNormal;
			Vector3D v;
			v.Cross( BBRDNormal, ViewDir);
			v.Normalize();
			float sint = std::sqrt(1-cost*cost);
			float one_sub_cost = 1 - cost;

			BBRDRotation.Identity();
			BBRDRotation._11 = v.x * v.x * one_sub_cost + cost;
			BBRDRotation._12 = v.x * v.y * one_sub_cost + v.z * sint;
			BBRDRotation._13 = v.x * v.z * one_sub_cost - v.y * sint;
			BBRDRotation._21 = v.x * v.y * one_sub_cost - v.z * sint;
			BBRDRotation._22 = v.y * v.y * one_sub_cost + cost;
			BBRDRotation._23 = v.y * v.z * one_sub_cost + v.x * sint;
			BBRDRotation._31 = v.x * v.z * one_sub_cost + v.y * sint;
			BBRDRotation._32 = v.y * v.z * one_sub_cost - v.x * sint;
			BBRDRotation._33 = v.z * v.z * one_sub_cost + cost;
		}
		break;
	case SGPMESHCF_BBRD_VERTICALGROUND:
		{
			Vector3D BBRDNormal(-pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertex[0].vNormal[0], 0, -pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertex[0].vNormal[2]);
			BBRDNormal.Normalize();

			Vector3D ViewDir;
			m_pRenderDevice->getCamreaViewDirection( &ViewDir );
			ViewDir.y = 0;
			ViewDir.Normalize();

			if( ViewDir.x < 0 )
				BBRDRotation.RotationY( float_2pi - BBRDNormal.AngleWith(ViewDir) );
			else				
				BBRDRotation.RotationY( BBRDNormal.AngleWith(ViewDir) );
		}
		break;
	}

	BBRDInvTrans.Identity();
	BBRDInvTrans.SetTranslation(-MeshCenter);
	BBRDTrans.Identity();
	BBRDTrans._11 = BBRDTrans._22 = BBRDTrans._33 = getScale();
	BBRDTrans.SetTranslation(MeshCenter * m_matModel);


	Matrix4x4 BBRDMatrix;
	BBRDMatrix = BBRDInvTrans * BBRDRotation * BBRDTrans;


	return BBRDMatrix;
}

