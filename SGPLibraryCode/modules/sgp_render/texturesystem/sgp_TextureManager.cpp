


CSGPTextureManager::CSGPTextureManager(ISGPRenderDevice *pdevice, Logger* logger)
	:m_pRenderDevice(pdevice), m_pLogger(logger), m_TextureCreationFlags(0),
	m_StringToTextureIDMap(INIT_TEXTUREARRAYSIZE)
{
	m_Textures.ensureStorageAllocated(INIT_TEXTUREARRAYSIZE);

#if defined (BUILD_OGLES2)
	// Now TextureManager only support PVRTC texture
	m_ImageLoaderArray.add( createImageLoaderPVRTC() );
#else
	// Now TextureManager support DDS and TGA texture
	m_ImageLoaderArray.add( createImageLoaderDDS() );
	m_ImageLoaderArray.add( createImageLoaderTGA() );
#endif
}

CSGPTextureManager::~CSGPTextureManager()
{
	releaseDefaultTexture();
	releaseWhiteTexture();
	releaseBlackTexture();

	for( int i=0; i<m_Textures.size(); ++i )
	{
		CTextureResource* foundTex = m_Textures.getUnchecked(i);
		if( foundTex && foundTex->pSGPTexture)
		{
			m_pLogger->writeToLog(String("Memory Leak: Texture is not deleted: ")+foundTex->pSGPTexture->getName(), ELL_ERROR);
			m_pLogger->writeToLog(String("Reference Count = ") + String(foundTex->getReferenceCount()), ELL_ERROR);
		}
	}
	m_Textures.clear(true);


	HashMap<uint64, uint32>::Iterator i (m_StringToTextureIDMap);
	while( i.next() )
	{
		uint32 TexID = i.getValue();
		m_pLogger->writeToLog(String("Texture ID is : ")+String(TexID), ELL_ERROR);
	}
}

void CSGPTextureManager::createDefaultTexture()
{
	uint32 PinkColor = 0xffffc0cb;
	ISGPImage* pimg = createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(1,1),
		&PinkColor,
		false);

	uint32 TexID = registerTextureFromImage(String("Default"), pimg, false);
	jassert( TexID == 0 );

	delete pimg;
	pimg = NULL;
}

void CSGPTextureManager::releaseDefaultTexture()
{
	unRegisterTextureByID(0);
}

void CSGPTextureManager::createWhiteTexture()
{
	uint32 WhiteColor = 0xffffffff;
	ISGPImage* pimg = createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(1,1),
		&WhiteColor,
		false);

	uint32 TexID = registerTextureFromImage(String("White"), pimg, false);
	jassert( TexID == 1 );

	delete pimg;
	pimg = NULL;
}

void CSGPTextureManager::releaseWhiteTexture()
{
	unRegisterTextureByID(1);
}

void CSGPTextureManager::createBlackTexture()
{
	uint32 BlackColor = 0xff000000;
	ISGPImage* pimg = createImageFromData(SGPPF_A8R8G8B8, 
		SDimension2D(1,1),
		&BlackColor,
		false);

	uint32 TexID = registerTextureFromImage(String("Black"), pimg, false);
	jassert( TexID == 2 );

	delete pimg;
	pimg = NULL;
}

void CSGPTextureManager::releaseBlackTexture()
{
	unRegisterTextureByID(2);
}

uint32 CSGPTextureManager::getImageLoaderCount() const
{
	return m_ImageLoaderArray.size();
}

ISGPImageLoader* CSGPTextureManager::getImageLoader(int idx)
{
	if(idx < m_ImageLoaderArray.size())
		return m_ImageLoaderArray[idx];
	return NULL;
}

//! Creates an empty software image.
ISGPImage* CSGPTextureManager::createImage(SGP_PIXEL_FORMAT format, const SDimension2D& size)
{
	if(ISGPImage::isRenderTargetOnlyFormat(format))
	{
		m_pLogger->writeToLog(String("Could not create ISGPImage, format only supported for render target textures."), ELL_WARNING);
		return 0;
	}

	return new SGPImageUncompressed(format, size);
}

