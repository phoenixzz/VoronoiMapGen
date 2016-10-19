
CMeshComponent::CMeshComponent(ISGPRenderDevice *pdevice, CSkeletonMeshInstance *pParent, SGP_ATTACHMENT_DEFINE part)
	: m_pRenderDevice(pdevice), m_pParentInstance(pParent),
	m_MF1ModelResourceID(0xFFFFFFFF),
	m_nAttachedBoneID(-1),
	m_nAttachedPart(part),
	m_BoneMatrixBuffer(NULL),
	m_pCurrentConfig(NULL),
	m_TBOID(0xFFFFFFFF)
{
	resetModel();
}

CMeshComponent::~CMeshComponent()
{
	if( m_BoneMatrixBuffer )
		delete [] m_BoneMatrixBuffer;
	m_BoneMatrixBuffer = NULL;
}


void CMeshComponent::resetModel()
{
	m_ModelFileName = String::empty;
	m_MF1ModelResourceID = 0xFFFFFFFF;

	if( m_BoneMatrixBuffer )
		delete [] m_BoneMatrixBuffer;
	m_BoneMatrixBuffer = NULL;

	m_nStartFrameTime = 0;
	m_nEndFrameTime = 0;
	m_fPlaySpeed = 1.0f;
	m_bLoopPlaying = false;
	m_fAnimPlayedTime = 0;

	m_bStatic = true;

	m_MF1ConfigIndex = 0;
	m_pCurrentConfig = NULL;

	m_ComponentMatrix.Identity();
	m_AttachInitMatrix.Identity();

	m_InstanceBatchConfig.Reset();
}

void CMeshComponent::destroyModel()
{
	setConfigSetting(NULL);

	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		if( m_pRenderDevice->isResLoadingMultiThread() )
			m_pRenderDevice->GetModelManager()->unRegisterModelByIDMT(m_MF1ModelResourceID);
		else
			m_pRenderDevice->GetModelManager()->unRegisterModelByID(m_MF1ModelResourceID);
	}

	m_pRenderDevice->GetVertexCacheManager()->ClearTextureBufferObject(m_TBOID);


	resetModel();
	m_TBOID = 0xFFFFFFFF;
}

void CMeshComponent::changeModel( const String& MF1ModelFileName, uint32 ConfigIndex )
{
	// If currently have resource in this Instance, first unregister
	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		destroyModel();
	}

	m_ModelFileName = MF1ModelFileName;
	m_MF1ConfigIndex = ConfigIndex;

	if( m_pRenderDevice->isResLoadingMultiThread() )
		m_MF1ModelResourceID = m_pRenderDevice->GetModelManager()->registerModelMT(MF1ModelFileName, true);
	else
		m_MF1ModelResourceID = m_pRenderDevice->GetModelManager()->registerModel(MF1ModelFileName, true);
	
	if( m_MF1ModelResourceID != 0xFFFFFFFF )
	{
		// Create TBO
		m_TBOID = m_pRenderDevice->GetVertexCacheManager()->CreateTextureBufferObjectByID(m_MF1ModelResourceID);
		CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
		if( pMF1Res && pMF1Res->pModelMF1 && pMF1Res->pModelMF1->m_iNumBones > 0 )
		{
			m_BoneMatrixBuffer = new float [pMF1Res->pModelMF1->m_iNumBones * 12];
			memset(m_BoneMatrixBuffer, 0, sizeof(float)*12*pMF1Res->pModelMF1->m_iNumBones);
			m_bStatic = (pMF1Res->pModelMF1->m_iNumBones == 0) || (pMF1Res->pModelMF1->m_iNumBoneGroup == 0);
		}

		// setting attached part
		CMF1FileResource* pParentMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_pParentInstance->getMF1ModelResourceID());
		if( pParentMF1Res && pParentMF1Res->pModelMF1 )
		{
			for( uint32 i=0; i<pParentMF1Res->pModelMF1->m_Header.m_iNumAttc; i++ )
			{
				if( pParentMF1Res->pModelMF1->m_pAttachTags[i].m_iAttachParts == m_nAttachedPart )
				{
					m_nAttachedBoneID = pParentMF1Res->pModelMF1->m_pAttachTags[i].m_iAttachBoneID;
					break;
				}
			}
		}
		if( m_nAttachedBoneID == -1 )
			Logger::getCurrentLogger()->writeToLog(String("addAttachment Error! Instance don't have attach point:")+String(g_AttachmentName[m_nAttachedPart]), ELL_ERROR);


		// Setting Config
		if( pMF1Res && pMF1Res->pModelMF1 && m_MF1ConfigIndex > 0 && m_MF1ConfigIndex <= pMF1Res->pModelMF1->m_Header.m_iNumConfigs )
			setConfigSetting(&pMF1Res->pModelMF1->m_pConfigSetting[m_MF1ConfigIndex-1]);

		buildOBB();

		// Check the Number of bones when not using Texture Buffer Object
		if( (m_TBOID == 0xFFFFFFFF) && (getBoneNum() > 40) )
		{
			Logger::getCurrentLogger()->writeToLog(String("Model File  ") + m_ModelFileName + String(" have too many bones, OpenGL ES 2.0 only support 40 bones!"), ELL_ERROR);
			destroyModel();
		}
	}
}

