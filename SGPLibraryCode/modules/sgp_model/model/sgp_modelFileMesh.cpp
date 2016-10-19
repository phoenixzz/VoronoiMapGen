
static Array<PtrOffset> s_CollectPtrs;
static uint32 s_CurFileOffset = 0;


CSGPModelMF1::CSGPModelMF1()
{
	m_pLOD0Meshes = m_pLOD1Meshes = m_pLOD2Meshes = NULL;
	m_pSkins = NULL;
	m_pBones = NULL;
	m_pBoneGroup =NULL;
	m_pBoneFileNames = NULL;

	m_pAttachTags = m_pEffectTags = NULL;
	m_pActionLists = NULL;

	m_pParticleEmitter = NULL;
	m_pRibbonEmitter = NULL;

	m_pConfigSetting = NULL;

	m_iNumBones = 0;
	m_iNumBoneGroup = 0;
}

CSGPModelMF1::~CSGPModelMF1()
{
	if(m_pLOD0Meshes)
	{
		delete [] m_pLOD0Meshes;
		m_pLOD0Meshes = NULL;
	}
	if(m_pLOD1Meshes)
	{
		delete [] m_pLOD1Meshes;
		m_pLOD1Meshes = NULL;
	}
	if(m_pLOD2Meshes)
	{
		delete [] m_pLOD2Meshes;
		m_pLOD2Meshes = NULL;
	}

	if(m_pSkins)
	{
		delete [] m_pSkins;
		m_pSkins = NULL;
	}

	if(m_pBones)
	{
		delete [] m_pBones;
		m_pBones = NULL;
	}
	if(m_pBoneGroup)
	{
		delete [] m_pBoneGroup;
		m_pBoneGroup = NULL;
	}
	if(m_pBoneFileNames)
	{
		delete [] m_pBoneFileNames;
		m_pBoneFileNames = NULL;
	}

	if(m_pAttachTags)
	{
		delete [] m_pAttachTags;
		m_pAttachTags = NULL;
	}
	if(m_pEffectTags)
	{
		delete [] m_pEffectTags;
		m_pEffectTags = NULL;
	}

	if( m_pParticleEmitter )
	{
		delete [] m_pParticleEmitter;
		m_pParticleEmitter = NULL;
	}
	if( m_pRibbonEmitter )
	{
		delete [] m_pRibbonEmitter;
		m_pRibbonEmitter = NULL;
	}
	if( m_pConfigSetting )
	{
		delete [] m_pConfigSetting;
		m_pConfigSetting = NULL;
	}

	if(m_pActionLists)
	{
		delete [] m_pActionLists;
		m_pActionLists = NULL;
	}


}