ISGPImage* CSGPTextureManager::createImageFromData(SGP_PIXEL_FORMAT format,
	const SDimension2D& size, void *data,
	bool ownForeignMemory)
{
	if(ISGPImage::isRenderTargetOnlyFormat(format))
	{
		m_pLogger->writeToLog(String("Could not create ISGPImage, format only supported for render target textures."), ELL_WARNING);
		return 0;
	}

	return new SGPImageUncompressed(format, size, data, ownForeignMemory);
}

//! Enables or disables a texture creation flag.
void CSGPTextureManager::setTextureCreationFlag(SGP_TEXTURE_CREATION_FLAG flag, bool enabled)
{
	// set flag
	m_TextureCreationFlags = (m_TextureCreationFlags & (~flag)) |
		((((uint32)!enabled)-1) & flag);
}


//! Returns if a texture creation flag is enabled or disabled.
bool CSGPTextureManager::getTextureCreationFlag(SGP_TEXTURE_CREATION_FLAG flag) const
{
	return (m_TextureCreationFlags & flag) != 0;
}

uint32 CSGPTextureManager::getTextureCount() const
{
	uint32 count = 0;
	for( int i=0; i<m_Textures.size(); ++i )
		if( m_Textures.getUnchecked(i) != NULL ) count++;

	return count;
}

uint32 CSGPTextureManager::getFirstEmptyID()
{
	// texture Array is not full
	if( m_Textures.size() < INIT_TEXTUREARRAYSIZE )
	{
		m_Textures.add(NULL);
		return m_Textures.size()-1;
	}


	// Find first empty array slot
	for( int i=0; i<m_Textures.size(); ++i )
		if( m_Textures.getUnchecked(i) == NULL )
			return (uint32)i;

	// texture array is full, first alloc one new space, then return id
	m_Textures.add(NULL);
	return m_Textures.size()-1;
}

uint32 CSGPTextureManager::getTextureIDByName(const String& filename)
{
	uint64 HashVal = WChar_tStringHash(filename.toWideCharPointer(), filename.length());
	
	if( m_StringToTextureIDMap.contains(HashVal) )
	{
		return m_StringToTextureIDMap[HashVal];
	}
	return 0;
}

CTextureResource* CSGPTextureManager::getTextureByID(uint32 id)
{
	if( (id < (uint32)m_Textures.size()) && m_Textures[id] )
		return m_Textures[id];
	return NULL;
}


void CSGPTextureManager::unRegisterTextureByName( const String& texturename )
{
	uint64 HashVal = WChar_tStringHash(texturename.toWideCharPointer(), texturename.length());

	if( m_StringToTextureIDMap.contains(HashVal) )
	{
		uint32 TexID = m_StringToTextureIDMap[HashVal];

		if( m_Textures.getUnchecked(TexID) )
			m_Textures.getUnchecked(TexID)->decReferenceCount();
		if( m_Textures.getUnchecked(TexID)->getReferenceCount() == 0 )
		{
			m_StringToTextureIDMap.remove(HashVal);
			m_Textures.set(TexID, NULL, true);
		}
	}
}

void CSGPTextureManager::unRegisterTextureByID(uint32 id)
{
	if( m_Textures.getUnchecked(id) )
		m_Textures.getUnchecked(id)->decReferenceCount();
	if( m_Textures.getUnchecked(id)->getReferenceCount() == 0 )
	{
		m_StringToTextureIDMap.removeValue(id);
		m_Textures.set(id, NULL, true);
	}
}

void CSGPTextureManager::unRegisterTextureByNameMT( const String& texturename )
{
	uint64 HashVal = WChar_tStringHash(texturename.toWideCharPointer(), texturename.length());

	if( m_StringToTextureIDMap.contains(HashVal) )
	{
		uint32 TexID = m_StringToTextureIDMap[HashVal];

		if( m_Textures.getUnchecked(TexID) )
			m_Textures.getUnchecked(TexID)->decReferenceCount();
		if( m_Textures.getUnchecked(TexID)->getReferenceCount() == 0 )
		{
			m_pRenderDevice->GetMTResourceLoader()->addDeletingTexture(m_Textures.getUnchecked(TexID), texturename);	
		}
	}
	else
		m_pRenderDevice->GetMTResourceLoader()->addDeletingTexture( NULL, texturename );
}