void CMeshComponent::render()
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

				if( m_bStatic )
				{
					if( pMF1Res->pModelMF1->m_pLOD0Meshes[i].m_nType >= static_cast<uint32>(SGPMESHCF_BBRD) &&
						pMF1Res->pModelMF1->m_pLOD0Meshes[i].m_nType <= static_cast<uint32>(SGPMESHCF_BBRD_VERTICALGROUND) )
						m_pRenderDevice->GetVertexCacheManager()->RenderStaticBuffer( pMF1Res->StaticMeshIDArray[i], getStaticBBRDMeshMatrix(i), m_InstanceBatchConfig );
					else
						m_pRenderDevice->GetVertexCacheManager()->RenderStaticBuffer( pMF1Res->StaticMeshIDArray[i], m_ComponentMatrix, m_InstanceBatchConfig );

				}
				else
				{
					if(	pMF1Res->pModelMF1->m_pLOD0Meshes[i].m_nType >= static_cast<uint32>(SGPMESHCF_BBRD) &&
						pMF1Res->pModelMF1->m_pLOD0Meshes[i].m_nType <= static_cast<uint32>(SGPMESHCF_BBRD_VERTICALGROUND) )
					{
						if( m_TBOID == 0xFFFFFFFF )
							m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], getSkeletonBBRDMeshMatrix(i), m_BoneMatrixBuffer, getBoneNum(), m_InstanceBatchConfig);
						else
							m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], getSkeletonBBRDMeshMatrix(i), m_TBOID, m_InstanceBatchConfig);
					}
					else
					{
						if( m_TBOID == 0xFFFFFFFF )
							m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], m_ComponentMatrix, m_BoneMatrixBuffer, getBoneNum(), m_InstanceBatchConfig);
						else
							m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], m_ComponentMatrix, m_TBOID, m_InstanceBatchConfig);
					}
				}
			}
		}
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
		{
			if( isParticleVisible(i) )
				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->render();
		}
	}
}

