
CSkeletonMeshInstance::CSkeletonMeshInstance(ISGPRenderDevice *pdevice) :
	m_pRenderDevice(pdevice), m_vPosition(0,0,0), m_vRotationXYZ(0,0,0), 
	m_fScale(1.0f),
	m_MF1ModelResourceID(0xFFFFFFFF),
	m_BoneMatrixBuffer(NULL),
	m_pBlendFrameMatrix(NULL),
	m_pUpperBodyBlendFrameMatrix(NULL),
	m_pCurrentConfig(NULL),
	m_bVisible(false),
	m_fInstanceRenderAlpha(1.0f),
	m_fOldInstanceRenderAlpha(1.0f),
	m_fInstanceRenderScale(1.0f),
	m_fOldInstanceRenderScale(1.0f),
	m_TBOID(0xFFFFFFFF)
{
	resetModel();
}

CSkeletonMeshInstance::~CSkeletonMeshInstance()
{
	if( m_BoneMatrixBuffer )
		delete [] m_BoneMatrixBuffer;
	m_BoneMatrixBuffer = NULL;

	if( m_pBlendFrameMatrix )
		delete [] m_pBlendFrameMatrix;
	m_pBlendFrameMatrix = NULL;
	if( m_pUpperBodyBlendFrameMatrix )
		delete [] m_pUpperBodyBlendFrameMatrix;
	m_pUpperBodyBlendFrameMatrix = NULL;

	// Attachment
	for( int i=0; i<SGPATTDEF_MAXATTACHMENT; i++ )
		detachAttachment( static_cast<SGP_ATTACHMENT_DEFINE>(i) );

}

void CSkeletonMeshInstance::resetModel()
{
	m_ModelFileName = String::empty;
	m_MF1ModelResourceID = 0xFFFFFFFF;

	if( m_BoneMatrixBuffer )
		delete [] m_BoneMatrixBuffer;
	m_BoneMatrixBuffer = NULL;

	if( m_pBlendFrameMatrix )
		delete [] m_pBlendFrameMatrix;
	m_pBlendFrameMatrix = NULL;
	if( m_pUpperBodyBlendFrameMatrix )
		delete [] m_pUpperBodyBlendFrameMatrix;
	m_pUpperBodyBlendFrameMatrix = NULL;

	m_fActionBlendAbsoluteTime = 0;
	m_fActionBlendPassageTime = 0;
	m_fUpperBodyActionBlendAbsoluteTime = 0;
	m_fUpperBodyActionBlendPassageTime = 0;


	m_matModel.Identity();
	//m_matUpperBodyRotYFromLowerBody.Identity();

	m_fLastTime = 0;
	m_fUpperLastTime = 0;

	m_nStartFrameTime = 0;
	m_nEndFrameTime = 0;
	m_fAnimPlayedTime = 0;
	m_fPlaySpeed = 1.0f;
	m_bLoopPlaying = false;

	m_nUpperStartFrameTime = 0;
	m_nUpperEndFrameTime = 0;
	m_fUpperAnimPlayedTime = 0;
	m_fUpperPlaySpeed = 1.0f;
	m_bUpperLoopPlaying = false;

	m_bEnableUpperBodyAnim = false;
	m_bPlayingUpperBodyAnim = false;

	m_MF1ConfigIndex = 0;
	m_RenderFlagEx = 0;

	m_bVisible = false;
	m_fInstanceRenderAlpha = 1.0f;
	m_fOldInstanceRenderAlpha = 1.0f;
	m_fInstanceRenderScale = 1.0f;	
	m_fOldInstanceRenderScale = 1.0f;

	m_pCurrentConfig = NULL;

	m_InstanceBatchConfig.Reset();

	// Attachment
	for( int i=0; i<SGPATTDEF_MAXATTACHMENT; i++ )
		m_pAttachedComponents[i] = NULL;
}

void CSkeletonMeshInstance::destroyModel()
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


	// Attachment
	for( int i=0; i<SGPATTDEF_MAXATTACHMENT; i++ )
		detachAttachment( static_cast<SGP_ATTACHMENT_DEFINE>(i) );

	resetModel();
	m_TBOID = 0xFFFFFFFF;
}

void CSkeletonMeshInstance::changeModel( const String& MF1ModelFileName, uint32 ConfigIndex )
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
		}

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



