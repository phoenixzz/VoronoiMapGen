
ISGPModelManager::ISGPModelManager(ISGPRenderDevice *pdevice, Logger* logger)
	:	m_pRenderDevice(pdevice), m_pLogger(logger), 
		m_StringToModelIDMap(INIT_MODELARRAYSIZE)
{
	m_MF1Models.ensureStorageAllocated(INIT_MODELARRAYSIZE);
}

ISGPModelManager::~ISGPModelManager()
{
	for( int i=0; i<m_MF1Models.size(); ++i )
	{
		CMF1FileResource* pMF1Resource = m_MF1Models.getUnchecked(i);
		if( pMF1Resource && pMF1Resource->pModelMF1 )
		{
			m_pLogger->writeToLog(String("Memory Leak: MF1 Model is not deleted: ")+String(pMF1Resource->pModelMF1->m_Header.m_cFilename), ELL_ERROR);
			m_pLogger->writeToLog(String("Reference Count = ") + String(pMF1Resource->getReferenceCount()), ELL_ERROR);
		}
	}
	m_MF1Models.clear(true);


	HashMap<uint64, uint32>::Iterator i (m_StringToModelIDMap);
	while( i.next() )
	{
		uint32 ModelID = i.getValue();
		m_pLogger->writeToLog(String("Model ID is : ")+String(ModelID), ELL_ERROR);
	}
}

uint32 ISGPModelManager::getModelCount() const
{
	uint32 count = 0;
	for( int i=0; i<m_MF1Models.size(); ++i )
		if( m_MF1Models.getUnchecked(i) != NULL ) count++;

	return count;
}

uint32 ISGPModelManager::getFirstEmptyID()
{
	// model Array is not full
	if( m_MF1Models.size() < INIT_MODELARRAYSIZE )
	{
		m_MF1Models.add(NULL);
		return m_MF1Models.size()-1;
	}


	// Find first empty array slot
	for( int i=0; i<m_MF1Models.size(); ++i )
		if( m_MF1Models.getUnchecked(i) == NULL )
			return (uint32)i;

	// model array is full, first alloc one new space, then return id
	m_MF1Models.add(NULL);
	return m_MF1Models.size()-1;
}

uint32 ISGPModelManager::registerModel(const String& modelfilename, bool bLoadBoneAnim)
{
	uint32 ModelID = getModelIDByName(modelfilename);
	if( ModelID != 0xFFFFFFFF )
	{
		getModelByID(ModelID)->incReferenceCount();
		return ModelID;
	}

	// try to load MF1 file
	CMF1FileResource *pMF1ModelRes = new CMF1FileResource();	
	pMF1ModelRes->incReferenceCount();
	pMF1ModelRes->pMF1RawMemoryAddress = CSGPModelMF1::LoadMF1(pMF1ModelRes->pModelMF1, m_WorkingDir, modelfilename);
	if( bLoadBoneAnim )
	{
		String BoneAnimFileName = modelfilename.dropLastCharacters(3) + String( "bf1" );		
		pMF1ModelRes->pBF1RawMemoryAddress.add( CSGPModelMF1::LoadBone(pMF1ModelRes->pModelMF1, m_WorkingDir, BoneAnimFileName, 0) );
	}

	// Create render resource
	createRenderResource(pMF1ModelRes);


	ModelID = getFirstEmptyID();
	uint64 HashVal = WChar_tStringHash(modelfilename.toWideCharPointer(), modelfilename.length());

	m_MF1Models.set(ModelID, pMF1ModelRes, false);
	m_StringToModelIDMap.set( HashVal, ModelID );

	return ModelID;
}

uint32 ISGPModelManager::registerModelMT(const String& modelfilename, bool bLoadBoneAnim)
{
	uint32 ModelID = getModelIDByName(modelfilename);
	if( ModelID != 0xFFFFFFFF )
	{
		getModelByID(ModelID)->incReferenceCount();
		return ModelID;
	}

	m_pRenderDevice->GetMTResourceLoader()->addLoadingModel(modelfilename, bLoadBoneAnim ? 0 : 0xFFFF);		
	return 0xFFFFFFFF;
}

uint32 ISGPModelManager::attachActionInfoByName(const String& modelfilename, uint16 boneAnimFileIndex)
{
	uint32 ModelID = getModelIDByName(modelfilename);
	if( ModelID != 0xFFFFFFFF )
	{
		CMF1FileResource *pMF1ModelRes = m_MF1Models[ModelID];

		if( pMF1ModelRes )
		{
			String BoneAnimFileName = modelfilename.dropLastCharacters(3) + String( "bf1" );		
			pMF1ModelRes->pBF1RawMemoryAddress.add( CSGPModelMF1::LoadBone(pMF1ModelRes->pModelMF1, m_WorkingDir, BoneAnimFileName, boneAnimFileIndex) );
		}
	}
	return ModelID;
}