bool CMeshComponent::update( float deltaTimeinSeconds )
{
	if( m_pRenderDevice->isResLoadingMultiThread() && (m_MF1ModelResourceID == 0xFFFFFFFF) )
	{
		m_MF1ModelResourceID = m_pRenderDevice->GetModelManager()->getModelIDByName(m_ModelFileName);
		if( m_MF1ModelResourceID != 0xFFFFFFFF )
		{
			m_TBOID = m_pRenderDevice->GetVertexCacheManager()->CreateTextureBufferObjectByID(m_MF1ModelResourceID);
			CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
			if( pMF1Res && pMF1Res->pModelMF1 && pMF1Res->pModelMF1->m_iNumBones > 0 )
			{
				m_BoneMatrixBuffer = new float [pMF1Res->pModelMF1->m_iNumBones * 12];
				memset(m_BoneMatrixBuffer, 0, sizeof(float)*12*pMF1Res->pModelMF1->m_iNumBones);
					
				m_bStatic = (pMF1Res->pModelMF1->m_iNumBones == 0) || (pMF1Res->pModelMF1->m_iNumBoneGroup == 0);
			}

			// setting attached part
			CMF1FileResource* pParentMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_pParentInstance->getMF1ModelResourceID());
			if( pParentMF1Res && pParentMF1Res->pModelMF1 )
			{
				for( uint32 i=0; i<pParentMF1Res->pModelMF1->m_Header.m_iNumAttc; i++ )
				{
					if( pParentMF1Res->pModelMF1->m_pAttachTags[i].m_iAttachParts == m_nAttachedPart )
					{
						m_nAttachedBoneID = pParentMF1Res->pModelMF1->m_pAttachTags[i].m_iAttachBoneID;
						m_AttachInitMatrix = pParentMF1Res->pModelMF1->m_pAttachTags[i].m_InitMatTag;
						break;
					}
				}
			}
			if( m_nAttachedBoneID == -1 )
				Logger::getCurrentLogger()->writeToLog(String("addAttachment Error! Instance don't have attach point:")+String(g_AttachmentName[m_nAttachedPart]), ELL_ERROR);

			// Setting Config
			if( pMF1Res && pMF1Res->pModelMF1 && m_MF1ConfigIndex > 0 && m_MF1ConfigIndex <= pMF1Res->pModelMF1->m_Header.m_iNumConfigs )
				setConfigSetting(&pMF1Res->pModelMF1->m_pConfigSetting[m_MF1ConfigIndex-1]);

			buildOBB();

			// Check the Number of bones when not using Texture Buffer Object
			if( (m_TBOID == 0xFFFFFFFF) && (getBoneNum() > 40) )
			{
				Logger::getCurrentLogger()->writeToLog(String("Model File  ") + m_ModelFileName + String(" have too many bones, OpenGL ES 2.0 only support 40 bones!"), ELL_ERROR);
				destroyModel();
			}
		}
	}

	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return false;

	float* pParentBoneMatrix = m_pParentInstance->getBonesMatrix();
	Matrix4x4 matTemp;
	matTemp.Identity();
	matTemp._11 = pParentBoneMatrix[12 * m_nAttachedBoneID     ];
	matTemp._21 = pParentBoneMatrix[12 * m_nAttachedBoneID +  1];
	matTemp._31 = pParentBoneMatrix[12 * m_nAttachedBoneID +  2];
	matTemp._41 = pParentBoneMatrix[12 * m_nAttachedBoneID +  3];
	matTemp._12 = pParentBoneMatrix[12 * m_nAttachedBoneID +  4];
	matTemp._22 = pParentBoneMatrix[12 * m_nAttachedBoneID +  5];
	matTemp._32 = pParentBoneMatrix[12 * m_nAttachedBoneID +  6];
	matTemp._42 = pParentBoneMatrix[12 * m_nAttachedBoneID +  7];
	matTemp._13 = pParentBoneMatrix[12 * m_nAttachedBoneID +  8];
	matTemp._23 = pParentBoneMatrix[12 * m_nAttachedBoneID +  9];
	matTemp._33 = pParentBoneMatrix[12 * m_nAttachedBoneID + 10];
	matTemp._43 = pParentBoneMatrix[12 * m_nAttachedBoneID + 11];


	m_ComponentMatrix = m_AttachInitMatrix * matTemp * m_pParentInstance->getModelMatrix();

	// Update Instance OOBB boundingbox
	updateOBB(m_ComponentMatrix);



	///////////////////////////////////////////////////////////////////////
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

	// If UV Anim, Update StaticBuffer Skin Time 
	if( pMF1Res->pModelMF1->m_Header.m_iUVAnim > 0 )
	{
		m_InstanceBatchConfig.m_fTimePassedFromCreated += deltaTimeinSeconds;
	}
	// Update Component Alpha using parent's Instance alpha
	m_InstanceBatchConfig.m_fBatchAlpha = m_pParentInstance->getInstanceAlpha();

	if( m_bStatic )
	{
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
		{
			if( isParticleVisible(i) )
			{
				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->
					updateAbsolutePosition(pMF1Res->pModelMF1->m_pParticleEmitter[i].m_AbsoluteMatrix * m_ComponentMatrix);

				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->update(deltaTimeinSeconds);
			}
		}
		return true;
	}

	// Update Anim Start time and End time
	if( (m_nStartFrameTime == 0) && (m_nEndFrameTime == 0) )
	{
		uint32 StartFrameID = pMF1Res->pModelMF1->m_pActionLists[0].m_iActionStart;
		uint32 EndFrameID = pMF1Res->pModelMF1->m_pActionLists[0].m_iActionEnd;
		
		for(uint32 i=1; i < pMF1Res->pModelMF1->m_Header.m_iNumActionList; ++i)
		{
			if( pMF1Res->pModelMF1->m_pActionLists[i].m_iActionStart < StartFrameID )
				StartFrameID = pMF1Res->pModelMF1->m_pActionLists[i].m_iActionStart;
			if( pMF1Res->pModelMF1->m_pActionLists[i].m_iActionEnd > EndFrameID )
				EndFrameID = pMF1Res->pModelMF1->m_pActionLists[i].m_iActionEnd;
		}
		m_nStartFrameTime = StartFrameID * ISGPInstanceManager::DefaultSecondsPerFrame;
		m_nEndFrameTime = EndFrameID * ISGPInstanceManager::DefaultSecondsPerFrame;
	}

	static float fLastTime = m_nStartFrameTime;

	float fTime = deltaTimeinSeconds * m_fPlaySpeed;
	fTime += fLastTime;
	fLastTime = fTime;

	if( m_nStartFrameTime == m_nEndFrameTime )	// single frame playing
	{
		fLastTime = m_nStartFrameTime;
		fTime = m_nStartFrameTime;
	}
	else if(fTime > m_nEndFrameTime)			// looping playing
	{
		if(m_bLoopPlaying)
		{
			fLastTime = m_nStartFrameTime;
			fTime = m_nStartFrameTime;
		}
		else
			fTime = m_nEndFrameTime;
	}
	m_fAnimPlayedTime = fTime;


	Vector3D vTranslation;
	Quaternion vRotationQuat;
	float fScale = 1.0f;

	if( pMF1Res && pMF1Res->pModelMF1 && (pMF1Res->pModelMF1->m_iNumBones > 0) )
	{
		for(uint32 x = 0; x < pMF1Res->pModelMF1->m_iNumBones; x++)
		{
			const SGPMF1Bone *pBone = &pMF1Res->pModelMF1->m_pBones[x];

			vTranslation = calculateCurrentTranslation(pBone, fTime);
			vRotationQuat = calculateCurrentRotation(pBone, fTime);
			fScale = calculateCurrentScale(pBone, fTime);

			Matrix4x4 BoneTransMat;
			BoneTransMat.Identity();
			BoneTransMat._11 = BoneTransMat._22 = BoneTransMat._33 = fScale;
			Matrix4x4 matRot;
			//Convert the quaternion to a rotation matrix
			vRotationQuat.GetMatrix( &matRot );
			BoneTransMat = BoneTransMat * matRot;
			BoneTransMat.SetTranslation( vTranslation );

			matTemp = pBone->m_matFrame0Inv * BoneTransMat;

			m_BoneMatrixBuffer[12 * x     ] = matTemp._11;  
            m_BoneMatrixBuffer[12 * x +  1] = matTemp._21;  
            m_BoneMatrixBuffer[12 * x +  2] = matTemp._31;  
            m_BoneMatrixBuffer[12 * x +  3] = matTemp._41;  
            m_BoneMatrixBuffer[12 * x +  4] = matTemp._12;  
            m_BoneMatrixBuffer[12 * x +  5] = matTemp._22;  
            m_BoneMatrixBuffer[12 * x +  6] = matTemp._32;  
            m_BoneMatrixBuffer[12 * x +  7] = matTemp._42;  
            m_BoneMatrixBuffer[12 * x +  8] = matTemp._13;  
            m_BoneMatrixBuffer[12 * x +  9] = matTemp._23;  
            m_BoneMatrixBuffer[12 * x + 10] = matTemp._33;  
            m_BoneMatrixBuffer[12 * x + 11] = matTemp._43;



			for(uint32 w = 0; w < pMF1Res->pModelMF1->m_Header.m_iNumParticles; w++)
			{
				if( pMF1Res->pModelMF1->m_pParticleEmitter[w].m_iAttachBoneID == x )
				{
					m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[w])->
						updateAbsolutePosition(pMF1Res->pModelMF1->m_pParticleEmitter[w].m_AbsoluteMatrix * matTemp * m_ComponentMatrix);
				}
			}


		}

		m_pRenderDevice->GetVertexCacheManager()->UpdateTextureBufferObjectByID(m_BoneMatrixBuffer, pMF1Res->pModelMF1->m_iNumBones, m_TBOID);
	
		//Update Attachment and Particle
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
		{
			if( isParticleVisible(i) )
				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->update(deltaTimeinSeconds);
		}
	}

	return true;
}