//-------------------------------------------------------------
//- Load
//- Loads an MF1 model from a file
//-------------------------------------------------------------
uint8* CSGPModelMF1::LoadMF1(CSGPModelMF1* &pOutModelMF1, const String& WorkingDir, const String& Filename)
{
	uint8 * ucpBuffer = 0;	
	uint32 iFileSize = 0;

	String AbsolutePath(Filename);
	// Identify by their absolute filenames if possible.
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = WorkingDir +	File::separatorString + String(Filename);
	}

	{
		ScopedPointer<FileInputStream> MF1FileStream( File(AbsolutePath).createInputStream() );
		if( MF1FileStream == nullptr )
		{
			Logger::getCurrentLogger()->writeToLog(String("Could not open MF1 File:") + Filename, ELL_ERROR);
			return NULL;
		}
		iFileSize = (uint32)MF1FileStream->getTotalLength();

		ucpBuffer = new uint8 [iFileSize];

		MF1FileStream->read(ucpBuffer, iFileSize);
	}

	//Make sure header is valid
	pOutModelMF1 = (CSGPModelMF1 *)ucpBuffer;

	if(pOutModelMF1->m_Header.m_iId != 0xCAFE2BEE || pOutModelMF1->m_Header.m_iVersion != 1)
	{
		Logger::getCurrentLogger()->writeToLog(Filename + String(" is not a valid MF1 File!"), ELL_ERROR);
		delete [] ucpBuffer;
		ucpBuffer = NULL;
		return NULL;
	}

	// Skin
	{
		if( pOutModelMF1->m_Header.m_iSkinOffset )
		{
			pOutModelMF1->m_pSkins = (SGPMF1Skin *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iSkinOffset);
			for( uint32 i=0; i<pOutModelMF1->m_Header.m_iNumSkins; i++ )
			{
				if( pOutModelMF1->m_pSkins[i].m_pMatKeyFrame )
					pOutModelMF1->m_pSkins[i].m_pMatKeyFrame = (SGPMF1MatKeyFrame *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pSkins[i].m_pMatKeyFrame);
			}
		}
		else
			pOutModelMF1->m_pSkins = NULL;
	}

	//Mesh
	{
		if( pOutModelMF1->m_Header.m_iLod0MeshOffset )
		{
			pOutModelMF1->m_pLOD0Meshes = (SGPMF1Mesh *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iLod0MeshOffset);
			for( uint32 i=0; i<pOutModelMF1->m_Header.m_iNumMeshes; i++ )
			{
				if( pOutModelMF1->m_pLOD0Meshes[i].m_pVertex )
					pOutModelMF1->m_pLOD0Meshes[i].m_pVertex = (SGPMF1Vertex *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pLOD0Meshes[i].m_pVertex);
				if( pOutModelMF1->m_pLOD0Meshes[i].m_pIndices )
					pOutModelMF1->m_pLOD0Meshes[i].m_pIndices = (uint16 *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pLOD0Meshes[i].m_pIndices);
				if( pOutModelMF1->m_pLOD0Meshes[i].m_pVertexBoneGroupID )
					pOutModelMF1->m_pLOD0Meshes[i].m_pVertexBoneGroupID = (uint16 *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pLOD0Meshes[i].m_pVertexBoneGroupID);
				if( pOutModelMF1->m_pLOD0Meshes[i].m_pTexCoords0 )
					pOutModelMF1->m_pLOD0Meshes[i].m_pTexCoords0 = (SGPMF1TexCoord *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pLOD0Meshes[i].m_pTexCoords0);
				if( pOutModelMF1->m_pLOD0Meshes[i].m_pTexCoords1 )
					pOutModelMF1->m_pLOD0Meshes[i].m_pTexCoords1 = (SGPMF1TexCoord *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pLOD0Meshes[i].m_pTexCoords1);
				if( pOutModelMF1->m_pLOD0Meshes[i].m_pVertexColor )
					pOutModelMF1->m_pLOD0Meshes[i].m_pVertexColor = (SGPMF1VertexColor *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pLOD0Meshes[i].m_pVertexColor);
			}
		}
		else
			pOutModelMF1->m_pLOD0Meshes = NULL;

		if( pOutModelMF1->m_Header.m_iLod1MeshOffset )
		{}
		else
			pOutModelMF1->m_pLOD1Meshes = NULL;

		if( pOutModelMF1->m_Header.m_iLod2MeshOffset )
		{}
		else
			pOutModelMF1->m_pLOD2Meshes = NULL;
	}

	// Bone File name
	{
		if( pOutModelMF1->m_Header.m_iBoneAnimFileOffset )
			pOutModelMF1->m_pBoneFileNames = (SGPMF1BoneFileName *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iBoneAnimFileOffset);
		else
			pOutModelMF1->m_pBoneFileNames = NULL;
	}

	// ActionList
	{
		if( pOutModelMF1->m_Header.m_iActionListOffset )
			pOutModelMF1->m_pActionLists = (SGPMF1ActionList *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iActionListOffset);
		else
			pOutModelMF1->m_pActionLists = NULL;
	}

	//Attachment
	{
		if( pOutModelMF1->m_Header.m_iAttachOffset )
			pOutModelMF1->m_pAttachTags = (SGPMF1AttachmentTag *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iAttachOffset);
		else
			pOutModelMF1->m_pAttachTags = NULL;

		if( pOutModelMF1->m_Header.m_iEttachOffset )
			pOutModelMF1->m_pEffectTags = (SGPMF1AttachmentTag *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iEttachOffset);
		else
			pOutModelMF1->m_pEffectTags = NULL;
	}

	// Particle
	{
		if( pOutModelMF1->m_Header.m_iParticleOffset )
		{
			pOutModelMF1->m_pParticleEmitter = (SGPMF1ParticleTag *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iParticleOffset);

			for( uint32 i=0; i < pOutModelMF1->m_Header.m_iNumParticles; ++i )
			{
				// Particle System
				ParticleSystemParam& systemParam = pOutModelMF1->m_pParticleEmitter[i].m_SystemParam;
				systemParam.m_pGroupParam = (ParticleGroupParam *)(ucpBuffer + (intptr_t)systemParam.m_pGroupParam);
				for( uint32 j=0; j < systemParam.m_groupCount; ++j )
				{
					// Particle Group
					ParticleGroupParam& groupParam = systemParam.m_pGroupParam[j];
					// Particle Model
					ParticleModelParam& modelParam = groupParam.m_ModelParam;

					modelParam.m_pRegularParam = (ParticleRegularParam *)(ucpBuffer + (intptr_t)modelParam.m_pRegularParam);
					modelParam.m_pInterpolatorParam = (ParticleInterpolatorParam *)(ucpBuffer + (intptr_t)modelParam.m_pInterpolatorParam);
					
					// interpolator
					for(uint32 k=0; k < modelParam.m_InterpolatorCount; ++k)
					{
						ParticleInterpolatorParam& interpolatorParam = modelParam.m_pInterpolatorParam[k];
						if(interpolatorParam.m_InterpolatorType == Interpolator_SelfDefine)
						{
							ParticleSelfDefInterpolatorData& selfDefData = interpolatorParam.m_SelfDefData;
							selfDefData.m_pEntry = (ParticleEntryParam *)(ucpBuffer + (intptr_t)selfDefData.m_pEntry);
						}
					}

					// Particle Emitter
					groupParam.m_pEmitterParam = (ParticleEmitterParam *)(ucpBuffer + (intptr_t)groupParam.m_pEmitterParam);
					// Particle Modifier
					groupParam.m_pModifierParam = (ParticleModifierParam *)(ucpBuffer + (intptr_t)groupParam.m_pModifierParam);
				}
			}
		}
	}
	//Ribbon
	{
	}

	//Config Setting
	{
		if( pOutModelMF1->m_Header.m_iConfigsOffset )
		{
			pOutModelMF1->m_pConfigSetting = (SGPMF1ConfigSetting *)(ucpBuffer + (intptr_t)pOutModelMF1->m_Header.m_iConfigsOffset);
			for( uint32 i=0; i<pOutModelMF1->m_Header.m_iNumConfigs; i++ )
			{
				if( pOutModelMF1->m_pConfigSetting[i].pMeshConfigList )
					pOutModelMF1->m_pConfigSetting[i].pMeshConfigList = (SGPMF1ConfigSetting::MeshConfig *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pConfigSetting[i].pMeshConfigList);
				if( pOutModelMF1->m_pConfigSetting[i].pReplaceTextureConfigList )
					pOutModelMF1->m_pConfigSetting[i].pReplaceTextureConfigList = (SGPMF1ConfigSetting::ReplaceTextureConfig *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pConfigSetting[i].pReplaceTextureConfigList);
				if( pOutModelMF1->m_pConfigSetting[i].pParticleConfigList )
					pOutModelMF1->m_pConfigSetting[i].pParticleConfigList = (SGPMF1ConfigSetting::ParticleConfig *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pConfigSetting[i].pParticleConfigList);
				if( pOutModelMF1->m_pConfigSetting[i].pRibbonConfigList )
					pOutModelMF1->m_pConfigSetting[i].pRibbonConfigList = (SGPMF1ConfigSetting::RibbonConfig *)(ucpBuffer + (intptr_t)pOutModelMF1->m_pConfigSetting[i].pRibbonConfigList);
			}
		}
		else
			pOutModelMF1->m_pConfigSetting = NULL;
	}

	pOutModelMF1->m_iNumBones = 0;
	pOutModelMF1->m_iNumBoneGroup = 0;

	pOutModelMF1->m_pBones = NULL;
	pOutModelMF1->m_pBoneGroup = NULL;


	return ucpBuffer;
}