uint32 ISGPModelManager::attachActionInfoByID(uint32 id, uint16 boneAnimFileIndex)
{
	CMF1FileResource *pMF1ModelRes = getModelByID(id);

	if( pMF1ModelRes )
	{		
		String BoneAnimFileName = String(pMF1ModelRes->pModelMF1->m_Header.m_cFilename).dropLastCharacters(3) + String( "bf1" );		
		pMF1ModelRes->pBF1RawMemoryAddress.add( CSGPModelMF1::LoadBone(pMF1ModelRes->pModelMF1, m_WorkingDir, BoneAnimFileName, boneAnimFileIndex) );
	}
	
	return id;
}


uint32 ISGPModelManager::getModelIDByName(const String& modelfilename)
{
	uint64 HashVal = WChar_tStringHash(modelfilename.toWideCharPointer(), modelfilename.length());
	
	if( m_StringToModelIDMap.contains(HashVal) )
	{
		return m_StringToModelIDMap[HashVal];
	}
	return 0xFFFFFFFF;
}

CMF1FileResource* ISGPModelManager::getModelByID(uint32 id)
{
	if( id < (uint32)m_MF1Models.size() )
		return m_MF1Models[id];
	return NULL;
}

void ISGPModelManager::unRegisterModelByName( const String& modelfilename )
{
	uint64 HashVal = WChar_tStringHash(modelfilename.toWideCharPointer(), modelfilename.length());

	if( m_StringToModelIDMap.contains(HashVal) )
	{
		uint32 ModelID = m_StringToModelIDMap[HashVal];

		if( m_MF1Models.getUnchecked(ModelID) )
			m_MF1Models.getUnchecked(ModelID)->decReferenceCount();
		if( m_MF1Models.getUnchecked(ModelID)->getReferenceCount() == 0 )
		{
			// Release render resource
			releaseRenderResource( m_MF1Models.getUnchecked(ModelID) );

			m_StringToModelIDMap.remove(HashVal);
			m_MF1Models.set(ModelID, NULL, true);
		}
	}
}

void ISGPModelManager::unRegisterModelByID(uint32 id)
{
	if( m_MF1Models.getUnchecked(id) )
		m_MF1Models.getUnchecked(id)->decReferenceCount();
	if( m_MF1Models.getUnchecked(id)->getReferenceCount() == 0 )
	{
		// Release render resource
		releaseRenderResource( m_MF1Models.getUnchecked(id) );

		m_StringToModelIDMap.removeValue(id);
		m_MF1Models.set(id, NULL, true);
	}
}

void ISGPModelManager::unRegisterModelByNameMT( const String& modelfilename )
{
	uint64 HashVal = WChar_tStringHash(modelfilename.toWideCharPointer(), modelfilename.length());

	if( m_StringToModelIDMap.contains(HashVal) )
	{
		uint32 ModelID = m_StringToModelIDMap[HashVal];
		if( m_MF1Models.getUnchecked(ModelID) )
			m_MF1Models.getUnchecked(ModelID)->decReferenceCount();
		if( m_MF1Models.getUnchecked(ModelID)->getReferenceCount() == 0 )
		{
			m_pRenderDevice->GetMTResourceLoader()->addDeletingModel(m_MF1Models.getUnchecked(ModelID), modelfilename);	
		}
	}
	else
		m_pRenderDevice->GetMTResourceLoader()->addDeletingModel( NULL, modelfilename );
}

void ISGPModelManager::unRegisterModelByIDMT(uint32 id)
{
	if( m_MF1Models.getUnchecked(id) )
		m_MF1Models.getUnchecked(id)->decReferenceCount();
	if( m_MF1Models.getUnchecked(id)->getReferenceCount() == 0 )
	{
		m_pRenderDevice->GetMTResourceLoader()->addDeletingModel(m_MF1Models.getUnchecked(id), String(m_MF1Models.getUnchecked(id)->pModelMF1->m_Header.m_cFilename));	
	}	
}

//  Without considering the reference count
void ISGPModelManager::removeAllModels()
{
	m_StringToModelIDMap.clear();

	m_MF1Models.clear(true);
}