void CMeshComponent::playAnim(float fSpeed, bool bLoop)
{
	m_fPlaySpeed = fSpeed;
	m_bLoopPlaying = bLoop;
	m_fAnimPlayedTime = 0;
}

void CMeshComponent::setConfigSetting(SGPMF1ConfigSetting* pConfigSetting)
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





void CMeshComponent::buildOBB()
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
	{
		m_ComponentOBBox.vcCenter = pMF1Res->pModelMF1->m_MeshAABBox.vcCenter;
		m_ComponentOBBox.vcA0.Set(1,0,0);
		m_ComponentOBBox.vcA1.Set(0,1,0);
		m_ComponentOBBox.vcA2.Set(0,0,1);
		Vector3D half_axis = pMF1Res->pModelMF1->m_MeshAABBox.vcMax - pMF1Res->pModelMF1->m_MeshAABBox.vcCenter;
		m_ComponentOBBox.fA0 = half_axis.x;
		m_ComponentOBBox.fA1 = half_axis.y;
		m_ComponentOBBox.fA2 = half_axis.z;
	}
}

void CMeshComponent::updateOBB(const Matrix4x4& Matrix)
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
	{
		m_ComponentOBBox.DeTransform( OBBox(&pMF1Res->pModelMF1->m_MeshAABBox), Matrix );
	}
}