void CSkeletonMeshInstance::render()
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
				{
					// Use traditional vertex shader vertex uniform vectors method
					if( m_TBOID == 0xFFFFFFFF )
						m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], getBBRDMeshMatrix(i), m_BoneMatrixBuffer, getBoneNum(), m_InstanceBatchConfig);
					else	// Use Texture Buffer Object method
						m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], getBBRDMeshMatrix(i), m_TBOID, m_InstanceBatchConfig);
				}
				else
				{
					if( m_TBOID == 0xFFFFFFFF )
						m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], m_matModel, m_BoneMatrixBuffer, getBoneNum(), m_InstanceBatchConfig);
					else	// Use Texture Buffer Object method
						m_pRenderDevice->GetVertexCacheManager()->RenderSkeletonMesh(pMF1Res->StaticMeshIDArray[i], m_matModel, m_TBOID, m_InstanceBatchConfig);
				}
		
			}
		}
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
		{
			if( isParticleVisible(i) )
				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->render();
		}
		// Attachment
		for( int i=0; i<SGPATTDEF_MAXATTACHMENT; i++ )
		{
			if( m_pAttachedComponents[i] )
				m_pAttachedComponents[i]->render();
		}
	}
}

bool CSkeletonMeshInstance::update( float deltaTimeinSeconds )
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
			}

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

	// Update Instance Model Transform Matrix
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

	if( !m_bVisible )
		return true;

	///////////////////////////////////////////////////////////////////////
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

	if (!pMF1Res || !pMF1Res->pModelMF1)
		return true;

	// If UV Anim, Update StaticBuffer Skin Time 
	if( pMF1Res->pModelMF1->m_Header.m_iUVAnim > 0 )
	{
		m_InstanceBatchConfig.m_fTimePassedFromCreated += deltaTimeinSeconds;
	}
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


	// Lower Body Animation
	float fTime = deltaTimeinSeconds * m_fPlaySpeed;
	fTime += m_fLastTime;
	m_fLastTime = fTime;
	m_fActionBlendPassageTime += deltaTimeinSeconds;


	if( m_nStartFrameTime == m_nEndFrameTime )	// single frame playing
	{
		m_fLastTime = m_nStartFrameTime;
		fTime = m_nStartFrameTime;
	}
	else if(fTime > m_nEndFrameTime)			// looping playing
	{
		if(m_bLoopPlaying)
		{
			m_fLastTime = m_nStartFrameTime;
			fTime = m_nStartFrameTime;
		}
		else
			fTime = m_nEndFrameTime;
	}
	m_fAnimPlayedTime = fTime;

	// Upper Body Animation
	float fUpperTime = deltaTimeinSeconds * m_fUpperPlaySpeed;
	fUpperTime += m_fUpperLastTime;
	m_fUpperLastTime = fUpperTime;
	m_fUpperBodyActionBlendPassageTime += deltaTimeinSeconds;

	if( m_nUpperStartFrameTime == m_nUpperEndFrameTime )		// single frame playing
	{
		m_fUpperLastTime = m_nUpperStartFrameTime;
		fUpperTime = m_nUpperStartFrameTime;
	}
	else if(fUpperTime > m_nUpperEndFrameTime)					// looping playing
	{
		if(m_bUpperLoopPlaying)
		{
			m_fUpperLastTime = m_nUpperStartFrameTime;
			fUpperTime = m_nUpperStartFrameTime;
		}
		else
			fUpperTime = m_nUpperEndFrameTime;
	}
	m_fUpperAnimPlayedTime = fUpperTime;


	m_bPlayingUpperBodyAnim = m_bEnableUpperBodyAnim && (pMF1Res->pModelMF1->m_Header.m_iBipBoneID != -1);


	Vector3D vUpperBodyOffset;
	Vector3D vTranslation;
	Quaternion vRotationQuat;
	float fScale = 1.0f;

	if( pMF1Res && pMF1Res->pModelMF1 && (pMF1Res->pModelMF1->m_iNumBones > 0) )
	{
		// If having upper anim, first calculate Upper Body Offset vector from Lower Body
		if( m_bPlayingUpperBodyAnim )
		{
			const SGPMF1Bone *pBone = &pMF1Res->pModelMF1->m_pBones[pMF1Res->pModelMF1->m_Header.m_iBipBoneID];
			Vector3D vUpperTranslation = calculateCurrentTranslation(pBone, fUpperTime);
			Vector3D vLowerTranslation = calculateCurrentTranslation(pBone, fTime);
			vUpperBodyOffset = vLowerTranslation - vUpperTranslation;
		}
		else
			vUpperBodyOffset.Set(0, 0, 0);


		for(uint32 x = 0; x < pMF1Res->pModelMF1->m_iNumBones; x++)
		{
			const SGPMF1Bone *pBone = &pMF1Res->pModelMF1->m_pBones[x];

			if( m_bPlayingUpperBodyAnim && ( pBone->m_bUpperBone == 1 ) )
			{
				// Upper Body Animation update
				vTranslation = calculateCurrentTranslation(pBone, fUpperTime);
				vRotationQuat = calculateCurrentRotation(pBone, fUpperTime);
				fScale = calculateCurrentScale(pBone, fUpperTime);

				Matrix4x4 BoneTransMat;
				BoneTransMat.Identity();
				BoneTransMat._11 = BoneTransMat._22 = BoneTransMat._33 = fScale;
				Matrix4x4 matRot;
				//Convert the quaternion to a rotation matrix
				vRotationQuat.GetMatrix( &matRot );
				BoneTransMat = BoneTransMat * matRot;
				BoneTransMat.SetTranslation( vTranslation );

				matTemp = pBone->m_matFrame0Inv * BoneTransMat;

				// In local space, upper body roattion from lower body
				//matTemp = matTemp * m_matUpperBodyRotYFromLowerBody;

				matTemp._41 += vUpperBodyOffset.x;
				matTemp._42 += vUpperBodyOffset.y;
				matTemp._43 += vUpperBodyOffset.z;

				// Action Blending
				if( m_fUpperBodyActionBlendAbsoluteTime > 0.0f )
				{
					float t = 1.0f - m_fUpperBodyActionBlendPassageTime / m_fUpperBodyActionBlendAbsoluteTime;
					if( t < 0.0f )
						m_fUpperBodyActionBlendAbsoluteTime = 0;			// Finish Blend action
					else
						matTemp.Lerp(m_pUpperBodyBlendFrameMatrix[x], t);	// linear Lerp bone matrix
				}
			}
			else
			{
				// Lower Body Animation update
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

				// Action Blending
				if( m_fActionBlendAbsoluteTime > 0.0f )
				{
					float t = 1.0f - m_fActionBlendPassageTime / m_fActionBlendAbsoluteTime;
					if( t < 0.0f )
						m_fActionBlendAbsoluteTime = 0;				// Finish Blend action
					else
						matTemp.Lerp(m_pBlendFrameMatrix[x], t);	// linear Lerp bone matrix
				}
			}

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
						updateAbsolutePosition(pMF1Res->pModelMF1->m_pParticleEmitter[w].m_AbsoluteMatrix * matTemp * m_matModel);
				}
			}

		}

		m_pRenderDevice->GetVertexCacheManager()->UpdateTextureBufferObjectByID(m_BoneMatrixBuffer, pMF1Res->pModelMF1->m_iNumBones, m_TBOID);
	
		//Update Particle
		for( int i=0; i<pMF1Res->ParticleSystemIDArray.size(); i++ )
		{
			if( isParticleVisible(i) )
				m_pRenderDevice->GetParticleManager()->getParticleSystemByID(pMF1Res->ParticleSystemIDArray[i])->update(deltaTimeinSeconds);
		}
	}

	// Attachment
	for( int i=0; i<SGPATTDEF_MAXATTACHMENT; i++ )
	{
		if( m_pAttachedComponents[i] )
			m_pAttachedComponents[i]->update(deltaTimeinSeconds);
	}

	return true;
}