void CSGPTextureManager::unRegisterTextureByIDMT(uint32 id)
{
	if( m_Textures.getUnchecked(id) )
		m_Textures.getUnchecked(id)->decReferenceCount();
	if( m_Textures.getUnchecked(id)->getReferenceCount() == 0 )
	{
		m_pRenderDevice->GetMTResourceLoader()->addDeletingTexture(m_Textures.getUnchecked(id), m_Textures.getUnchecked(id)->pSGPTexture->getName());	
	}
}


//  Without considering the reference count
void CSGPTextureManager::removeAllTextures()
{
	m_StringToTextureIDMap.clear();

	m_Textures.clear(true);
}


//! register a Texture from a file.
uint32 CSGPTextureManager::registerTexture(const String& texturename, bool bGenMipMap)
{	
	ISGPImage* pImage = NULL;
	String AbsolutePath = texturename;


	// Identify textures by their absolute filenames if possible.
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = m_WorkingDir + File::separatorString + texturename;
	}

	File textureFile(AbsolutePath);


	uint32 TexID = getTextureIDByName(texturename);
	if( TexID != 0 )
	{
		getTextureByID(TexID)->incReferenceCount();
		return TexID;
	}

	// try to load file based on file extension
	for(int i = 0; i < m_ImageLoaderArray.size(); ++i)
	{
		if(m_ImageLoaderArray[i] && m_ImageLoaderArray[i]->isALoadableFileExtension(texturename))
		{
			pImage = m_ImageLoaderArray[i]->loadImage( &textureFile );
		}
	}
	if( !pImage )
	{
		// try to load file based on what is in it
		for(int i = 0; i < m_ImageLoaderArray.size(); ++i)
		{
			if( m_ImageLoaderArray[i]->isALoadableFileFormat( &textureFile ) )
			{
				pImage = m_ImageLoaderArray[i]->loadImage( &textureFile );
				if( pImage )
					break;
			}
		}
	}

	if( !pImage )
	{
		m_pLogger->writeToLog(String("Could not load texture ")+AbsolutePath, ELL_WARNING);
		return 0;
	}

	ISGPTexture* pTex = m_pRenderDevice->createTexture(pImage, texturename, bGenMipMap);
	CTextureResource *pTextureRes = new CTextureResource();
	pTextureRes->pSGPTexture = pTex;
	pTextureRes->incReferenceCount();

	delete pImage;
	pImage = NULL;

	TexID = getFirstEmptyID();
	uint64 HashVal = WChar_tStringHash(texturename.toWideCharPointer(), texturename.length());

	m_Textures.set(TexID, pTextureRes, false);
	m_StringToTextureIDMap.set( HashVal, TexID );

	return TexID;
}

//! register a Texture from a file. (Multi-thread version)
uint32 CSGPTextureManager::registerTextureMT(const String& texturename, bool bGenMipMap)
{
	uint32 TexID = getTextureIDByName(texturename);
	if( TexID != 0 )
	{
		getTextureByID(TexID)->incReferenceCount();
		return TexID;
	}


	m_pRenderDevice->GetMTResourceLoader()->addLoadingTexture(texturename, bGenMipMap);	

	return 0;
}