bool CMeshComponent::isMeshVisible(int meshIndex)
{
	if( m_pCurrentConfig )
	{
		for( uint32 i=0; i<m_pCurrentConfig->MeshConfigNum; i++ )
			if( m_pCurrentConfig->pMeshConfigList[i].MeshID == (uint32)meshIndex )
				return m_pCurrentConfig->pMeshConfigList[i].bVisible;
	}
	return true;
}

bool CMeshComponent::isParticleVisible(int partIndex)
{
	if( m_pCurrentConfig )
	{
		for( uint32 i=0; i<m_pCurrentConfig->ParticleConfigNum; i++ )
			if( m_pCurrentConfig->pParticleConfigList[i].ParticleID == (uint32)partIndex )
				return m_pCurrentConfig->pParticleConfigList[i].bVisible;
	}
	return true;
}

bool CMeshComponent::isBoneVisible(uint16 nBoneID)
{
	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return false;

	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

	if( nBoneID >= pMF1Res->pModelMF1->m_iNumBones )
		return false;

	const SGPMF1Bone *pBone = &pMF1Res->pModelMF1->m_pBones[nBoneID];
	if( pBone )
		return calculateCurrentVisible(pBone, m_fAnimPlayedTime);

	return false;
}

uint32 CMeshComponent::getBoneNum()
{
	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return 0;

	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	return pMF1Res->pModelMF1->m_iNumBones;
}