void CSkeletonMeshInstance::playAnim(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, bool bNewAnim)
{
	m_nStartFrameTime = StartFrameID * ISGPInstanceManager::DefaultSecondsPerFrame;
	m_nEndFrameTime = EndFrameID * ISGPInstanceManager::DefaultSecondsPerFrame;
	m_fAnimPlayedTime = 0;

	m_fPlaySpeed = fSpeed;
	m_bLoopPlaying = bLoop;

	if( bNewAnim )
		m_fLastTime = m_nStartFrameTime;

	// Attachment
	playAttachmentAnim(fSpeed, bLoop);
}

void CSkeletonMeshInstance::playAnimCrossFade(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, float fBlendAbsoluteTime)
{
	playAnim( fSpeed, StartFrameID, EndFrameID, bLoop, true );

	if( fBlendAbsoluteTime > 0 && m_BoneMatrixBuffer )
	{
		CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

		if( !m_pBlendFrameMatrix )
			m_pBlendFrameMatrix = new Matrix4x4 [pMF1Res->pModelMF1->m_iNumBones];

		for( uint32 i=0; i<pMF1Res->pModelMF1->m_iNumBones; ++i )
		{
			m_pBlendFrameMatrix[i]._11 = m_BoneMatrixBuffer[12 * i     ];  
            m_pBlendFrameMatrix[i]._21 = m_BoneMatrixBuffer[12 * i +  1];  
            m_pBlendFrameMatrix[i]._31 = m_BoneMatrixBuffer[12 * i +  2];  
            m_pBlendFrameMatrix[i]._41 = m_BoneMatrixBuffer[12 * i +  3];  
            m_pBlendFrameMatrix[i]._12 = m_BoneMatrixBuffer[12 * i +  4];  
            m_pBlendFrameMatrix[i]._22 = m_BoneMatrixBuffer[12 * i +  5];  
            m_pBlendFrameMatrix[i]._32 = m_BoneMatrixBuffer[12 * i +  6];  
            m_pBlendFrameMatrix[i]._42 = m_BoneMatrixBuffer[12 * i +  7];  
            m_pBlendFrameMatrix[i]._13 = m_BoneMatrixBuffer[12 * i +  8];  
            m_pBlendFrameMatrix[i]._23 = m_BoneMatrixBuffer[12 * i +  9];  
            m_pBlendFrameMatrix[i]._33 = m_BoneMatrixBuffer[12 * i + 10];  
            m_pBlendFrameMatrix[i]._43 = m_BoneMatrixBuffer[12 * i + 11];
			m_pBlendFrameMatrix[i]._14 = 0;
			m_pBlendFrameMatrix[i]._24 = 0;
			m_pBlendFrameMatrix[i]._34 = 0;
			m_pBlendFrameMatrix[i]._44 = 1;
		}
		m_fActionBlendAbsoluteTime = fBlendAbsoluteTime;
		m_fActionBlendPassageTime = 0;
	}
}