//Load an MF1 bone animation file
uint8* CSGPModelMF1::LoadBone(CSGPModelMF1* &pOutModelMF1, const String& WorkingDir, const String& BoneFilename, uint16 BoneFileIndex)
{
	uint8 * ucpBuffer = 0;	
	uint32 iFileSize = 0;

	String AbsolutePath(BoneFilename);
	// Identify by their absolute filenames if possible.
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = WorkingDir +	File::separatorString + String(BoneFilename);
	}
	if( BoneFileIndex > 0 )
		AbsolutePath = AbsolutePath + String(BoneFileIndex);

	{
		ScopedPointer<FileInputStream> BF1FileStream( File(AbsolutePath).createInputStream() );
		if( BF1FileStream == nullptr )
		{
			Logger::getCurrentLogger()->writeToLog(String("Could not open BF1 File:") + BoneFilename, ELL_ERROR);
			return NULL;
		}
		iFileSize = (uint32)BF1FileStream->getTotalLength();

		ucpBuffer = new uint8 [iFileSize];

		BF1FileStream->read(ucpBuffer, iFileSize);
	}

	SGPMF1BoneHeader *pBoneHeader = (SGPMF1BoneHeader *)ucpBuffer;
	if(pBoneHeader->m_iId != 0xCAFEBBEE || pBoneHeader->m_iVersion != 1)
	{
		Logger::getCurrentLogger()->writeToLog(BoneFilename + String(" is not a valid BF1 File!"), ELL_ERROR);
		delete [] ucpBuffer;
		ucpBuffer = NULL;
		return NULL;
	}

	if( BoneFileIndex == 0 )
	{
		pOutModelMF1->m_iNumBones = pBoneHeader->m_iNumBones;
		pOutModelMF1->m_iNumBoneGroup = pBoneHeader->m_iNumBoneGroup;

		pOutModelMF1->m_pBones = (SGPMF1Bone *)(ucpBuffer + (intptr_t)pBoneHeader->m_iBonesOffset);

		for(uint32 i=0; i<pOutModelMF1->m_iNumBones; i++)
		{
			pOutModelMF1->m_pBones[i].m_ChildIds = (uint16*)(ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_ChildIds);

			pOutModelMF1->m_pBones[i].m_TransKeyFrames = (KeyFrameBlock*)(ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_TransKeyFrames);
			pOutModelMF1->m_pBones[i].m_RotsKeyFrames = (KeyFrameBlock*)(ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_RotsKeyFrames);
			pOutModelMF1->m_pBones[i].m_ScaleKeyFrames = (ScaleKeyFrameBlock*)(ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_ScaleKeyFrames);
			pOutModelMF1->m_pBones[i].m_VisibleKeyFrames = (VisibleKeyFrameBlock*)(ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_VisibleKeyFrames);

			pOutModelMF1->m_pBones[i].m_TransKeyFrames->m_KeyFrames = (SGPMF1KeyFrame*) (ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_TransKeyFrames->m_KeyFrames);
			pOutModelMF1->m_pBones[i].m_RotsKeyFrames->m_KeyFrames = (SGPMF1KeyFrame*) (ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_RotsKeyFrames->m_KeyFrames);
			pOutModelMF1->m_pBones[i].m_ScaleKeyFrames->m_KeyFrames = (SGPMF1ScaleKeyFrame*) (ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_ScaleKeyFrames->m_KeyFrames);
			pOutModelMF1->m_pBones[i].m_VisibleKeyFrames->m_KeyFrames = (SGPMF1VisibleKeyFrame*) (ucpBuffer + (intptr_t)pOutModelMF1->m_pBones[i].m_VisibleKeyFrames->m_KeyFrames);



		}
		pOutModelMF1->m_pBoneGroup = (SGPMF1BoneGroup *)(ucpBuffer + (intptr_t)pBoneHeader->m_iBoneGroupOffset);
	}
	else if( BoneFileIndex > 0 )
	{
		// Some check
		jassert(pOutModelMF1->m_iNumBones == pBoneHeader->m_iNumBones);
		jassert(pOutModelMF1->m_iNumBoneGroup == pBoneHeader->m_iNumBoneGroup);


		// Relocate Pointer
		SGPMF1Bone* pBones = (SGPMF1Bone *)(ucpBuffer + (intptr_t)pBoneHeader->m_iBonesOffset);
		for(uint32 i=0; i<pOutModelMF1->m_iNumBones; i++)
		{
			KeyFrameBlock* pTransBlock = (KeyFrameBlock*)(ucpBuffer + (intptr_t)pBones[i].m_TransKeyFrames);
			KeyFrameBlock* pRotsBlock = (KeyFrameBlock*)(ucpBuffer + (intptr_t)pBones[i].m_RotsKeyFrames);
			ScaleKeyFrameBlock* pScalesBlock = (ScaleKeyFrameBlock*)(ucpBuffer + (intptr_t)pBones[i].m_ScaleKeyFrames);
			VisibleKeyFrameBlock* pVisibleBlock = (VisibleKeyFrameBlock*)(ucpBuffer + (intptr_t)pBones[i].m_VisibleKeyFrames);

			pTransBlock->m_KeyFrames = (SGPMF1KeyFrame*) (ucpBuffer + (intptr_t)pTransBlock->m_KeyFrames);
			pRotsBlock->m_KeyFrames = (SGPMF1KeyFrame*) (ucpBuffer + (intptr_t)pRotsBlock->m_KeyFrames);
			pScalesBlock->m_KeyFrames = (SGPMF1ScaleKeyFrame*) (ucpBuffer + (intptr_t)pScalesBlock->m_KeyFrames);
			pVisibleBlock->m_KeyFrames = (SGPMF1VisibleKeyFrame*) (ucpBuffer + (intptr_t)pVisibleBlock->m_KeyFrames);						

			pTransBlock->m_BoneFileID = BoneFileIndex;
			pRotsBlock->m_BoneFileID = BoneFileIndex;
			pScalesBlock->m_BoneFileID = BoneFileIndex;
			pVisibleBlock->m_BoneFileID = BoneFileIndex;

			// Find next free Block
			KeyFrameBlock** ppNextTransBlock = &pOutModelMF1->m_pBones[i].m_TransKeyFrames->m_nextBlock;
			KeyFrameBlock** ppNextRotsBlock = &pOutModelMF1->m_pBones[i].m_RotsKeyFrames->m_nextBlock;
			ScaleKeyFrameBlock** ppNextScaleBlock = &pOutModelMF1->m_pBones[i].m_ScaleKeyFrames->m_nextBlock;
			VisibleKeyFrameBlock** ppNextVisibleBlock = &pOutModelMF1->m_pBones[i].m_VisibleKeyFrames->m_nextBlock;
			while(*ppNextTransBlock != NULL)
			{
				ppNextTransBlock = &((*ppNextTransBlock)->m_nextBlock);
				ppNextRotsBlock = &((*ppNextRotsBlock)->m_nextBlock);
				ppNextScaleBlock = &((*ppNextScaleBlock)->m_nextBlock);
				ppNextVisibleBlock = &((*ppNextVisibleBlock)->m_nextBlock);
			}
			
			*ppNextTransBlock = pTransBlock;
			*ppNextRotsBlock = pRotsBlock;
			*ppNextScaleBlock = pScalesBlock;
			*ppNextVisibleBlock = pVisibleBlock;
		}
	}

	return ucpBuffer;
}



