

void CSGPResourceLoaderMuitiThread::run()
{
    // this is the code that runs this thread - we'll loop continuously,

    // threadShouldExit() returns true when the stopThread() method has been
    // called, so we should check it often, and exit as soon as it gets flagged.
    while (! threadShouldExit())
    {
        // sleep a bit so the threads don't all grind the CPU to a halt..
        wait (interval);

		const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);


		// Deleting
		// DO Release models
		for( int i=0; i<m_DeletingModels.size(); i++ )
		{
			if( !m_DeletingModels.getReference(i).pMF1Resource )
				continue;
			if( m_DeletingModels.getReference(i).bReady )
				continue;
			if( m_DeletingModels.getReference(i).pMF1Resource->getReferenceCount() > 0 )
			{
				m_DeletingModels.remove(i);
				i--;
				continue;
			}
			if( m_pDevice->getRenderDeviceTime() - m_DeletingModels.getReference(i).pMF1Resource->deleteTimeStamp > RESOURCE_BONE_TO_FREE_KEEPTIME )
			{
				Logger::getCurrentLogger()->writeToLog(String("Delete MF1 Model in Other Thread") + m_DeletingModels.getReference(i).MF1AbsoluteFileName, ELL_INFORMATION);

				// Immediately, try to unRegisterMT used textures			
				m_pDevice->GetModelManager()->unRegisterSkinTexturesMT(m_DeletingModels.getReference(i).pMF1Resource);

				// Setting flags, In Render Thread, will release render resource				
				m_DeletingModels.getReference(i).bReady = true;
			}
		}

		// Do Release Textures
		for( int i=0; i<m_DeletingTextures.size(); i++ )
		{
			if( !m_DeletingTextures.getReference(i).pTexResource )
				continue;
			if( m_DeletingTextures.getReference(i).bReady )
				continue;

			if( m_DeletingTextures.getReference(i).pTexResource->getReferenceCount() > 0 )
			{
				m_DeletingTextures.remove(i);
				i--;
				continue;
			}
			if( m_pDevice->getRenderDeviceTime() - m_DeletingTextures.getReference(i).pTexResource->deleteTimeStamp > RESOURCE_BONE_TO_FREE_KEEPTIME )
			{
				// Setting flags, In Render Thread, will release render resource
				// Also Remove StringToTextureIDMap and TextureArray in TextureManager
				Logger::getCurrentLogger()->writeToLog(String("Delete texture in Other Thread") + m_DeletingTextures.getReference(i).TexFileName, ELL_INFORMATION);
				m_DeletingTextures.getReference(i).bReady = true;
			}
		}
			


		// Loading
		// Do Load Models
		for( int i=0; i<m_DeletingModels.size(); i++ )
		{
			if( m_DeletingModels.getReference(i).pMF1Resource != NULL )
				continue;
			else
			{
				int idx = m_LoadingModels.indexOf( m_DeletingModels.getReference(i) );
				if( idx != -1 )
				{
					if(	m_LoadingModels.getReference(idx).nRefCount > 0 )
						m_LoadingModels.getReference(idx).nRefCount--;
					else
					{
						m_LoadingModels.remove(idx);
						m_DeletingModels.remove(i);
						i--;
					}
				}
			}
		}
		for( int i=0; i<m_LoadingModels.size(); i++ )
		{
			if( m_LoadingModels.getReference(i).bReady )
				continue;
			//Load Raw MF1 file data 
			m_LoadingModels.getReference(i).pMF1Resource = new CMF1FileResource();
			m_LoadingModels.getReference(i).pMF1Resource->pMF1RawMemoryAddress = 
				CSGPModelMF1::LoadMF1( 	m_LoadingModels.getReference(i).pMF1Resource->pModelMF1,
										m_pDevice->GetModelManager()->getWorkingDirection(),
										m_LoadingModels.getReference(i).MF1AbsoluteFileName );
			if( m_LoadingModels.getReference(i).BF1FileIndex != 0xFFFF )
			{
				String BoneAnimFileName = m_LoadingModels.getReference(i).MF1AbsoluteFileName.dropLastCharacters(3) + String( "bf1" );		
				m_LoadingModels.getReference(i).pMF1Resource->pBF1RawMemoryAddress.add(
					CSGPModelMF1::LoadBone(	m_LoadingModels.getReference(i).pMF1Resource->pModelMF1,
											m_pDevice->GetModelManager()->getWorkingDirection(),
											BoneAnimFileName, 0 ) );
			}

			// If MF1 files can not be opened, delete this Model Resource
			if( !m_LoadingModels.getReference(i).pMF1Resource->pMF1RawMemoryAddress )
			{
				delete m_LoadingModels.getReference(i).pMF1Resource;
				m_LoadingModels.getReference(i).pMF1Resource = NULL;
				m_LoadingModels.remove(i);
				i--;
			}
			else
			{
				Logger::getCurrentLogger()->writeToLog(String("Loading MF1 Model in Other Thread : ") + m_LoadingModels.getReference(i).MF1AbsoluteFileName, ELL_INFORMATION);
				
				// Immediately, try to registerMT used textures			
				m_pDevice->GetModelManager()->registerSkinTexturesMT(m_LoadingModels.getReference(i).pMF1Resource);

				// Setting flags, In Render Thread, will create render resource			
				m_LoadingModels.getReference(i).bReady = true;
			}
		}

		// Loading
		// Do Load Textures
		for( int i=0; i<m_DeletingTextures.size(); i++ )
		{
			if( m_DeletingTextures.getReference(i).pTexResource != NULL )
				continue;
			else
			{
				int idx = m_LoadingTextures.indexOf( m_DeletingTextures.getReference(i) );
				if( idx != -1 )
				{
					if( m_LoadingTextures.getReference(idx).nRefCount > 0 )
						m_LoadingTextures.getReference(idx).nRefCount--;
					else
					{
						m_LoadingTextures.remove(idx);
						m_DeletingTextures.remove(i);
						i--;
					}
				}
			}
		}

		for( int i=0; i<m_LoadingTextures.size(); i++ )
		{
			if( m_LoadingTextures.getReference(i).bReady )
				continue;
			//Load Raw texture data
			m_LoadingTextures.getReference(i).pTexResource = new CTextureResource();
			m_LoadingTextures.getReference(i).pTexResource->pSGPImage = m_pDevice->GetTextureManager()->createImageFromFile(m_LoadingTextures.getReference(i).TexFileName);
			
			// If texture files can not be opened, delete this texture Resource
			if( !m_LoadingTextures.getReference(i).pTexResource->pSGPImage )
			{
				delete m_LoadingTextures.getReference(i).pTexResource;
				m_LoadingTextures.getReference(i).pTexResource = NULL;
				m_LoadingTextures.remove(i);
				i--;
			}
			else
			{
				Logger::getCurrentLogger()->writeToLog(String("Loading texture in Other Thread : ") + m_LoadingTextures.getReference(i).TexFileName, ELL_INFORMATION);

				// Setting flags, In Render Thread, will create render resource
				m_LoadingTextures.getReference(i).bReady = true;
			}
		}
    }
}