void CSkeletonMeshInstance::playUpperBodyAnim(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, bool bNewAnim)
{
	m_nUpperStartFrameTime = StartFrameID * ISGPInstanceManager::DefaultSecondsPerFrame;
	m_nUpperEndFrameTime = EndFrameID * ISGPInstanceManager::DefaultSecondsPerFrame;
	m_fUpperAnimPlayedTime = 0;

	m_fUpperPlaySpeed = fSpeed;
	m_bUpperLoopPlaying = bLoop;

	if( bNewAnim )
		m_fUpperLastTime = m_nUpperStartFrameTime;
}

void CSkeletonMeshInstance::playUpperBodyAnimCrossFade(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, float fBlendAbsoluteTime)
{
	playUpperBodyAnim( fSpeed, StartFrameID, EndFrameID, bLoop, true );
	
	if( fBlendAbsoluteTime > 0 && m_BoneMatrixBuffer )
	{
		CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

		if( !m_pUpperBodyBlendFrameMatrix )
			m_pUpperBodyBlendFrameMatrix = new Matrix4x4 [pMF1Res->pModelMF1->m_iNumBones];

		for( uint32 i=0; i<pMF1Res->pModelMF1->m_iNumBones; ++i )
		{
			m_pUpperBodyBlendFrameMatrix[i]._11 = m_BoneMatrixBuffer[12 * i     ];  
            m_pUpperBodyBlendFrameMatrix[i]._21 = m_BoneMatrixBuffer[12 * i +  1];  
            m_pUpperBodyBlendFrameMatrix[i]._31 = m_BoneMatrixBuffer[12 * i +  2];  
            m_pUpperBodyBlendFrameMatrix[i]._41 = m_BoneMatrixBuffer[12 * i +  3];  
            m_pUpperBodyBlendFrameMatrix[i]._12 = m_BoneMatrixBuffer[12 * i +  4];  
            m_pUpperBodyBlendFrameMatrix[i]._22 = m_BoneMatrixBuffer[12 * i +  5];  
            m_pUpperBodyBlendFrameMatrix[i]._32 = m_BoneMatrixBuffer[12 * i +  6];  
            m_pUpperBodyBlendFrameMatrix[i]._42 = m_BoneMatrixBuffer[12 * i +  7];  
            m_pUpperBodyBlendFrameMatrix[i]._13 = m_BoneMatrixBuffer[12 * i +  8];  
            m_pUpperBodyBlendFrameMatrix[i]._23 = m_BoneMatrixBuffer[12 * i +  9];  
            m_pUpperBodyBlendFrameMatrix[i]._33 = m_BoneMatrixBuffer[12 * i + 10];  
            m_pUpperBodyBlendFrameMatrix[i]._43 = m_BoneMatrixBuffer[12 * i + 11];
			m_pUpperBodyBlendFrameMatrix[i]._14 = 0;
			m_pUpperBodyBlendFrameMatrix[i]._24 = 0;
			m_pUpperBodyBlendFrameMatrix[i]._34 = 0;
			m_pUpperBodyBlendFrameMatrix[i]._44 = 1;
		}
		m_fUpperBodyActionBlendAbsoluteTime = fBlendAbsoluteTime;
		m_fUpperBodyActionBlendPassageTime = 0;
	}
}