ISGPImage* CSGPTextureManager::createImageFromFile(const String& texturename)
{
	ISGPImage* pImage = NULL;
	String AbsolutePath = texturename;


	// Identify textures by their absolute filenames if possible.
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = m_WorkingDir + File::separatorString + texturename;
	}

	File textureFile(AbsolutePath);

	// try to load file based on file extension
	for(int i = 0; i < m_ImageLoaderArray.size(); ++i)
	{
		if(m_ImageLoaderArray[i] && m_ImageLoaderArray[i]->isALoadableFileExtension(texturename))
		{
			pImage = m_ImageLoaderArray[i]->loadImage( &textureFile );
		}
	}
	if( !pImage )
	{
		// try to load file based on what is in it
		for(int i = 0; i < m_ImageLoaderArray.size(); ++i)
		{
			if( m_ImageLoaderArray[i]->isALoadableFileFormat( &textureFile ) )
			{
				pImage = m_ImageLoaderArray[i]->loadImage( &textureFile );
				if( pImage )
					break;
			}
		}
	}

	if( !pImage )
	{
		m_pLogger->writeToLog(String("Could not load texture From File : ") + AbsolutePath, ELL_WARNING);
		return 0;
	}

	return pImage;
}

//! register a texture from a loaded ISGPImage.
uint32 CSGPTextureManager::registerTextureFromImage(const String& name, ISGPImage* image, bool bHasMipmap)
{
	if( 0 == name.length() || !image )
		return 0;

	uint64 HashVal = WChar_tStringHash(name.toWideCharPointer(), name.length());

	if( m_StringToTextureIDMap.contains(HashVal) )
	{
		m_pLogger->writeToLog(String("Texture name has exist : ")+name, ELL_WARNING);
		return 0;
	}

	ISGPTexture* pTex = m_pRenderDevice->createTexture(image, name, bHasMipmap);
	CTextureResource *pTextureRes = new CTextureResource();
	pTextureRes->pSGPTexture = pTex;
	pTextureRes->incReferenceCount();

	uint32 TexID = getFirstEmptyID();

	m_Textures.set(TexID, pTextureRes, false);
	m_StringToTextureIDMap.set( HashVal, TexID );

	return TexID;
}

//! register an empty texture of specified size.
uint32 CSGPTextureManager::registerEmptyTexture(const SDimension2D& size,
		const String& name, SGP_PIXEL_FORMAT format)
{
	if(ISGPImage::isRenderTargetOnlyFormat(format))
	{
		m_pLogger->writeToLog(String("Could not create ISGPTexture, format only supported for render target textures."), ELL_WARNING);
		return 0;
	}

	if ( 0 == name.length() )
		return 0;

	uint64 HashVal = WChar_tStringHash(name.toWideCharPointer(), name.length());

	if( m_StringToTextureIDMap.contains(HashVal) )
	{
		m_pLogger->writeToLog(String("Texture name has exist ")+name, ELL_WARNING);
		return 0;
	}	

	ISGPImage* image = new SGPImageUncompressed(format, size);

	ISGPTexture* pTex = m_pRenderDevice->createTexture(image, name, false);
	CTextureResource *pTextureRes = new CTextureResource();
	pTextureRes->pSGPTexture = pTex;
	pTextureRes->incReferenceCount();

	delete image;
	image = NULL;

	uint32 TexID = getFirstEmptyID();

	m_Textures.set(TexID, pTextureRes, false);
	m_StringToTextureIDMap.set( HashVal, TexID );

	return TexID;
}

uint32 CSGPTextureManager::registerTextureFromResourceMT(SGPTextureRecord Record)
{
	uint64 HashVal = WChar_tStringHash(Record.TexFileName.toWideCharPointer(), Record.TexFileName.length());

	Record.pTexResource->pSGPTexture = m_pRenderDevice->createTexture(
		Record.pTexResource->pSGPImage,
		Record.TexFileName,
		Record.bGenMipMap);
	Record.pTexResource->incReferenceCount();
	for( uint32 i=0; i<Record.nRefCount; i++ )
		Record.pTexResource->incReferenceCount();

	uint32 TexID = getFirstEmptyID();

	m_Textures.set(TexID, Record.pTexResource, false);
	m_StringToTextureIDMap.set( HashVal, TexID );

	
	return TexID;
}

void CSGPTextureManager::unRegisterTextureFromResourceMT( SGPTextureRecord Record )
{
	uint32 TexID = getTextureIDByName(Record.TexFileName);
	if( TexID != 0)
	{
		m_StringToTextureIDMap.removeValue(TexID);
		m_Textures.set(TexID, NULL, true);	
	}
}