void CSGPResourceLoaderMuitiThread::addLoadingTexture(const String& texturename, bool bGenMipMap)
{
	SGPTextureRecord Record;
	Record.TexFileName = texturename;
	Record.bGenMipMap = bGenMipMap;
	
	{
		const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

		int idx = m_LoadingTextures.indexOf( Record );
		if( idx != -1 )
			m_LoadingTextures.getReference(idx).nRefCount++;
		else
			m_LoadingTextures.add(Record);
	}
}

void CSGPResourceLoaderMuitiThread::addDeletingTexture(CTextureResource *pTextureRes, const String& texturename)
{	
	SGPTextureRecord Record;
	Record.pTexResource = pTextureRes;
	Record.TexFileName = texturename;
	if( pTextureRes )
		pTextureRes->deleteTimeStamp = m_pDevice->getRenderDeviceTime();

	
	{
		const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

		m_DeletingTextures.add(Record);

	}
}

void CSGPResourceLoaderMuitiThread::addLoadingModel(const String& modelname, uint16 BF1FileIndex)
{

	SGPModelRecord Record;
	Record.MF1AbsoluteFileName = modelname;
	Record.BF1FileIndex = BF1FileIndex;

	{
		const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

		int idx = m_LoadingModels.indexOf( Record );
		if( idx != -1 )
			m_LoadingModels.getReference(idx).nRefCount++;
		else
			m_LoadingModels.add(Record);
	}

}

void CSGPResourceLoaderMuitiThread::addDeletingModel(CMF1FileResource *pModelRes, const String& modelname)
{
	SGPModelRecord Record;
	Record.MF1AbsoluteFileName = modelname;
	Record.pMF1Resource = pModelRes;
	if( pModelRes )
		pModelRes->deleteTimeStamp = m_pDevice->getRenderDeviceTime();

	{
		const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

		m_DeletingModels.add(Record);
	}	
}