void CSkeletonMeshInstance::getActionFrameIDFromName( const char* ActionName, uint32& startFrameID, uint32& endFrameID )
{
	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return;

	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	
	if( STRNICMP( ActionName, "all", 3 ) == 0 )
	{
		startFrameID = pMF1Res->pModelMF1->m_pActionLists[0].m_iActionStart;
		endFrameID = pMF1Res->pModelMF1->m_pActionLists[0].m_iActionEnd;
		
		for(uint32 i=1; i < pMF1Res->pModelMF1->m_Header.m_iNumActionList; ++i)
		{
			if( pMF1Res->pModelMF1->m_pActionLists[i].m_iActionStart < startFrameID )
				startFrameID = pMF1Res->pModelMF1->m_pActionLists[i].m_iActionStart;
			if( pMF1Res->pModelMF1->m_pActionLists[i].m_iActionEnd > endFrameID )
				endFrameID = pMF1Res->pModelMF1->m_pActionLists[i].m_iActionEnd;
		}

		return;
	}
	
	for(uint32 i=0; i < pMF1Res->pModelMF1->m_Header.m_iNumActionList; ++i)
	{
		if( STRNICMP( ActionName, pMF1Res->pModelMF1->m_pActionLists[i].m_cName, strlen(ActionName) ) == 0 )
		{
			startFrameID = pMF1Res->pModelMF1->m_pActionLists[i].m_iActionStart;
			endFrameID = pMF1Res->pModelMF1->m_pActionLists[i].m_iActionEnd;
			return;
		}
	}
}

int32 CSkeletonMeshInstance::getBoneIDByName( const char* BoneName )
{
	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return -1;
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

	for(uint32 i=0; i < pMF1Res->pModelMF1->m_iNumBones; ++i)
	{
		if( STRNICMP( BoneName, pMF1Res->pModelMF1->m_pBones[i].m_cName, strlen(BoneName) ) == 0 )
			return i;
	}
	return -1;
}

Vector3D CSkeletonMeshInstance::getBoneWorldPositionByID( int32 iBoneID )
{
	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return Vector3D(0,0,0);

	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);

	if( (iBoneID >= 0) && (iBoneID < (int32)pMF1Res->pModelMF1->m_iNumBones) )
	{
		Matrix4x4 matBoneLocal;
		matBoneLocal.InverseOf(pMF1Res->pModelMF1->m_pBones[iBoneID].m_matFrame0Inv);
		

		Matrix4x4 matAnimation;
		matAnimation.Identity();
		matAnimation._11 = m_BoneMatrixBuffer[12 * iBoneID    ];
		matAnimation._21 = m_BoneMatrixBuffer[12 * iBoneID +  1];
		matAnimation._31 = m_BoneMatrixBuffer[12 * iBoneID +  2];
		matAnimation._41 = m_BoneMatrixBuffer[12 * iBoneID +  3];
		matAnimation._12 = m_BoneMatrixBuffer[12 * iBoneID +  4];
		matAnimation._22 = m_BoneMatrixBuffer[12 * iBoneID +  5];
		matAnimation._32 = m_BoneMatrixBuffer[12 * iBoneID +  6];
		matAnimation._42 = m_BoneMatrixBuffer[12 * iBoneID +  7];
		matAnimation._13 = m_BoneMatrixBuffer[12 * iBoneID +  8];
		matAnimation._23 = m_BoneMatrixBuffer[12 * iBoneID +  9];
		matAnimation._33 = m_BoneMatrixBuffer[12 * iBoneID + 10];
		matAnimation._43 = m_BoneMatrixBuffer[12 * iBoneID + 11];

		matBoneLocal = matBoneLocal * matAnimation;

		return matBoneLocal.GetTranslation() * getModelMatrix();
	}

	return Vector3D(0,0,0);
}