//-------------------------------------------------------------
//- Save an MF1 model to a file
//- WorkingDir is current working path
//- Filename is relative path File name of Model File
//-------------------------------------------------------------
bool CSGPModelMF1::SaveMF1(const String& WorkingDir, const String& Filename)
{
	bool SaveResult = true;
	s_CurFileOffset = 0;
	s_CollectPtrs.clear();

	
	CollectPointers( &m_Header, sizeof(SGPMF1Header) );
	CollectPointers( &m_MeshAABBox, sizeof(AABBox) );

	CollectPointers( &m_pSkins, sizeof(SGPMF1Skin *) );

	CollectPointers( &m_pLOD0Meshes, sizeof(SGPMF1Mesh *) );
	CollectPointers( &m_pLOD1Meshes, sizeof(SGPMF1Mesh *) );
	CollectPointers( &m_pLOD2Meshes, sizeof(SGPMF1Mesh *) );

	CollectPointers( &m_pBoneFileNames, sizeof(SGPMF1BoneFileName *) );
	CollectPointers( &m_pActionLists, sizeof(SGPMF1ActionList *) );

	CollectPointers( &m_pAttachTags, sizeof(SGPMF1AttachmentTag *) );
	CollectPointers( &m_pEffectTags, sizeof(SGPMF1AttachmentTag *) );

	CollectPointers( &m_pBones, sizeof(SGPMF1Bone *) );
	CollectPointers( &m_pBoneGroup, sizeof(SGPMF1BoneGroup *) );

	CollectPointers( &m_pParticleEmitter, sizeof(SGPMF1ParticleTag *) );// Particle
	CollectPointers( &m_pRibbonEmitter, sizeof(void *) );				// Ribbon

	CollectPointers( &m_pConfigSetting, sizeof(SGPMF1ConfigSetting *) );// Config setting

	CollectPointers( &m_iNumBones, sizeof(uint32) );
	CollectPointers( &m_iNumBoneGroup, sizeof(uint32) );


	// Skin
	{
		m_Header.m_iSkinOffset = CollectPointers( m_pSkins, sizeof(SGPMF1Skin) * m_Header.m_iNumSkins );
		for( uint32 i=0; i<m_Header.m_iNumSkins; i++ )
		{
			if( m_pSkins[i].m_iNumMatKeyFrame > 0 )
				m_pSkins[i].m_pMatKeyFrame = (SGPMF1MatKeyFrame *)CollectPointers( m_pSkins[i].m_pMatKeyFrame, sizeof(SGPMF1MatKeyFrame) * m_pSkins[i].m_iNumMatKeyFrame );
		}
	}

	// LOD Mesh
	{
		if( m_Header.m_iNumLods >= 1 )
		{
			for( uint32 i = 0; i < m_Header.m_iNumMeshes; i++ )
			{
				m_pLOD0Meshes[i].m_pVertex = (SGPMF1Vertex *)CollectPointers(m_pLOD0Meshes[i].m_pVertex, sizeof(SGPMF1Vertex) * m_pLOD0Meshes[i].m_iNumVerts );
				m_pLOD0Meshes[i].m_pIndices = (uint16 *)CollectPointers(m_pLOD0Meshes[i].m_pIndices, sizeof(uint16) * m_pLOD0Meshes[i].m_iNumIndices);
				m_pLOD0Meshes[i].m_pVertexBoneGroupID = (uint16 *)CollectPointers(m_pLOD0Meshes[i].m_pVertexBoneGroupID, sizeof(uint16) * m_pLOD0Meshes[i].m_iNumVerts);
				if( m_pLOD0Meshes[i].m_iNumUV0 > 0 )
					m_pLOD0Meshes[i].m_pTexCoords0 = (SGPMF1TexCoord *)CollectPointers(m_pLOD0Meshes[i].m_pTexCoords0, sizeof(SGPMF1TexCoord) * m_pLOD0Meshes[i].m_iNumUV0);
				if( m_pLOD0Meshes[i].m_iNumUV1 > 0 )
					m_pLOD0Meshes[i].m_pTexCoords1 = (SGPMF1TexCoord *)CollectPointers(m_pLOD0Meshes[i].m_pTexCoords1, sizeof(SGPMF1TexCoord) * m_pLOD0Meshes[i].m_iNumUV1);
				if( m_pLOD0Meshes[i].m_iNumVertexColor > 0 )
					m_pLOD0Meshes[i].m_pVertexColor = (SGPMF1VertexColor *)CollectPointers(m_pLOD0Meshes[i].m_pVertexColor, sizeof(SGPMF1VertexColor) * m_pLOD0Meshes[i].m_iNumVertexColor);
			}
			m_Header.m_iLod0MeshOffset = CollectPointers( m_pLOD0Meshes, sizeof(SGPMF1Mesh) * m_Header.m_iNumMeshes );
		}
		if( m_Header.m_iNumLods >= 2 )
		{
		}
		if( m_Header.m_iNumLods >= 3 )
		{
		}
		if( m_Header.m_iNumLods >= 4 )
			jassertfalse;
	}

	// Bone File name
	if( m_Header.m_iNumBoneAnimFile > 0 )
	{
		m_Header.m_iBoneAnimFileOffset = CollectPointers(m_pBoneFileNames, sizeof(SGPMF1BoneFileName) * m_Header.m_iNumBoneAnimFile);
	}

	// ActionList
	if( m_Header.m_iNumActionList > 0 )
	{
		m_Header.m_iActionListOffset = CollectPointers(m_pActionLists, sizeof(SGPMF1ActionList) * m_Header.m_iNumActionList);
	}

	//ATTACHMENT
	if( m_Header.m_iNumAttc > 0 )
		m_Header.m_iAttachOffset = CollectPointers(m_pAttachTags, sizeof(SGPMF1AttachmentTag) * m_Header.m_iNumAttc);
	if( m_Header.m_iNumEttc > 0 )
		m_Header.m_iEttachOffset = CollectPointers(m_pEffectTags, sizeof(SGPMF1AttachmentTag) * m_Header.m_iNumEttc);

	// Particle
	if( m_Header.m_iNumParticles > 0 )
	{
		m_Header.m_iParticleOffset = CollectPointers(m_pParticleEmitter, sizeof(SGPMF1ParticleTag) * m_Header.m_iNumParticles);

		for( uint32 i=0; i<m_Header.m_iNumParticles; ++i )
		{
			// Particle System
			ParticleSystemParam& systemParam = m_pParticleEmitter[i].m_SystemParam;
			ParticleGroupParam* pGroupParam = systemParam.m_pGroupParam;
			if(systemParam.m_groupCount > 0)
				systemParam.m_pGroupParam = (ParticleGroupParam*)CollectPointers(systemParam.m_pGroupParam, sizeof(ParticleGroupParam) * systemParam.m_groupCount);

			for( uint32 j=0; j<systemParam.m_groupCount; ++j )
			{
				// Particle Group
				ParticleGroupParam& groupParam = pGroupParam[j];

				// Particle Model
				ParticleModelParam& modelParam = groupParam.m_ModelParam;

				// Model Normal Param
				if(modelParam.m_ParamCount > 0)
					modelParam.m_pRegularParam = (ParticleRegularParam*)CollectPointers(modelParam.m_pRegularParam, sizeof(ParticleRegularParam) * modelParam.m_ParamCount);
				
				// Model Interpolator Param
				if(modelParam.m_InterpolatorCount > 0)
				{
					ParticleInterpolatorParam* pInterpolatorParam = modelParam.m_pInterpolatorParam;
					modelParam.m_pInterpolatorParam = (ParticleInterpolatorParam*)CollectPointers(modelParam.m_pInterpolatorParam, sizeof(ParticleInterpolatorParam) * modelParam.m_InterpolatorCount);
					for(uint32 k=0; k<modelParam.m_InterpolatorCount; ++k)
					{
						ParticleInterpolatorParam& interpolatorParam = pInterpolatorParam[k];
						if(interpolatorParam.m_InterpolatorType == Interpolator_SelfDefine)
						{
							ParticleSelfDefInterpolatorData& selfDefData = interpolatorParam.m_SelfDefData;
							if(selfDefData.m_count > 0)
								selfDefData.m_pEntry = (ParticleEntryParam*)CollectPointers(selfDefData.m_pEntry, sizeof(ParticleEntryParam) * selfDefData.m_count);
						}
					}
				}

				// Particle Emitter
				if( groupParam.m_nEmitterCount > 0 )
					groupParam.m_pEmitterParam = (ParticleEmitterParam*)CollectPointers(groupParam.m_pEmitterParam, sizeof(ParticleEmitterParam) * groupParam.m_nEmitterCount);

				// Particle Modifier
				if( groupParam.m_nModifierCount > 0 )
					groupParam.m_pModifierParam = (ParticleModifierParam*)CollectPointers(groupParam.m_pModifierParam, sizeof(ParticleModifierParam) * groupParam.m_nModifierCount);
			}
		}
	}
	// RIBBON
	{
	}

	// Config Setting
	if( m_Header.m_iNumConfigs > 0 )
	{
		for( uint32 i = 0; i < m_Header.m_iNumConfigs; i++ )
		{
			if( m_pConfigSetting[i].MeshConfigNum > 0 )
				m_pConfigSetting[i].pMeshConfigList = (SGPMF1ConfigSetting::MeshConfig *)CollectPointers( m_pConfigSetting[i].pMeshConfigList, sizeof(SGPMF1ConfigSetting::MeshConfig) * m_pConfigSetting[i].MeshConfigNum );
			if( m_pConfigSetting[i].ReplaceTextureConfigNum > 0 )
				m_pConfigSetting[i].pReplaceTextureConfigList = (SGPMF1ConfigSetting::ReplaceTextureConfig *)CollectPointers( m_pConfigSetting[i].pReplaceTextureConfigList, sizeof(SGPMF1ConfigSetting::ReplaceTextureConfig) * m_pConfigSetting[i].ReplaceTextureConfigNum );
			if( m_pConfigSetting[i].ParticleConfigNum > 0 )
				m_pConfigSetting[i].pParticleConfigList = (SGPMF1ConfigSetting::ParticleConfig *)CollectPointers( m_pConfigSetting[i].pParticleConfigList, sizeof(SGPMF1ConfigSetting::ParticleConfig) * m_pConfigSetting[i].ParticleConfigNum );
			if( m_pConfigSetting[i].RibbonConfigNum > 0 )
				m_pConfigSetting[i].pRibbonConfigList = (SGPMF1ConfigSetting::RibbonConfig *)CollectPointers( m_pConfigSetting[i].pRibbonConfigList, sizeof(SGPMF1ConfigSetting::RibbonConfig) * m_pConfigSetting[i].RibbonConfigNum );
		}
		m_Header.m_iConfigsOffset = CollectPointers(m_pConfigSetting, sizeof(SGPMF1ConfigSetting) * m_Header.m_iNumConfigs);
	}


	//-------------------------------------------------------------
	// Identify it by the absolute filenames if possible.
	String AbsolutePath = Filename;	
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = WorkingDir +	File::separatorString + Filename;
	}

	ScopedPointer<FileOutputStream> fileStream( File(AbsolutePath).createOutputStream() );
	if (fileStream != nullptr && fileStream->openedOk() )
    {
		fileStream->setPosition(0);
		fileStream->truncate();

		for( int i=0; i<s_CollectPtrs.size(); i++ )
		{
			fileStream->write( (const uint8*)s_CollectPtrs[i].pDataPointer, s_CollectPtrs[i].DataSize );
		}

		fileStream->flush();
	}
	else
		SaveResult = false;
	//-------------------------------------------------------------

	// Reset Pointers - For release memory correctly
	RelocateFirstPointers();							//SGPMF1Header
	RelocateFirstPointers();							//AABBox

	(SGPMF1Skin *)RelocateFirstPointers();				//SGPMF1Skin*

	(SGPMF1Mesh *)RelocateFirstPointers();
	(SGPMF1Mesh *)RelocateFirstPointers();
	(SGPMF1Mesh *)RelocateFirstPointers();

	(SGPMF1BoneFileName *)RelocateFirstPointers();
	(SGPMF1ActionList *)RelocateFirstPointers();
	(SGPMF1AttachmentTag *)RelocateFirstPointers();
	(SGPMF1AttachmentTag *)RelocateFirstPointers();

	(SGPMF1Bone *)RelocateFirstPointers();
	(SGPMF1BoneGroup *)RelocateFirstPointers();
	(SGPMF1ParticleTag *)RelocateFirstPointers();		//Particle
	(void *)RelocateFirstPointers();					//Ribbon
	(SGPMF1ConfigSetting *)RelocateFirstPointers();		//ConfigSetting

	RelocateFirstPointers();
	RelocateFirstPointers();



	(SGPMF1Skin *)RelocateFirstPointers();
	for( uint32 i=0; i<m_Header.m_iNumSkins; i++ )
	{
		if( m_pSkins[i].m_iNumMatKeyFrame > 0 )
			m_pSkins[i].m_pMatKeyFrame = (SGPMF1MatKeyFrame *)RelocateFirstPointers();
	}

	if( m_Header.m_iNumLods >= 1 )
	{
		for( uint32 i = 0; i < m_Header.m_iNumMeshes; i++ )
		{
			m_pLOD0Meshes[i].m_pVertex = (SGPMF1Vertex *)RelocateFirstPointers();
			m_pLOD0Meshes[i].m_pIndices = (uint16 *)RelocateFirstPointers();
			m_pLOD0Meshes[i].m_pVertexBoneGroupID = (uint16 *)RelocateFirstPointers();
			if( m_pLOD0Meshes[i].m_iNumUV0 > 0 )
				m_pLOD0Meshes[i].m_pTexCoords0 = (SGPMF1TexCoord *)RelocateFirstPointers();
			if( m_pLOD0Meshes[i].m_iNumUV1 > 0 )
				m_pLOD0Meshes[i].m_pTexCoords1 = (SGPMF1TexCoord *)RelocateFirstPointers();
			if( m_pLOD0Meshes[i].m_iNumVertexColor > 0 )
				m_pLOD0Meshes[i].m_pVertexColor = (SGPMF1VertexColor *)RelocateFirstPointers();
		}
		(SGPMF1Mesh*)RelocateFirstPointers();
	}
	if( m_Header.m_iNumLods >= 2 )
	{
	}
	if( m_Header.m_iNumLods >= 3 )
	{
	}


	if( m_Header.m_iNumBoneAnimFile > 0 )
		(SGPMF1BoneFileName*)RelocateFirstPointers();
	if( m_Header.m_iNumActionList > 0 )
		(SGPMF1ActionList*)RelocateFirstPointers();

	if( m_Header.m_iNumAttc > 0 )
		(SGPMF1AttachmentTag*)RelocateFirstPointers();
	if( m_Header.m_iNumEttc > 0 )
		(SGPMF1AttachmentTag*)RelocateFirstPointers();

	// Particle
	if( m_Header.m_iNumParticles > 0 )
	{
		(SGPMF1ParticleTag*)RelocateFirstPointers();

		for( uint32 i=0; i < m_Header.m_iNumParticles; ++i )
		{
			// Particle System
			ParticleSystemParam& systemParam = m_pParticleEmitter[i].m_SystemParam;
			if( systemParam.m_groupCount > 0 )
				systemParam.m_pGroupParam = (ParticleGroupParam *)RelocateFirstPointers();
			for( uint32 j=0; j<systemParam.m_groupCount; ++j )
			{
				// Particle Group
				ParticleGroupParam& groupParam = systemParam.m_pGroupParam[j];

				// Particle Model
				ParticleModelParam& modelParam = groupParam.m_ModelParam;

				// Model Normal Param
				if( modelParam.m_ParamCount > 0 )
					modelParam.m_pRegularParam = (ParticleRegularParam *)RelocateFirstPointers();
				
				// Model Interpolator Param
				if( modelParam.m_InterpolatorCount > 0 )
				{
					modelParam.m_pInterpolatorParam = (ParticleInterpolatorParam *)RelocateFirstPointers();
					for(uint32 k=0; k<modelParam.m_InterpolatorCount; ++k)
					{
						ParticleInterpolatorParam& interpolatorParam = modelParam.m_pInterpolatorParam[k];
						if(interpolatorParam.m_InterpolatorType == Interpolator_SelfDefine)
						{
							ParticleSelfDefInterpolatorData& selfDefData = interpolatorParam.m_SelfDefData;
							if( selfDefData.m_count > 0 )
								selfDefData.m_pEntry = (ParticleEntryParam *)RelocateFirstPointers();

						}
					}
				}

				// Particle Emitter
				if( groupParam.m_nEmitterCount > 0 )
					groupParam.m_pEmitterParam = (ParticleEmitterParam *)RelocateFirstPointers();

				// Particle Modifier
				if( groupParam.m_nModifierCount > 0 )
					groupParam.m_pModifierParam = (ParticleModifierParam *)RelocateFirstPointers();
			}
		}

	}
	// RIBBON
	{
	}
	// Config Setting
	if( m_Header.m_iNumConfigs > 0 )
	{
		for( uint32 i = 0; i < m_Header.m_iNumConfigs; i++ )
		{
			if( m_pConfigSetting[i].MeshConfigNum > 0 )
				m_pConfigSetting[i].pMeshConfigList = (SGPMF1ConfigSetting::MeshConfig *)RelocateFirstPointers();
			if( m_pConfigSetting[i].ReplaceTextureConfigNum > 0 )
				m_pConfigSetting[i].pReplaceTextureConfigList = (SGPMF1ConfigSetting::ReplaceTextureConfig *)RelocateFirstPointers();
			if( m_pConfigSetting[i].ParticleConfigNum > 0 )
				m_pConfigSetting[i].pParticleConfigList = (SGPMF1ConfigSetting::ParticleConfig *)RelocateFirstPointers();
			if( m_pConfigSetting[i].RibbonConfigNum > 0 )
				m_pConfigSetting[i].pRibbonConfigList = (SGPMF1ConfigSetting::RibbonConfig *)RelocateFirstPointers();
		}
		(SGPMF1ConfigSetting*)RelocateFirstPointers();
	}

	return SaveResult;
}