void ISGPModelManager::registerSkinTexturesMT(CMF1FileResource* pMF1FileRes)
{
	CSGPModelMF1* pMF1Model = pMF1FileRes->pModelMF1;
	if( !pMF1Model )
		return;

	// Register Used skin Textures
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumSkins; i++ )
	{
		m_pRenderDevice->GetTextureManager()->registerTextureMT( String(pMF1Model->m_pSkins[i].m_cName) );
	}
	// Register Particle system Used Textures
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumParticles; i++ )
	{
		uint32 nGroupCount = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_groupCount;
		
		for(uint32 j=0; j<nGroupCount; ++j)
		{
			const ParticleRenderParam& renderParam = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_pGroupParam[j].m_RenderParam;
			switch(renderParam.m_type)
			{
			case Render_Point:				
				m_pRenderDevice->GetTextureManager()->registerTextureMT( String(renderParam.m_pointData.m_texPath) );
				break;
			case Render_Quad:
				m_pRenderDevice->GetTextureManager()->registerTextureMT( String(renderParam.m_quadData.m_texPath) );
				break;
            default:
                break;
			}
		}
	}
}

void ISGPModelManager::unRegisterSkinTexturesMT(CMF1FileResource* pMF1FileRes)
{
	CSGPModelMF1* pMF1Model = pMF1FileRes->pModelMF1;
	if( !pMF1Model )
		return;

	// unRegister Used skin Textures
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumSkins; i++ )
	{
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByNameMT(String(pMF1Model->m_pSkins[i].m_cName));
	}
	// unRegister Used particle system Textures
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumParticles; i++ )
	{
		uint32 nGroupCount = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_groupCount;
		for(uint32 j=0; j<nGroupCount; ++j)
		{
			const ParticleRenderParam& renderParam = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_pGroupParam[j].m_RenderParam;
			switch(renderParam.m_type)
			{
			case Render_Point:
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByNameMT( String(renderParam.m_pointData.m_texPath) );
				break;
			case Render_Quad:
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByNameMT( String(renderParam.m_quadData.m_texPath) );
				break;
            default:
				break;
			}
		}	
	}
}

void ISGPModelManager::createRenderResource(CMF1FileResource* pMF1FileRes)
{
	CSGPModelMF1* pMF1Model = pMF1FileRes->pModelMF1;
	if( !pMF1Model )
		return;


	// Register Used skin Textures
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumSkins; i++ )
	{
		m_pRenderDevice->GetTextureManager()->registerTexture(String(pMF1Model->m_pSkins[i].m_cName));
	}
	// Register Particle system Used Textures
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumParticles; i++ )
	{
		uint32 nGroupCount = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_groupCount;
		for(uint32 j=0; j<nGroupCount; ++j)
		{
			const ParticleRenderParam& renderParam = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_pGroupParam[j].m_RenderParam;
			switch(renderParam.m_type)
			{
			case Render_Point:
				m_pRenderDevice->GetTextureManager()->registerTexture(String(renderParam.m_pointData.m_texPath));
				break;
			case Render_Quad:
				m_pRenderDevice->GetTextureManager()->registerTexture(String(renderParam.m_quadData.m_texPath));
				break;
            default:
                break;
			}
		}	
	}

	// Create Static Mesh
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
	{
		const SGPMF1Skin& MeshSkin = ( pMF1Model->m_pLOD0Meshes[i].m_SkinIndex == 0xFFFFFFFF ) ? getDefaultMF1Skin() :
			pMF1Model->m_pSkins[ pMF1Model->m_pLOD0Meshes[i].m_SkinIndex ];

		const SGPMF1Mesh& MF1Mesh = pMF1Model->m_pLOD0Meshes[i];
		pMF1FileRes->StaticMeshIDArray.add(
			m_pRenderDevice->GetVertexCacheManager()->CreateMF1MeshStaticBuffer( MeshSkin, MF1Mesh, pMF1Model->m_pBoneGroup, pMF1Model->m_iNumBoneGroup ) );
	}
	// Create Particle System
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumParticles; i++ )
	{
		pMF1FileRes->ParticleSystemIDArray.add(
			m_pRenderDevice->createOpenGLParticleSystem( pMF1Model->m_pParticleEmitter[i]) );
	}
}

void ISGPModelManager::releaseRenderResource(CMF1FileResource* pMF1FileRes)
{
	CSGPModelMF1* pMF1Model = pMF1FileRes->pModelMF1;
	if( !pMF1Model )
		return;

	// unRegister skin texture
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumSkins; i++ )
	{
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(String(pMF1Model->m_pSkins[i].m_cName));
	}
	// unRegister particle system texture
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumParticles; i++ )
	{
		uint32 nGroupCount = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_groupCount;
		for(uint32 j=0; j<nGroupCount; ++j)
		{
			const ParticleRenderParam& renderParam = pMF1Model->m_pParticleEmitter[i].m_SystemParam.m_pGroupParam[j].m_RenderParam;
			switch(renderParam.m_type)
			{
			case Render_Point:
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(String(renderParam.m_pointData.m_texPath));
				break;
			case Render_Quad:
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(String(renderParam.m_quadData.m_texPath));
				break;
            default:
                break;
			}
		}	
	}

	// Release static mesh
	for( int i=0; i<pMF1FileRes->StaticMeshIDArray.size(); i++ )
	{
		m_pRenderDevice->GetVertexCacheManager()->ClearStaticBuffer( pMF1FileRes->StaticMeshIDArray[i] );
	}
	// Release particle system
	for( int i=0; i<pMF1FileRes->ParticleSystemIDArray.size(); i++ )
	{
		m_pRenderDevice->GetParticleManager()->clearParticleSystemByID( pMF1FileRes->ParticleSystemIDArray[i] );
	}
}