Vector3D CMeshComponent::calculateCurrentTranslation(const SGPMF1Bone *pBone, float fTime)
{
	Vector3D vTrans;
	float fInterp = 0.0f;

	//if there are one keyframes, don't do any transformations
	if( pBone->m_TransKeyFrames && (pBone->m_TransKeyFrames->m_nextBlock == NULL) &&
		(pBone->m_TransKeyFrames->m_iNumber == 1) )
	{
		vTrans.Set( pBone->m_TransKeyFrames->m_KeyFrames[0].m_fParam[0],
					pBone->m_TransKeyFrames->m_KeyFrames[0].m_fParam[1],
					pBone->m_TransKeyFrames->m_KeyFrames[0].m_fParam[2] );
	}
	else if( pBone->m_TransKeyFrames && (pBone->m_TransKeyFrames->m_iNumber > 1) )
	{
		SGPMF1KeyFrame *pLastKeyFrame = pBone->m_TransKeyFrames->m_KeyFrames;


		//Calculate the current Translation frame
		KeyFrameBlock *pCurTransBlock = pBone->m_TransKeyFrames;
		while( pCurTransBlock != NULL )
		{
			for( int i=0; i<pCurTransBlock->m_iNumber; i++ )
			{
				if( pCurTransBlock->m_KeyFrames[i].m_iFrameID * ISGPInstanceManager::DefaultSecondsPerFrame >= fTime )
				{
					if( pCurTransBlock->m_KeyFrames[i].m_iFrameID == pLastKeyFrame->m_iFrameID )
						fInterp = 0;
					else
						fInterp = (fTime / ISGPInstanceManager::DefaultSecondsPerFrame - pLastKeyFrame->m_iFrameID) /
							float(pCurTransBlock->m_KeyFrames[i].m_iFrameID - pLastKeyFrame->m_iFrameID);

					vTrans.Set( pLastKeyFrame->m_fParam[0] + (pCurTransBlock->m_KeyFrames[i].m_fParam[0] - pLastKeyFrame->m_fParam[0]) * fInterp,
								pLastKeyFrame->m_fParam[1] + (pCurTransBlock->m_KeyFrames[i].m_fParam[1] - pLastKeyFrame->m_fParam[1]) * fInterp,
								pLastKeyFrame->m_fParam[2] + (pCurTransBlock->m_KeyFrames[i].m_fParam[2] - pLastKeyFrame->m_fParam[2]) * fInterp );

					return vTrans;
				}
				pLastKeyFrame = &pCurTransBlock->m_KeyFrames[i];
			}
			pCurTransBlock = pCurTransBlock->m_nextBlock;
		}
	}
	return vTrans;
}

Quaternion CMeshComponent::calculateCurrentRotation(const SGPMF1Bone *pBone, float fTime)
{
	Quaternion vRots;
	float fInterp = 0.0f;

	//if there are one keyframes, don't do any transformations
	if( pBone->m_RotsKeyFrames && (pBone->m_RotsKeyFrames->m_nextBlock == NULL) &&
		(pBone->m_RotsKeyFrames->m_iNumber == 1) )
	{
		return Quaternion(  pBone->m_RotsKeyFrames->m_KeyFrames[0].m_fParam[0],
							pBone->m_RotsKeyFrames->m_KeyFrames[0].m_fParam[1],
							pBone->m_RotsKeyFrames->m_KeyFrames[0].m_fParam[2],
							pBone->m_RotsKeyFrames->m_KeyFrames[0].m_fParam[3] );
	}
	else if( pBone->m_RotsKeyFrames && (pBone->m_RotsKeyFrames->m_iNumber > 1) )
	{
		SGPMF1KeyFrame *pLastKeyFrame = pBone->m_RotsKeyFrames->m_KeyFrames;


		//Calculate the current Rotation frame
		KeyFrameBlock *pCurRotsBlock = pBone->m_RotsKeyFrames;
		while( pCurRotsBlock != NULL )
		{
			for( int i=0; i<pCurRotsBlock->m_iNumber; i++ )
			{
				if( pCurRotsBlock->m_KeyFrames[i].m_iFrameID * ISGPInstanceManager::DefaultSecondsPerFrame >= fTime )
				{
					if( pCurRotsBlock->m_KeyFrames[i].m_iFrameID == pLastKeyFrame->m_iFrameID )
						fInterp = 0;
					else
						fInterp = (fTime / ISGPInstanceManager::DefaultSecondsPerFrame - pLastKeyFrame->m_iFrameID) /
							float(pCurRotsBlock->m_KeyFrames[i].m_iFrameID - pLastKeyFrame->m_iFrameID);

					Quaternion vCurRots( pCurRotsBlock->m_KeyFrames[i].m_fParam[0], 
						pCurRotsBlock->m_KeyFrames[i].m_fParam[1], 
						pCurRotsBlock->m_KeyFrames[i].m_fParam[2], 
						pCurRotsBlock->m_KeyFrames[i].m_fParam[3] );
					vRots.x = pLastKeyFrame->m_fParam[0];
					vRots.y = pLastKeyFrame->m_fParam[1];
					vRots.z = pLastKeyFrame->m_fParam[2];
					vRots.w = pLastKeyFrame->m_fParam[3];

					vRots.Slerp(vCurRots, fInterp);

					return vRots;
				}
				pLastKeyFrame = &pCurRotsBlock->m_KeyFrames[i];
			}
			pCurRotsBlock = pCurRotsBlock->m_nextBlock;
		}
	}
	return vRots;
}