//-------------------------------------------------------------
//- SaveBone
//- Save an MF1 bone animation file
//- WorkingDir is current Working dir
//- BoneFilename is relative path File name of BoneAnim 
//- BoneFileIndex is the index of split BoneAnim files
// For example, we would save XX.mf1, XX.bf1, XX.bf11, XX.bf12, XX.bf13 ... files
//-------------------------------------------------------------
bool CSGPModelMF1::SaveBone(const String& WorkingDir, const String& BoneFilename, uint16 BoneFileIndex)
{
	bool SaveResult = true;
	s_CurFileOffset = 0;
	s_CollectPtrs.clear();

	Array<uint16> BackupTransFrameNum;
	Array<uint16> BackupRotsFrameNum;
	Array<uint16> BackupScaleFrameNum;
	Array<uint16> BackupVisibleFrameNum;

	uint32 KFStart = 0xFFFFFFFF;
	uint32 KFEnd = 0;

	// get start frame ID and end frame ID from Action list in this bone file
	for(uint32 i=0; i<m_Header.m_iNumActionList; i++)
	{
		if( m_pActionLists[i].m_iBoneFileIndex == BoneFileIndex )
		{
			if( m_pActionLists[i].m_iActionStart < KFStart )
				KFStart = m_pActionLists[i].m_iActionStart;
			if( m_pActionLists[i].m_iActionEnd > KFEnd )
				KFEnd = m_pActionLists[i].m_iActionEnd;
		}
	}


	SGPMF1BoneHeader BF1Header;
	BF1Header.m_iHeaderSize = sizeof(SGPMF1BoneHeader);
	if( BoneFileIndex > 0 )
		strcpy(BF1Header.m_cFilename, String(BoneFilename + String(BoneFileIndex)).toUTF8().getAddress());
	else
		strcpy(BF1Header.m_cFilename, BoneFilename.toUTF8().getAddress());

	BF1Header.m_iNumBones = m_iNumBones;
	BF1Header.m_iNumBoneGroup = m_iNumBoneGroup;

	CollectPointers(&BF1Header, sizeof(SGPMF1BoneHeader));

	BF1Header.m_iBonesOffset = CollectPointers(m_pBones, sizeof(SGPMF1Bone) * m_iNumBones);
	for(uint32 i=0; i<m_iNumBones; i++)
	{
		if( m_pBones[i].m_iNumChildId > 0 )
			m_pBones[i].m_ChildIds = (uint16*)CollectPointers( m_pBones[i].m_ChildIds, sizeof(uint16) * m_pBones[i].m_iNumChildId );

		BackupTransFrameNum.add(m_pBones[i].m_TransKeyFrames->m_iNumber);
		BackupRotsFrameNum.add(m_pBones[i].m_RotsKeyFrames->m_iNumber);
		BackupScaleFrameNum.add(m_pBones[i].m_ScaleKeyFrames->m_iNumber);
		BackupVisibleFrameNum.add(m_pBones[i].m_VisibleKeyFrames->m_iNumber);
		
		uint32 TransKFStartOffset = 0;
		uint16 TransKFNum = 0;
		uint32 RotsKFStartOffset = 0;
		uint16 RotsKFNum = 0;
		uint32 ScaleKFStartOffset = 0;
		uint16 ScaleKFNum = 0;
		uint32 VisibleKFStartOffset = 0;
		uint16 VisibleKFNum = 0;

		for(int j=0; j<m_pBones[i].m_TransKeyFrames->m_iNumber; j++)
		{
			if( m_pBones[i].m_TransKeyFrames->m_KeyFrames[j].m_iFrameID >= KFStart )
			{
				if( m_pBones[i].m_TransKeyFrames->m_KeyFrames[j].m_iFrameID <= KFEnd )
					TransKFNum++;					
			}
			else
				TransKFStartOffset++;				
		}
		for(int j=0; j<m_pBones[i].m_RotsKeyFrames->m_iNumber; j++)
		{
			if( m_pBones[i].m_RotsKeyFrames->m_KeyFrames[j].m_iFrameID >= KFStart )
			{
				if( m_pBones[i].m_RotsKeyFrames->m_KeyFrames[j].m_iFrameID <= KFEnd )
					RotsKFNum++;
			}		
			else
				RotsKFStartOffset++;				
		}
		for(int j=0; j<m_pBones[i].m_ScaleKeyFrames->m_iNumber; j++)
		{
			if( m_pBones[i].m_ScaleKeyFrames->m_KeyFrames[j].m_iFrameID >= KFStart )
			{
				if( m_pBones[i].m_ScaleKeyFrames->m_KeyFrames[j].m_iFrameID <= KFEnd )
					ScaleKFNum++;					
			}
			else
				ScaleKFStartOffset++;				
		}
		for(int j=0; j<m_pBones[i].m_VisibleKeyFrames->m_iNumber; j++)
		{
			if( m_pBones[i].m_VisibleKeyFrames->m_KeyFrames[j].m_iFrameID >= KFStart )
			{
				if( m_pBones[i].m_VisibleKeyFrames->m_KeyFrames[j].m_iFrameID <= KFEnd )
					VisibleKFNum++;
			}
			else
				VisibleKFStartOffset++;				
		}

		if( m_Header.m_iNumActionList == 0 )
		{
			TransKFNum = m_pBones[i].m_TransKeyFrames->m_iNumber;
			TransKFStartOffset = 0;
			RotsKFNum = m_pBones[i].m_RotsKeyFrames->m_iNumber;
			RotsKFStartOffset = 0;
			ScaleKFNum = m_pBones[i].m_ScaleKeyFrames->m_iNumber;
			ScaleKFStartOffset = 0;
			VisibleKFNum = m_pBones[i].m_VisibleKeyFrames->m_iNumber;
			VisibleKFStartOffset = 0;
		}

		KeyFrameBlock* pTransKeyFrames = m_pBones[i].m_TransKeyFrames;
		KeyFrameBlock* pRotsKeyFrames = m_pBones[i].m_RotsKeyFrames;
		ScaleKeyFrameBlock*	pScaleKeyFrames = m_pBones[i].m_ScaleKeyFrames;
		VisibleKeyFrameBlock* pVisibleKeyFrames = m_pBones[i].m_VisibleKeyFrames;

		pTransKeyFrames->m_BoneFileID = BoneFileIndex;
		pRotsKeyFrames->m_BoneFileID = BoneFileIndex;
		pScaleKeyFrames->m_BoneFileID = BoneFileIndex;
		pVisibleKeyFrames->m_BoneFileID = BoneFileIndex;

		pTransKeyFrames->m_iNumber = TransKFNum;
		pRotsKeyFrames->m_iNumber = RotsKFNum;
		pScaleKeyFrames->m_iNumber = ScaleKFNum;
		pVisibleKeyFrames->m_iNumber = VisibleKFNum;

		m_pBones[i].m_TransKeyFrames = (KeyFrameBlock*)CollectPointers( m_pBones[i].m_TransKeyFrames, sizeof(KeyFrameBlock) );
		m_pBones[i].m_RotsKeyFrames = (KeyFrameBlock*)CollectPointers( m_pBones[i].m_RotsKeyFrames, sizeof(KeyFrameBlock) );
		m_pBones[i].m_ScaleKeyFrames = (ScaleKeyFrameBlock*)CollectPointers( m_pBones[i].m_ScaleKeyFrames, sizeof(ScaleKeyFrameBlock) );
		m_pBones[i].m_VisibleKeyFrames = (VisibleKeyFrameBlock*)CollectPointers( m_pBones[i].m_VisibleKeyFrames, sizeof(VisibleKeyFrameBlock) );

		pTransKeyFrames->m_KeyFrames = (SGPMF1KeyFrame *)CollectPointers( pTransKeyFrames->m_KeyFrames + TransKFStartOffset, sizeof(SGPMF1KeyFrame) * TransKFNum, TransKFStartOffset * sizeof(SGPMF1KeyFrame));
		pRotsKeyFrames->m_KeyFrames = (SGPMF1KeyFrame *)CollectPointers( pRotsKeyFrames->m_KeyFrames + RotsKFStartOffset, sizeof(SGPMF1KeyFrame) * RotsKFNum, RotsKFStartOffset * sizeof(SGPMF1KeyFrame));
		pScaleKeyFrames->m_KeyFrames = (SGPMF1ScaleKeyFrame *)CollectPointers( pScaleKeyFrames->m_KeyFrames + ScaleKFStartOffset, sizeof(SGPMF1ScaleKeyFrame) * ScaleKFNum, ScaleKFStartOffset * sizeof(SGPMF1ScaleKeyFrame));
		pVisibleKeyFrames->m_KeyFrames = (SGPMF1VisibleKeyFrame *)CollectPointers( pVisibleKeyFrames->m_KeyFrames + VisibleKFStartOffset, sizeof(SGPMF1VisibleKeyFrame) * VisibleKFNum, VisibleKFStartOffset * sizeof(SGPMF1VisibleKeyFrame));
	}
	

	BF1Header.m_iBoneGroupOffset = CollectPointers( m_pBoneGroup, sizeof(SGPMF1BoneGroup) * m_iNumBoneGroup );



	//-------------------------------------------------------------
	// Identify it by the absolute filenames if possible.
	String AbsolutePath = BoneFilename;	
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = WorkingDir +	File::separatorString + BoneFilename;
	}
	if( BoneFileIndex > 0 )
		AbsolutePath = AbsolutePath + String(BoneFileIndex);


	ScopedPointer<FileOutputStream> fileStream( File(AbsolutePath).createOutputStream() );
	if (fileStream != nullptr && fileStream->openedOk() )
    {
		fileStream->setPosition(0);
		fileStream->truncate();

		for( int i=0; i<s_CollectPtrs.size(); i++ )
		{
			fileStream->write( (const uint8*)s_CollectPtrs[i].pDataPointer, s_CollectPtrs[i].DataSize );
		}

		fileStream->flush();
	}
	else
		SaveResult = false;
	//-------------------------------------------------------------

	// Reset Pointers - For release memory correctly
	RelocateFirstPointers();							//SGPMF1BoneHeader
	(SGPMF1Bone*)RelocateFirstPointers();
	for(uint32 i=0; i<m_iNumBones; i++)
	{
		if( m_pBones[i].m_iNumChildId > 0 )
			m_pBones[i].m_ChildIds = (uint16*)RelocateFirstPointers();

		m_pBones[i].m_TransKeyFrames = (KeyFrameBlock*)RelocateFirstPointers();
		m_pBones[i].m_RotsKeyFrames = (KeyFrameBlock*)RelocateFirstPointers();
		m_pBones[i].m_ScaleKeyFrames = (ScaleKeyFrameBlock*)RelocateFirstPointers();
		m_pBones[i].m_VisibleKeyFrames = (VisibleKeyFrameBlock*)RelocateFirstPointers();

		m_pBones[i].m_TransKeyFrames->m_KeyFrames = (SGPMF1KeyFrame*)RelocateFirstPointers();
		m_pBones[i].m_RotsKeyFrames->m_KeyFrames = (SGPMF1KeyFrame*)RelocateFirstPointers();
		m_pBones[i].m_ScaleKeyFrames->m_KeyFrames = (SGPMF1ScaleKeyFrame*)RelocateFirstPointers();
		m_pBones[i].m_VisibleKeyFrames->m_KeyFrames = (SGPMF1VisibleKeyFrame*)RelocateFirstPointers();
	
		m_pBones[i].m_TransKeyFrames->m_iNumber = BackupTransFrameNum[i];
		m_pBones[i].m_RotsKeyFrames->m_iNumber = BackupRotsFrameNum[i];
		m_pBones[i].m_ScaleKeyFrames->m_iNumber = BackupScaleFrameNum[i];
		m_pBones[i].m_VisibleKeyFrames->m_iNumber = BackupVisibleFrameNum[i];		
	}
	(SGPMF1BoneGroup *)RelocateFirstPointers();

	return SaveResult;
}