void ISGPModelManager::createRenderResourceMT(SGPModelRecord Record)
{
	CSGPModelMF1* pMF1Model = Record.pMF1Resource->pModelMF1;
	if( !pMF1Model )
		return;

	// Create Static Mesh
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
	{
		const SGPMF1Skin& MeshSkin = ( pMF1Model->m_pLOD0Meshes[i].m_SkinIndex == 0xFFFFFFFF ) ? getDefaultMF1Skin() :
			pMF1Model->m_pSkins[ pMF1Model->m_pLOD0Meshes[i].m_SkinIndex ];

		const SGPMF1Mesh& MF1Mesh = pMF1Model->m_pLOD0Meshes[i];
		Record.pMF1Resource->StaticMeshIDArray.add(
			m_pRenderDevice->GetVertexCacheManager()->CreateMF1MeshStaticBuffer( MeshSkin, MF1Mesh, pMF1Model->m_pBoneGroup, pMF1Model->m_iNumBoneGroup ) );
	}

	// Create Particle System
	for( uint32 i=0; i<pMF1Model->m_Header.m_iNumParticles; i++ )
	{
		Record.pMF1Resource->ParticleSystemIDArray.add(
			m_pRenderDevice->createOpenGLParticleSystem( pMF1Model->m_pParticleEmitter[i]) );
	}

	Record.pMF1Resource->incReferenceCount();
	for( uint32 i=0; i<Record.nRefCount; i++ )
		Record.pMF1Resource->incReferenceCount();


	uint32 ModelID = getFirstEmptyID();
	uint64 HashVal = WChar_tStringHash(Record.MF1AbsoluteFileName.toWideCharPointer(), Record.MF1AbsoluteFileName.length());

	m_MF1Models.set(ModelID, Record.pMF1Resource, false);
	m_StringToModelIDMap.set( HashVal, ModelID );

	return;
}

void ISGPModelManager::releaseRenderResourceMT(SGPModelRecord Record)
{
	CSGPModelMF1* pMF1Model = Record.pMF1Resource->pModelMF1;
	if( !pMF1Model )
		return;

	// Release Static Mesh
	for( int i=0; i<Record.pMF1Resource->StaticMeshIDArray.size(); i++ )
	{
		m_pRenderDevice->GetVertexCacheManager()->ClearStaticBuffer( Record.pMF1Resource->StaticMeshIDArray[i] );
	}

	// Release Particle system
	for( int i=0; i<Record.pMF1Resource->ParticleSystemIDArray.size(); i++ )
	{
		m_pRenderDevice->GetParticleManager()->clearParticleSystemByID( Record.pMF1Resource->ParticleSystemIDArray[i] );
	}

	uint32 ModelID = getModelIDByName(Record.MF1AbsoluteFileName);
	if( ModelID != 0xFFFFFFFF )
	{
		m_StringToModelIDMap.removeValue(ModelID);
		m_MF1Models.set(ModelID, NULL, true);
	}
}

const SGPMF1Skin& ISGPModelManager::getDefaultMF1Skin()
{
	static SGPMF1Skin DefaultSkin;

	if( DefaultSkin.m_iNumMatKeyFrame == 0 )
	{
		DefaultSkin.m_iNumMatKeyFrame = 1;
		DefaultSkin.m_pMatKeyFrame = new SGPMF1MatKeyFrame();
		DefaultSkin.m_pMatKeyFrame->m_iFrameID = 0;
		DefaultSkin.m_pMatKeyFrame->m_fUOffset = DefaultSkin.m_pMatKeyFrame->m_fVOffset = 0;
		DefaultSkin.m_pMatKeyFrame->m_MaterialColor[0] = DefaultSkin.m_pMatKeyFrame->m_MaterialColor[1] = DefaultSkin.m_pMatKeyFrame->m_MaterialColor[2] = 0.5859375f;
		DefaultSkin.m_pMatKeyFrame->m_MaterialColor[3] = 1.0f;
	}

	return DefaultSkin;
}