void CSGPResourceLoaderMuitiThread::syncRenderResource()
{
	const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

	for( int i=0; i<m_LoadingTextures.size(); i++ )
	{
		if( m_LoadingTextures.getReference(i).bReady &&
			m_LoadingTextures.getReference(i).pTexResource &&
			m_LoadingTextures.getReference(i).pTexResource->pSGPImage )
		{
			// In render thread, create render resource
			m_pDevice->GetTextureManager()->registerTextureFromResourceMT(m_LoadingTextures.getReference(i));
			delete m_LoadingTextures.getReference(i).pTexResource->pSGPImage;
			m_LoadingTextures.getReference(i).pTexResource->pSGPImage = NULL;
			Logger::getCurrentLogger()->writeToLog(String("Create Render texture in Render Thread") + m_LoadingTextures.getReference(i).TexFileName, ELL_INFORMATION);
			
			m_LoadingTextures.remove(i);
			i--;
		}
	}

	for( int i=0; i<m_DeletingTextures.size(); i++ )
	{
		if( m_DeletingTextures.getReference(i).bReady &&
			m_DeletingTextures.getReference(i).pTexResource )
		{
			// In render thread, Also Remove StringToTextureIDMap and TextureArray in TextureManager
			m_pDevice->GetTextureManager()->unRegisterTextureFromResourceMT(m_DeletingTextures.getReference(i));
			Logger::getCurrentLogger()->writeToLog(String("Delete Render texture in Render Thread") + m_DeletingTextures.getReference(i).TexFileName, ELL_INFORMATION);
			
			m_DeletingTextures.remove(i);
			i--;
		}
	}


	for( int i=0; i<m_LoadingModels.size(); i++ )
	{
		if( m_LoadingModels.getReference(i).bReady &&
			m_LoadingModels.getReference(i).pMF1Resource &&
			m_LoadingModels.getReference(i).pMF1Resource->pMF1RawMemoryAddress )
		{
			// In render thread, create render resource
			// In render thread, Also set new m_StringToModelIDMap and MF1Models Array in ModelManager
			m_pDevice->GetModelManager()->createRenderResourceMT(m_LoadingModels.getReference(i));

			Logger::getCurrentLogger()->writeToLog(String("Create Static Mesh in Render Thread") + m_LoadingModels.getReference(i).MF1AbsoluteFileName, ELL_INFORMATION);

			m_LoadingModels.remove(i);
			i--;
		}

	}
	for( int i=0; i<m_DeletingModels.size(); i++ )
	{
		if( m_DeletingModels.getReference(i).bReady &&
			m_DeletingModels.getReference(i).pMF1Resource )
		{
			// In render thread, release render resource
			// In render thread, Also Remove m_StringToModelIDMap and MF1Models Array in ModelManager
			m_pDevice->GetModelManager()->releaseRenderResourceMT(m_DeletingModels.getReference(i));

			Logger::getCurrentLogger()->writeToLog(String("Delete Static Mesh in Render Thread") + m_DeletingModels.getReference(i).MF1AbsoluteFileName, ELL_INFORMATION);

			m_DeletingModels.remove(i);
			i--;
		}

	}
}


void CSGPResourceLoaderMuitiThread::removeAll()
{
	const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

	// Created in Loading thread, but not created render resource textures and models
	for( int i=0; i<m_LoadingTextures.size(); i++ )
	{
		if( m_LoadingTextures.getReference(i).bReady &&
			m_LoadingTextures.getReference(i).pTexResource &&
			m_LoadingTextures.getReference(i).pTexResource->pSGPImage )
		{
			delete m_LoadingTextures.getReference(i).pTexResource;
			m_LoadingTextures.getReference(i).pTexResource = NULL;
			delete m_LoadingTextures.getReference(i).pTexResource->pSGPImage;
			m_LoadingTextures.getReference(i).pTexResource->pSGPImage = NULL;
		}
	}

	for( int i=0; i<m_LoadingModels.size(); i++ )
	{
		if( m_LoadingModels.getReference(i).bReady &&
			m_LoadingModels.getReference(i).pMF1Resource )
		{
			delete m_LoadingModels.getReference(i).pMF1Resource;
			m_LoadingModels.getReference(i).pMF1Resource = NULL;
		}
	}

	m_LoadingModels.clear();
	m_LoadingTextures.clear();

	for( int i=0; i<m_DeletingModels.size(); i++ )
	{
		if( m_DeletingModels.getReference(i).pMF1Resource )
		{
			// Immediately, try to unRegisterMT used textures			
			m_pDevice->GetModelManager()->unRegisterSkinTexturesMT(m_DeletingModels.getReference(i).pMF1Resource);
			m_pDevice->GetModelManager()->releaseRenderResourceMT(m_DeletingModels.getReference(i));
			Logger::getCurrentLogger()->writeToLog(String("Delete Static Mesh in Render Thread") + m_DeletingModels.getReference(i).MF1AbsoluteFileName, ELL_INFORMATION);
		}
	}
	for( int i=0; i<m_DeletingTextures.size(); i++ )
	{
		if( m_DeletingTextures.getReference(i).pTexResource )
		{
			m_pDevice->GetTextureManager()->unRegisterTextureFromResourceMT(m_DeletingTextures.getReference(i));
			Logger::getCurrentLogger()->writeToLog(String("Delete Render texture in Render Thread") + m_DeletingTextures.getReference(i).TexFileName, ELL_INFORMATION);
		}
	}


	m_DeletingModels.clear();
	m_DeletingTextures.clear();
}