//-------------------------------------------------------------
//- GetMeshPointFromSecondTexCoord
//- Get real vertex position and normal from mesh according to TexCoord1
//-------------------------------------------------------------
bool CSGPModelMF1::GetMeshPointFromSecondTexCoord( Vector3D& position, Vector3D& normal, const Vector2D& uv, const Matrix4x4& modelMatrix  )
{
	for( uint32 i=0; i<m_Header.m_iNumMeshes; i++ )
	{
		if( !m_pLOD0Meshes[i].m_pTexCoords1 || (m_pLOD0Meshes[i].m_iNumUV1==0) )
			continue;

		jassert( m_pLOD0Meshes[i].m_iNumUV0 == m_pLOD0Meshes[i].m_iNumUV1 );


		for( uint32 j=0; j<m_pLOD0Meshes[i].m_iNumIndices; j += 3 )
		{
			Vector3D v0(
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].vPos[0],
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].vPos[1],
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].vPos[2] );
			Vector3D v1(
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].vPos[0],
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].vPos[1],
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].vPos[2] );
			Vector3D v2(
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].vPos[0],
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].vPos[1],
				m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].vPos[2] );

			v0 = v0 * modelMatrix;
			v1 = v1 * modelMatrix;
			v2 = v2 * modelMatrix;

			Vector2D t0(m_pLOD0Meshes[i].m_pTexCoords1[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].m_fTexCoord[0],
						m_pLOD0Meshes[i].m_pTexCoords1[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].m_fTexCoord[1]	);
			Vector2D t1(m_pLOD0Meshes[i].m_pTexCoords1[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].m_fTexCoord[0],
						m_pLOD0Meshes[i].m_pTexCoords1[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].m_fTexCoord[1]	);
			Vector2D t2(m_pLOD0Meshes[i].m_pTexCoords1[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].m_fTexCoord[0],
						m_pLOD0Meshes[i].m_pTexCoords1[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].m_fTexCoord[1]	);

			Vector3D samplePos = TexcoordToPos( v0, v1, v2, t0, t1, t2, uv );
			if( !IsPointInsideTriangle(v0, v1, v2, samplePos, 0.1f) )
				continue;
			else
			{
				Vector4D n0(
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].vNormal[0],
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].vNormal[1],
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j  ]  ].vNormal[2],
					0);
				Vector4D n1(
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].vNormal[0],
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].vNormal[1],
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+1]  ].vNormal[2],
					0);
				Vector4D n2(
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].vNormal[0],
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].vNormal[1],
					m_pLOD0Meshes[i].m_pVertex[  m_pLOD0Meshes[i].m_pIndices[j+2]  ].vNormal[2],
					0);


				n0 = n0 * modelMatrix;
				n1 = n1 * modelMatrix;
				n2 = n2 * modelMatrix;

				Vector3D nn0(n0.x, n0.y, n0.z);
				Vector3D nn1(n1.x, n1.y, n1.z);
				Vector3D nn2(n2.x, n2.y, n2.z);
				nn0.Normalize();
				nn1.Normalize();
				nn2.Normalize();

				normal = TexcoordToPos( nn0, nn1, nn2, t0, t1, t2, uv );
				normal.Normalize();
				position = samplePos;

				return true;
			}
		}
	}
	return false;
}












//-------------------------------------------------------------
//	In this function you transfer the pDataBuffer pointers to file offset according to DataSize.
//  the pointers are adjusted to become an offset from the beginning of the block.
//-------------------------------------------------------------
uint32 CSGPModelMF1::CollectPointers( void* pDataBuffer, uint32 DataSize, uint32 RawOffset )
{
	if( !pDataBuffer || DataSize == 0 )
		return 0;

	PtrOffset DataOffset;
	DataOffset.pDataPointer = pDataBuffer;
	DataOffset.DataSize = DataSize;
	DataOffset.RawOffset = RawOffset;
	s_CollectPtrs.add(DataOffset);

	s_CurFileOffset += DataSize;

	return s_CurFileOffset - DataSize;
}

//Return recorded original pointer from Array
void* CSGPModelMF1::RelocateFirstPointers()
{
	PtrOffset DataOffset = s_CollectPtrs.remove(0);
	return (void*)((intptr_t)DataOffset.pDataPointer - DataOffset.RawOffset);
}