float CMeshComponent::calculateCurrentScale(const SGPMF1Bone *pBone, float fTime)
{
	float scale = 1.0f;
	float fInterp = 0.0f;

	//if there are one keyframes, don't do any transformations
	if( pBone->m_ScaleKeyFrames && (pBone->m_ScaleKeyFrames->m_nextBlock == NULL) &&
		(pBone->m_ScaleKeyFrames->m_iNumber == 1) )
	{
		return pBone->m_ScaleKeyFrames->m_KeyFrames[0].m_scale;
	}
	else if( pBone->m_ScaleKeyFrames && (pBone->m_ScaleKeyFrames->m_iNumber > 1) )
	{
		SGPMF1ScaleKeyFrame *pLastKeyFrame = pBone->m_ScaleKeyFrames->m_KeyFrames;


		//Calculate the current scale frame
		ScaleKeyFrameBlock *pCurScaleBlock = pBone->m_ScaleKeyFrames;
		while( pCurScaleBlock != NULL )
		{
			for( int i=0; i<pCurScaleBlock->m_iNumber; i++ )
			{
				if( pCurScaleBlock->m_KeyFrames[i].m_iFrameID * ISGPInstanceManager::DefaultSecondsPerFrame >= fTime )
				{
					if( pCurScaleBlock->m_KeyFrames[i].m_iFrameID == pLastKeyFrame->m_iFrameID )
						fInterp = 0;
					else
						fInterp = (fTime / ISGPInstanceManager::DefaultSecondsPerFrame - pLastKeyFrame->m_iFrameID) /
							float(pCurScaleBlock->m_KeyFrames[i].m_iFrameID - pLastKeyFrame->m_iFrameID);

					scale = pLastKeyFrame->m_scale + (pCurScaleBlock->m_KeyFrames[i].m_scale - pLastKeyFrame->m_scale) * fInterp;

					return scale;
				}
				pLastKeyFrame = &pCurScaleBlock->m_KeyFrames[i];
			}
			pCurScaleBlock = pCurScaleBlock->m_nextBlock;
		}

	}
	return scale;
}

bool CMeshComponent::calculateCurrentVisible(const SGPMF1Bone *pBone, float fTime)
{
	bool bVisible = true;

	//if there are one keyframes, don't do anything
	if( pBone->m_VisibleKeyFrames && (pBone->m_VisibleKeyFrames->m_nextBlock == NULL) &&
		(pBone->m_VisibleKeyFrames->m_iNumber == 1) )
	{
		return pBone->m_VisibleKeyFrames->m_KeyFrames[0].m_Visible;
	}
	else if( pBone->m_VisibleKeyFrames && (pBone->m_VisibleKeyFrames->m_iNumber > 1) )
	{
		bVisible = pBone->m_VisibleKeyFrames->m_KeyFrames[0].m_Visible;

		//Calculate the current visible frame
		VisibleKeyFrameBlock *pCurVisibleBlock = pBone->m_VisibleKeyFrames;
		while( pCurVisibleBlock != NULL )
		{
			for( int i=0; i<pCurVisibleBlock->m_iNumber; i++ )
			{
				if( pCurVisibleBlock->m_KeyFrames[i].m_iFrameID * ISGPInstanceManager::DefaultSecondsPerFrame == fTime )
					return pCurVisibleBlock->m_KeyFrames[i].m_Visible;

				if( pCurVisibleBlock->m_KeyFrames[i].m_iFrameID * ISGPInstanceManager::DefaultSecondsPerFrame > fTime )
					return bVisible;

				bVisible = pCurVisibleBlock->m_KeyFrames[i].m_Visible;
			}
			pCurVisibleBlock = pCurVisibleBlock->m_nextBlock;
		}

	}
	return bVisible;
}