uint32 CSkeletonMeshInstance::getBoneNum()
{
	if( m_MF1ModelResourceID == 0xFFFFFFFF )
		return 0;

	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	return pMF1Res->pModelMF1->m_iNumBones;
}






void CSkeletonMeshInstance::resetParticleSystemByID(uint32 partID, const SGPMF1ParticleTag& newSetting, bool* pGroupVisibleArray)
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

void CSkeletonMeshInstance::setConfigSetting(SGPMF1ConfigSetting* pConfigSetting)
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

OBBox CSkeletonMeshInstance::getStaticMeshOBBox()
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
		return OBBox(&pMF1Res->pModelMF1->m_MeshAABBox);
	return OBBox();
}

void CSkeletonMeshInstance::addAttachment(SGP_ATTACHMENT_DEFINE part, const String& MF1ModelFileName, uint32 ConfigIndex)
{
	// If having other ATTACHMENT, first destroy it
	detachAttachment(part);

	m_pAttachedComponents[part] = new CMeshComponent( m_pRenderDevice, this, part );
	m_pAttachedComponents[part]->changeModel(MF1ModelFileName, ConfigIndex);
	m_pAttachedComponents[part]->playAnim(m_fPlaySpeed, m_bLoopPlaying);
}

CMeshComponent* CSkeletonMeshInstance::getAttachment(SGP_ATTACHMENT_DEFINE part)
{
	return m_pAttachedComponents[part];
}

void CSkeletonMeshInstance::detachAttachment(SGP_ATTACHMENT_DEFINE part)
{
	if( m_pAttachedComponents[part] )
	{
		m_pAttachedComponents[part]->destroyModel();
		delete m_pAttachedComponents[part];
		m_pAttachedComponents[part] = NULL;
	}
}

void CSkeletonMeshInstance::playAttachmentAnim(float fSpeed, bool bLoop)
{
	for( int i=0; i<SGPATTDEF_MAXATTACHMENT; i++ )
	{
		if( m_pAttachedComponents[i] )
		{
			m_pAttachedComponents[i]->playAnim(fSpeed, bLoop);
		}
	}
}












Vector3D CSkeletonMeshInstance::calculateCurrentTranslation(const SGPMF1Bone *pBone, float fTime)
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

Quaternion CSkeletonMeshInstance::calculateCurrentRotation(const SGPMF1Bone *pBone, float fTime)
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

float CSkeletonMeshInstance::calculateCurrentScale(const SGPMF1Bone *pBone, float fTime)
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

bool CSkeletonMeshInstance::calculateCurrentVisible(const SGPMF1Bone *pBone, float fTime)
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

void CSkeletonMeshInstance::buildOBB()
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

void CSkeletonMeshInstance::updateOBB(const Matrix4x4& Matrix)
{
	CMF1FileResource* pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(m_MF1ModelResourceID);
	if( pMF1Res && pMF1Res->pModelMF1 )
	{
		m_InstanceOBBox.DeTransform( OBBox(&pMF1Res->pModelMF1->m_MeshAABBox), Matrix );
	}
}

bool CSkeletonMeshInstance::isMeshVisible(int meshIndex)
{
	if( m_pCurrentConfig )
	{
		for( uint32 i=0; i<m_pCurrentConfig->MeshConfigNum; i++ )
			if( m_pCurrentConfig->pMeshConfigList[i].MeshID == (uint32)meshIndex )
				return m_pCurrentConfig->pMeshConfigList[i].bVisible;
	}
	return true;
}

bool CSkeletonMeshInstance::isParticleVisible(int partIndex)
{
	if( m_pCurrentConfig )
	{
		for( uint32 i=0; i<m_pCurrentConfig->ParticleConfigNum; i++ )
			if( m_pCurrentConfig->pParticleConfigList[i].ParticleID == (uint32)partIndex )
				return m_pCurrentConfig->pParticleConfigList[i].bVisible;
	}
	return true;
}

bool CSkeletonMeshInstance::isBoneVisible(uint16 nBoneID)
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

Matrix4x4 CSkeletonMeshInstance::getBBRDMeshMatrix(int meshIndex)
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
	MeshTrans._11 = MeshTrans._22 = MeshTrans._33 = getScale();
	MeshTrans.SetTranslation(MeshCenter * MeshCenterTrans * m_matModel);

	Matrix4x4 BBRDMatrix;
	BBRDMatrix = BBRDInvTrans * BBRDRotation * MeshTrans;

	return BBRDMatrix;
}