Matrix4x4 CMeshComponent::getStaticBBRDMeshMatrix(int meshIndex)
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
	BBRDTrans._11 = m_ComponentMatrix.GetScaleX();
	BBRDTrans._22 = m_ComponentMatrix.GetScaleY();
	BBRDTrans._33 = m_ComponentMatrix.GetScaleZ();
	BBRDTrans.SetTranslation( MeshCenter * m_ComponentMatrix );

	Matrix4x4 BBRDMatrix;
	BBRDMatrix = BBRDInvTrans * BBRDRotation * BBRDTrans;


	return BBRDMatrix;
}

Matrix4x4 CMeshComponent::getSkeletonBBRDMeshMatrix(int meshIndex)
{
	CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;
	
	jassert( pMF1Model );
	jassert( (meshIndex >= 0) && (meshIndex < (int)pMF1Model->m_Header.m_iNumMeshes) );

	// NOTE: Now bone id of BBRD is default vertex 0 bone id and only one bone weight
	uint16 BBRDBoneID0 = pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertexBoneGroupID[0];
	jassert(BBRDBoneID0 == pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertexBoneGroupID[1]);
	jassert(BBRDBoneID0 == pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertexBoneGroupID[2]);
	jassert(BBRDBoneID0 == pMF1Model->m_pLOD0Meshes[meshIndex].m_pVertexBoneGroupID[3]);

	jassert( pMF1Model->m_pBoneGroup );

	uint8 BBRDBoneIndex = pMF1Model->m_pBoneGroup[BBRDBoneID0].BoneIndex.x;

	Matrix4x4 MeshCenterTrans;
	MeshCenterTrans.Identity();
	MeshCenterTrans._11 = m_BoneMatrixBuffer[12 * BBRDBoneIndex     ];
	MeshCenterTrans._21 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  1];
	MeshCenterTrans._31 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  2];
	MeshCenterTrans._41 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  3];
	MeshCenterTrans._12 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  4];
	MeshCenterTrans._22 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  5];
	MeshCenterTrans._32 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  6];
	MeshCenterTrans._42 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  7];
	MeshCenterTrans._13 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  8];
	MeshCenterTrans._23 = m_BoneMatrixBuffer[12 * BBRDBoneIndex +  9];
	MeshCenterTrans._33 = m_BoneMatrixBuffer[12 * BBRDBoneIndex + 10];
	MeshCenterTrans._43 = m_BoneMatrixBuffer[12 * BBRDBoneIndex + 11];


	Vector3D MeshCenter = pMF1Model->m_pLOD0Meshes[meshIndex].m_bbox.vcCenter;
	Matrix4x4 BBRDRotation, BBRDInvTrans, MeshTrans;


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

	MeshTrans.Identity();
	MeshTrans._11 = m_ComponentMatrix.GetScaleX();
	MeshTrans._22 = m_ComponentMatrix.GetScaleY();
	MeshTrans._33 = m_ComponentMatrix.GetScaleZ();
	MeshTrans.SetTranslation(MeshCenter * MeshCenterTrans * m_ComponentMatrix);

	Matrix4x4 BBRDMatrix;
	BBRDMatrix = BBRDInvTrans * BBRDRotation * MeshTrans;

	return BBRDMatrix;
}
