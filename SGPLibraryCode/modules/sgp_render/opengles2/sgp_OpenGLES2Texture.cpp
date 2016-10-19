
#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

//! constructor for usual textures
COpenGLES2Texture::COpenGLES2Texture(ISGPImage* origImage, const String& name, COpenGLES2RenderDevice* renderdevice, bool bHasMipmaps)
	: ISGPTexture(name), m_Image(0), m_OrigImage(origImage),
	RenderDevice(renderdevice),
	OpenGLTextureID(0), 
	eTextureFormat(0), eTextureInternalFormat(0), eTextureType(0),	
	HasMipMaps(bHasMipmaps),
	MipMapLevels(0),
	TextureTarget(GL_TEXTURE_2D),
	TextureMagFilter(TEXTURE_FILTER_MAG_BILINEAR), TextureMinFilter(TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
{
	TextureWrap[0] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[1] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[2] = TEXTURE_ADDRESS_REPEAT;

	// Normal Texture Format
	if( !origImage->IsPVRTCImage() && !origImage->IsDDSImage() )
	{
		bPVRTCTexture = false;

		ImageSize.Width = TextureSize.Width = origImage->getDimension().Width;
		ImageSize.Height = TextureSize.Height = origImage->getDimension().Height;

		getOpenGLES2FormatFromImage(origImage, eTextureInternalFormat, eTextureFormat, eTextureType);

		glGenTextures(1, &OpenGLTextureID);

		bool bindsucceed = BindTexture2D(0);
		if( !bindsucceed || RenderDevice->testGLError() )
			Logger::getCurrentLogger()->writeToLog(String("Could not bind Texture"), ELL_ERROR);
		
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		// now get image data and upload to GPU
		void* source = origImage->lock();

		glTexImage2D(GL_TEXTURE_2D, 0, eTextureInternalFormat, origImage->getDimension().Width,
			origImage->getDimension().Height, 0, eTextureFormat, eTextureType, source);

		origImage->unlock();

		if (RenderDevice->testGLError())
			Logger::getCurrentLogger()->writeToLog(String("Could not glTexImage2D"), ELL_ERROR);

		//
		setTextureParameter();

		// Restore
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return;
	}

	COpenGLES2Config* pOpenGLES2Config = COpenGLES2Config::getInstance();

	// PVRTC Texture Format
	m_Image = static_cast<SGPImagePVRTC*>(origImage);
	jassert(m_Image);
	bPVRTCTexture = true;

	PVRTextureHeaderV3& sTextureHeader = m_Image->getTextureHeader();
	MipMapLevels = sTextureHeader.u32MIPMapCount;
	if( !pOpenGLES2Config->Force_Disable_MIPMAPPING )
	{
		if( MipMapLevels > 1 )
			HasMipMaps = true;
	}

	ImageSize.Width = TextureSize.Width = sTextureHeader.u32Width;
	ImageSize.Height = TextureSize.Height = sTextureHeader.u32Height;

	//Get the OGLES format values.
	PVRTGetOGLES2TextureFormat(sTextureHeader, eTextureInternalFormat, eTextureFormat, eTextureType);

	//Check supported texture formats.
	bool bIsPVRTCSupported = pOpenGLES2Config->bIsPVRTCSupported;
	bool bIsPVRTC2Supported = pOpenGLES2Config->bIsPVRTC2Supported;
	bool bIsBGRA8888Supported = pOpenGLES2Config->bIsBGRA8888Supported;

	bool bIsFloat16Supported = pOpenGLES2Config->bIsFloat16Supported;
	bool bIsFloat32Supported = pOpenGLES2Config->bIsFloat32Supported;
#ifndef TARGET_OS_IPHONE
	bool bIsETCSupported = pOpenGLES2Config->bIsETCSupported;
#endif

	//Just in case header and pointer for decompression.
	PVRTextureHeaderV3 sTextureHeaderDecomp;
	void* pDecompressedData = NULL;

	//Check for compressed formats
	if( (eTextureFormat == 0) && (eTextureType == 0) && (eTextureInternalFormat != 0) )
	{
		if( (eTextureInternalFormat >= GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG) && (eTextureInternalFormat <= GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG) )
		{
			//Check for PVRTCI support.
			if( bIsPVRTCSupported )
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
			}
			else
			{
				//Try to decompress the texture.
				if( m_Image->getAllowDecompress() )
				{
					//Output a warning.
					Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer warning: PVRTC not supported. Converting to RGBA8888 instead."), ELL_WARNING);

					//Modify boolean values.
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat = true;

					//Check if it's 2bpp.
					bool bIs2bppPVRTC = (eTextureInternalFormat == GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG) || (eTextureInternalFormat == GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);

					//Change texture format.
					eTextureFormat = eTextureInternalFormat = GL_RGBA;
					eTextureType = GL_UNSIGNED_BYTE;

					//Create a near-identical texture header for the decompressed header.
					sTextureHeaderDecomp = sTextureHeader;
					sTextureHeaderDecomp.u32ChannelType = ePVRTVarTypeUnsignedByteNorm;
					sTextureHeaderDecomp.u32ColourSpace = ePVRTCSpacelRGB;
					sTextureHeaderDecomp.u64PixelFormat = PVRTGENPIXELID4('r','g','b','a',8,8,8,8);

					//Allocate enough memory for the decompressed data. OGLES2, so only decompress one surface, but all faces.
					pDecompressedData = malloc( PVRTGetTextureDataSize(sTextureHeaderDecomp, PVRTEX_ALLMIPLEVELS, false, true) );

					//Check the malloc.
					if( !pDecompressedData )
					{
						Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer error: Unable to allocate memory to decompress texture"), ELL_ERROR);
						jassertfalse;
					}

					//Get the dimensions for the current MIP level.
					PVRTuint32 uiMIPWidth = sTextureHeaderDecomp.u32Width >> m_Image->getLoadFromLevel();
					PVRTuint32 uiMIPHeight = sTextureHeaderDecomp.u32Height >> m_Image->getLoadFromLevel();

					//Setup temporary variables.
					PVRTuint8* pTempDecompData = (PVRTuint8*)pDecompressedData;
					PVRTuint8* pTempCompData = (PVRTuint8*)(m_Image->getTextureData());

					if( m_Image->IsLegacyPVR() )
					{
						//Decompress all the MIP levels.
						for(PVRTuint32 uiFace=0; uiFace<sTextureHeader.u32NumFaces; ++uiFace)
						{
							for(PVRTuint32 uiMIPMap=m_Image->getLoadFromLevel(); uiMIPMap<sTextureHeader.u32MIPMapCount; ++uiMIPMap)
							{
								//Get the face offset. Varies per MIP level.
								PVRTuint32 decompressedFaceOffset = PVRTGetTextureDataSize(sTextureHeaderDecomp, uiMIPMap, false, false);
								PVRTuint32 compressedFaceOffset = PVRTGetTextureDataSize(sTextureHeader, uiMIPMap, false, false);

								//Decompress the texture data.
								PVRTDecompressPVRTC(pTempCompData, (bIs2bppPVRTC ? 1 : 0), uiMIPWidth, uiMIPHeight, pTempDecompData);

								//Move forward through the pointers.
								pTempDecompData += decompressedFaceOffset;
								pTempCompData += compressedFaceOffset;

								//Work out the current MIP dimensions.
								uiMIPWidth = PVRT_MAX(1, uiMIPWidth>>1);
								uiMIPHeight = PVRT_MAX(1, uiMIPHeight>>1);
							}

							//Reset the dims.
							uiMIPWidth = sTextureHeader.u32Width;
							uiMIPHeight = sTextureHeader.u32Height;
						}
					}
					else
					{
						//Decompress all the MIP levels.
						for(PVRTuint32 uiMIPMap=m_Image->getLoadFromLevel(); uiMIPMap<sTextureHeader.u32MIPMapCount; ++uiMIPMap)
						{
							//Get the face offset. Varies per MIP level.
							PVRTuint32 decompressedFaceOffset = PVRTGetTextureDataSize(sTextureHeaderDecomp, uiMIPMap, false, false);
							PVRTuint32 compressedFaceOffset = PVRTGetTextureDataSize(sTextureHeader, uiMIPMap, false, false);

							for(PVRTuint32 uiFace=0; uiFace<sTextureHeader.u32NumFaces; ++uiFace)
							{
								//Decompress the texture data.
								PVRTDecompressPVRTC(pTempCompData, (bIs2bppPVRTC ? 1 : 0), uiMIPWidth, uiMIPHeight, pTempDecompData);

								//Move forward through the pointers.
								pTempDecompData += decompressedFaceOffset;
								pTempCompData += compressedFaceOffset;
							}

							//Work out the current MIP dimensions.
							uiMIPWidth = PVRT_MAX(1, uiMIPWidth>>1);
							uiMIPHeight = PVRT_MAX(1, uiMIPHeight>>1);
						}
					}
				}
				else
				{
					Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer error: PVRTC not supported."), ELL_ERROR);
					jassertfalse;
				}
			}
		}
#ifndef TARGET_OS_IPHONE //TODO
		else if( (eTextureInternalFormat == GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG) || (eTextureInternalFormat == GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG) )
		{
			//Check for PVRTCI support.
			if(bIsPVRTC2Supported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
			}
			else
			{				
				Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer error: PVRTC not supported."), ELL_ERROR);
				jassertfalse;
			}
		}
		else if (eTextureInternalFormat == GL_ETC1_RGB8_OES)
		{
			if(bIsETCSupported)
			{
				bIsCompressedFormatSupported = bIsCompressedFormat = true;
			}
			else
			{
				if(m_Image->getAllowDecompress())
				{
					//Output a warning.
					Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer warning: ETC not supported. Converting to RGBA8888 instead."), ELL_WARNING);

					//Modify boolean values.
					bIsCompressedFormatSupported = false;
					bIsCompressedFormat = true;

					//Change texture format.
					eTextureFormat = eTextureInternalFormat = GL_RGBA;
					eTextureType = GL_UNSIGNED_BYTE;

					//Create a near-identical texture header for the decompressed header.
					sTextureHeaderDecomp = sTextureHeader;
					sTextureHeaderDecomp.u32ChannelType = ePVRTVarTypeUnsignedByteNorm;
					sTextureHeaderDecomp.u32ColourSpace = ePVRTCSpacelRGB;
					sTextureHeaderDecomp.u64PixelFormat = PVRTGENPIXELID4('r','g','b','a',8,8,8,8);

					//Allocate enough memory for the decompressed data. OGLES1, so only decompress one surface/face.
					pDecompressedData = malloc(PVRTGetTextureDataSize(sTextureHeaderDecomp, PVRTEX_ALLMIPLEVELS, false, true) );

					//Check the malloc.
					if (!pDecompressedData)
					{
						Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer error: Unable to allocate memory to decompress texture."), ELL_ERROR);
						jassertfalse;
					}

					//Get the dimensions for the current MIP level.
					PVRTuint32 uiMIPWidth = sTextureHeaderDecomp.u32Width >> m_Image->getLoadFromLevel();
					PVRTuint32 uiMIPHeight = sTextureHeaderDecomp.u32Height >> m_Image->getLoadFromLevel();

					//Setup temporary variables.
					PVRTuint8* pTempDecompData = (PVRTuint8*)pDecompressedData;
					PVRTuint8* pTempCompData = (PVRTuint8*)(m_Image->getTextureData());

					if(m_Image->IsLegacyPVR())
					{
						//Decompress all the MIP levels.
						for(PVRTuint32 uiFace=0; uiFace<sTextureHeader.u32NumFaces; ++uiFace)
						{

							for(PVRTuint32 uiMIPMap=m_Image->getLoadFromLevel(); uiMIPMap<sTextureHeader.u32MIPMapCount; ++uiMIPMap)
							{
								//Get the face offset. Varies per MIP level.
								PVRTuint32 decompressedFaceOffset = PVRTGetTextureDataSize(sTextureHeaderDecomp, uiMIPMap, false, false);
								PVRTuint32 compressedFaceOffset = PVRTGetTextureDataSize(sTextureHeader, uiMIPMap, false, false);

								//Decompress the texture data.
								PVRTDecompressETC(pTempCompData, uiMIPWidth, uiMIPHeight, pTempDecompData, 0);

								//Move forward through the pointers.
								pTempDecompData += decompressedFaceOffset;
								pTempCompData += compressedFaceOffset;

								//Work out the current MIP dimensions.
								uiMIPWidth = PVRT_MAX(1, uiMIPWidth>>1);
								uiMIPHeight = PVRT_MAX(1, uiMIPHeight>>1);
							}

							//Reset the dims.
							uiMIPWidth = sTextureHeader.u32Width;
							uiMIPHeight = sTextureHeader.u32Height;
						}
					}
					else
					{
						//Decompress all the MIP levels.
						for(PVRTuint32 uiMIPMap=m_Image->getLoadFromLevel(); uiMIPMap<sTextureHeader.u32MIPMapCount; ++uiMIPMap)
						{
							//Get the face offset. Varies per MIP level.
							PVRTuint32 decompressedFaceOffset = PVRTGetTextureDataSize(sTextureHeaderDecomp, uiMIPMap, false, false);
							PVRTuint32 compressedFaceOffset = PVRTGetTextureDataSize(sTextureHeader, uiMIPMap, false, false);

							for(PVRTuint32 uiFace=0; uiFace<sTextureHeader.u32NumFaces; ++uiFace)
							{
								//Decompress the texture data.
								PVRTDecompressETC(pTempCompData, uiMIPWidth, uiMIPHeight, pTempDecompData, 0);

								//Move forward through the pointers.
								pTempDecompData += decompressedFaceOffset;
								pTempCompData += compressedFaceOffset;
							}

							//Work out the current MIP dimensions.
							uiMIPWidth = PVRT_MAX(1, uiMIPWidth>>1);
							uiMIPHeight = PVRT_MAX(1, uiMIPHeight>>1);
						}
					}
				}
				else
				{
					Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer error: ETC not supported."), ELL_ERROR);
					jassertfalse;
				}
			}
		}
#endif
	}

	//Check for BGRA support.	
	if(eTextureFormat == GL_BGRA)
	{
#ifdef TARGET_OS_IPHONE
		eTextureInternalFormat = GL_RGBA;
#endif
		if( !bIsBGRA8888Supported )
		{
#ifdef TARGET_OS_IPHONE
			Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_APPLE_texture_format_BGRA8888 is unsupported."), ELL_ERROR);
#else
			Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported."), ELL_ERROR);
#endif
			jassertfalse;
		}
	}

	//Check for floating point textures
	if(eTextureType == GL_HALF_FLOAT_OES)
	{
		if( !bIsFloat16Supported )
		{
			Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: Unable to load GL_HALF_FLOAT_OES texture as extension GL_OES_texture_half_float is unsupported."), ELL_ERROR);
		}
	}
	if (eTextureType==GL_FLOAT)
	{
		if(!bIsFloat32Supported)
		{
			Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: Unable to load GL_FLOAT texture as extension GL_OES_texture_float is unsupported."), ELL_ERROR);
		}
	}

	//Deal with unsupported texture formats
	if(eTextureInternalFormat == 0)
	{
		Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: pixel type not supported."), ELL_ERROR);
		jassertfalse;
	}

	glGenTextures(1, &OpenGLTextureID);


	//PVR files are never row aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	// get correct opengl color data values
	TextureTarget = GL_TEXTURE_2D;

	if( sTextureHeader.u32NumFaces > 1 )
	{
		TextureTarget = GL_TEXTURE_CUBE_MAP;
	}

	//Check if this is a texture array.
	if( sTextureHeader.u32NumSurfaces > 1 )
	{
		//Not supported in OpenGLES 2.0
		Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: Texture arrays are not available in OGLES2.0."), ELL_ERROR);
		jassertfalse;
	}


	bool bindsucceed = BindTexture2D(0);
	if( !bindsucceed || RenderDevice->testGLError() )
		Logger::getCurrentLogger()->writeToLog(String("Could not bind Texture"), ELL_ERROR);


	//Initialise the current MIP size.
	PVRTuint32 uiCurrentMIPSize = 0;

	//Loop through the faces
	//Check if this is a cube map.
	if( sTextureHeader.u32NumFaces > 1 )
	{
		TextureTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	}

	//Initialise the width/height
	PVRTuint32 u32MIPWidth = sTextureHeader.u32Width;
	PVRTuint32 u32MIPHeight = sTextureHeader.u32Height;

	//Temporary data to save on if statements within the load loops.
	PVRTuint8* pTempData = NULL;
	PVRTextureHeaderV3 *psTempHeader = NULL;
	if( bIsCompressedFormat && !bIsCompressedFormatSupported )
	{
		pTempData = (PVRTuint8*)pDecompressedData;
		psTempHeader = &sTextureHeaderDecomp;
	}
	else
	{
		pTempData = m_Image->getTextureData();
		psTempHeader = &sTextureHeader;
	}

	//Loop through all MIP levels.
	if( m_Image->IsLegacyPVR() )
	{
		//Temporary texture target.
		GLint eTextureTarget = TextureTarget;

		PVRTuint32 MIPMapNum = (pOpenGLES2Config->Force_Disable_MIPMAPPING ? 1 : psTempHeader->u32MIPMapCount);
		
		//Loop through all the faces.
		for(PVRTuint32 uiFace=0; uiFace<psTempHeader->u32NumFaces; ++uiFace)
		{
			//Loop through all the mip levels.
			for(PVRTuint32 uiMIPLevel=0; uiMIPLevel<MIPMapNum; ++uiMIPLevel)
			{
				//Get the current MIP size.
				uiCurrentMIPSize = PVRTGetTextureDataSize(*psTempHeader, uiMIPLevel, false, false);

				if( uiMIPLevel >= m_Image->getLoadFromLevel() )
				{
					//Upload the texture
					if( bIsCompressedFormat && bIsCompressedFormatSupported )
					{
						glCompressedTexImage2D(eTextureTarget, uiMIPLevel-m_Image->getLoadFromLevel(), eTextureInternalFormat, u32MIPWidth, u32MIPHeight, 0, uiCurrentMIPSize, pTempData);
					}
					else
					{
						glTexImage2D(eTextureTarget, uiMIPLevel-m_Image->getLoadFromLevel(), eTextureInternalFormat, u32MIPWidth, u32MIPHeight, 0, eTextureFormat, eTextureType, pTempData);
					}
				}
				pTempData += uiCurrentMIPSize;

				//Reduce the MIP Size.
				u32MIPWidth = PVRT_MAX(1, u32MIPWidth>>1);
				u32MIPHeight = PVRT_MAX(1, u32MIPHeight>>1);
			}

			//Increase the texture target.
			eTextureTarget++;

			//Reset the current MIP dimensions.
			u32MIPWidth = psTempHeader->u32Width;
			u32MIPHeight = psTempHeader->u32Height;

			//Error check
			if( RenderDevice->testGLError() )
			{
				FREE(pDecompressedData);
				Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: glTexImage2D() failed."), ELL_ERROR);
				jassertfalse;
			}
		}
	}
	else
	{
		PVRTuint32 MIPMapNum = (pOpenGLES2Config->Force_Disable_MIPMAPPING ? 1 : psTempHeader->u32MIPMapCount);

		for(PVRTuint32 uiMIPLevel=0; uiMIPLevel<MIPMapNum; ++uiMIPLevel)
		{
			//Get the current MIP size.
			uiCurrentMIPSize = PVRTGetTextureDataSize(*psTempHeader, uiMIPLevel, false, false);

			GLint eTextureTarget = TextureTarget;

			for(PVRTuint32 uiFace=0; uiFace<psTempHeader->u32NumFaces; ++uiFace)
			{
				if(uiMIPLevel >= m_Image->getLoadFromLevel())
				{
					//Upload the texture
					if( bIsCompressedFormat && bIsCompressedFormatSupported )
					{
						glCompressedTexImage2D(eTextureTarget, uiMIPLevel-m_Image->getLoadFromLevel(), eTextureInternalFormat, u32MIPWidth, u32MIPHeight, 0, uiCurrentMIPSize, pTempData);
					}
					else
					{
						glTexImage2D(eTextureTarget, uiMIPLevel-m_Image->getLoadFromLevel(), eTextureInternalFormat, u32MIPWidth, u32MIPHeight, 0, eTextureFormat, eTextureType, pTempData);
					}
				}
				pTempData += uiCurrentMIPSize;
				eTextureTarget++;
			}

			//Reduce the MIP Size.
			u32MIPWidth = PVRT_MAX(1, u32MIPWidth>>1);
			u32MIPHeight = PVRT_MAX(1, u32MIPHeight>>1);

			//Error check
			if( RenderDevice->testGLError() )
			{
				FREE(pDecompressedData);
				Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: glTexImage2D() failed."), ELL_ERROR);
				jassertfalse;
			}
		}
	}

	FREE(pDecompressedData);

	if( TextureTarget != GL_TEXTURE_2D )
	{
		TextureTarget = GL_TEXTURE_CUBE_MAP;
	}

	// Error check
	if( RenderDevice->testGLError() )
	{
		Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: glTexImage2D() failed."), ELL_ERROR);
		jassertfalse;
	}

	// setting Mipmap
	setTextureParameter();

	// Restore
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}


//! constructor for basic setup (only for derived classes)
COpenGLES2Texture::COpenGLES2Texture(const String& name, COpenGLES2RenderDevice* renderdevice)
	: ISGPTexture(name), 
	RenderDevice(renderdevice), m_Image(0), m_OrigImage(0),
	OpenGLTextureID(0),
	eTextureFormat(0), eTextureInternalFormat(0), eTextureType(0),
	MipMapLevels(0), HasMipMaps(false), bPVRTCTexture(false),
	TextureTarget(GL_TEXTURE_2D),
	TextureMagFilter(TEXTURE_FILTER_MAG_BILINEAR), TextureMinFilter(TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
{
	TextureWrap[0] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[1] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[2] = TEXTURE_ADDRESS_REPEAT;
}


//! destructor
COpenGLES2Texture::~COpenGLES2Texture()
{
	if( OpenGLTextureID )
		glDeleteTextures(1, &OpenGLTextureID);
	if( m_Image )
		m_Image = NULL;
	if( m_OrigImage )
		m_OrigImage = NULL;
}


//! set texture parameters into an OpenGLES2 texture.
void COpenGLES2Texture::setTextureParameter()
{
	COpenGLES2Config* pOpenGLES2Config = COpenGLES2Config::getInstance();

	if( pOpenGLES2Config->Force_Disable_MIPMAPPING )
		HasMipMaps = false;
	if( HasMipMaps && (MipMapLevels <= 1) )
	{
		//	IMPORTANT NOTE:
		//	if OpenGL ES 2.0, use glGenerateMipmap
		regenerateMipMapLevels();

		if( RenderDevice->testGLError() )
		{
			Logger::getCurrentLogger()->writeToLog(String("Could not generate Mipmap"), ELL_ERROR);
			HasMipMaps = false;
		}
		else
			HasMipMaps = true;
	}

	setFiltering( TextureMagFilter, TextureMinFilter );

	setWrapMode( TextureWrap[0], TextureWrap[1] );
}

GLint COpenGLES2Texture::getOpenGLFormatFromTextureWrapMode(SGP_TEXTURE_ADDRESSING wrap)
{
	switch(wrap)
	{
	case TEXTURE_ADDRESS_CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case TEXTURE_ADDRESS_CLAMP_TO_BORDER:
		Logger::getCurrentLogger()->writeToLog(String("OpenGL ES2.0 Could not Support CLAMP_TO_BORDER"), ELL_ERROR);
		return GL_CLAMP_TO_EDGE;
	case TEXTURE_ADDRESS_MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
	case TEXTURE_ADDRESS_REPEAT:
		return GL_REPEAT;
	}
	return GL_REPEAT;
}

void COpenGLES2Texture::getOpenGLES2FormatFromImage(ISGPImage* pImage, GLenum& internalformat, GLenum& format, GLenum& type)
{
	if( bPVRTCTexture )
		return;

	switch(pImage->getColorFormat())
	{
		case SGPPF_A8R8G8B8:
			internalformat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		break;
		case SGPPF_A4R4G4B4:
			internalformat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
		case SGPPF_A1R5G5B5:
			internalformat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
		case SGPPF_R5G6B5:
			internalformat = GL_RGB;
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_6_5;
		break;


		default:
			Logger::getCurrentLogger()->writeToLog(String("Image pixel format do not support for OpenGL Es 2.0!"), ELL_ERROR);
			jassertfalse;
		break;
	}


	return;
}


void COpenGLES2Texture::setFiltering(SGP_TEXTURE_FILTERING Magnification, SGP_TEXTURE_FILTERING Minification)
{
	//Set Minification and Magnification filters according to whether MIP maps are present.
	if( (eTextureType == GL_FLOAT) || (eTextureType == GL_HALF_FLOAT_OES) )
	{
		if( MipMapLevels == 1 )
		{	
			// Texture filter modes are limited to these for float textures
			Minification = TEXTURE_FILTER_MIN_NEAREST;
			Magnification = TEXTURE_FILTER_MAG_NEAREST;
		}
		else
		{
			Minification = TEXTURE_FILTER_MIN_NEAREST_MIPMAP;
			Magnification = TEXTURE_FILTER_MAG_NEAREST;
		}
	}

	// Set magnification filter
	if( Magnification == TEXTURE_FILTER_MAG_NEAREST )
		glTexParameteri(TextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if( Magnification == TEXTURE_FILTER_MAG_BILINEAR )
		glTexParameteri(TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	TextureMagFilter = Magnification;
	TextureMinFilter = Minification;

	// Set minification filter
	if( !HasMipMaps )
	{
		if( Minification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP )
		{
			glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			TextureMinFilter = TEXTURE_FILTER_MIN_NEAREST;
			return;
		}
		else if( (Minification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP) ||
				 (Minification == TEXTURE_FILTER_MIN_TRILINEAR) )
		{
			glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			TextureMinFilter = TEXTURE_FILTER_MIN_BILINEAR;
			return;
		}
	}

	if( Minification == TEXTURE_FILTER_MIN_NEAREST )
		glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if( Minification == TEXTURE_FILTER_MIN_BILINEAR )
		glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if( Minification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP )
		glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if( Minification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP )
		glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if( Minification == TEXTURE_FILTER_MIN_TRILINEAR )
		glTexParameteri(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

}

void COpenGLES2Texture::setWrapMode(SGP_TEXTURE_ADDRESSING s, SGP_TEXTURE_ADDRESSING t, SGP_TEXTURE_ADDRESSING r)
{
	if( bPVRTCTexture )
	{
		if(	(TextureSize.Width & (TextureSize.Width - 1)) | (TextureSize.Height & (TextureSize.Height - 1)) )
		{
			/*
				NPOT textures requires the wrap mode to be set explicitly to
				GL_CLAMP_TO_EDGE or the texture will be inconsistent.
			*/
			s = TEXTURE_ADDRESS_CLAMP_TO_EDGE;
			t = TEXTURE_ADDRESS_CLAMP_TO_EDGE;
		}
	}

	glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, getOpenGLFormatFromTextureWrapMode(s));
	glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, getOpenGLFormatFromTextureWrapMode(t));

	TextureWrap[0] = s;
	TextureWrap[1] = t;
	TextureWrap[2] = r;

	//Error check
	if( RenderDevice->testGLError() )
	{
		Logger::getCurrentLogger()->writeToLog(String("PVRTTextureLoadFromPointer failed: glTexParameter() failed"), ELL_ERROR);
		jassertfalse;
	}
}





bool COpenGLES2Texture::BindTexture2D(int iTextureUnit )
{
	if(iTextureUnit >= COpenGLES2Config::getInstance()->MaxTextureUnits)
		return false;

	glActiveTexture(GL_TEXTURE0 + iTextureUnit);

	glBindTexture(GL_TEXTURE_2D, OpenGLTextureID);
	
	return true;
}



bool COpenGLES2Texture::BindTextureCubeMap(int iTextureUnit )
{
	if( iTextureUnit >= COpenGLES2Config::getInstance()->MaxTextureUnits )
		return false;

	glActiveTexture(GL_TEXTURE0 + iTextureUnit);


	glBindTexture(GL_TEXTURE_CUBE_MAP, OpenGLTextureID);
	
	return true;
}

bool COpenGLES2Texture::unBindTexture2D(int iTextureUnit)
{
	if(iTextureUnit >= COpenGLES2Config::getInstance()->MaxTextureUnits)
		return false;
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}


bool COpenGLES2Texture::unBindTextureCubeMap(int iTextureUnit)
{
	if( iTextureUnit >= COpenGLES2Config::getInstance()->MaxTextureUnits )
		return false;

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	return true;
}

void COpenGLES2Texture::regenerateMipMapLevels()
{
	if( !HasMipMaps || !m_OrigImage )
		return;

	uint32 width = 0;
	uint32 height = 0;

	if( m_OrigImage->IsPVRTCImage() && m_Image )
	{
		width = m_Image->getTextureHeader().u32Width;
		height = m_Image->getTextureHeader().u32Height;
	}
	else
	{
		width = m_OrigImage->getDimension().Width;
		height = m_OrigImage->getDimension().Height;
	}

	if( (width==1) && (height==1) )
		return;


	if( MipMapLevels == 0 )
	{
		glGenerateMipmap( TextureTarget );
		

		uint32 i=0;
		do
		{
			if(width>1)
				width>>=1;
			if(height>1)
				height>>=1;
			++i;
		}
		while (width!=1 || height!=1);
		MipMapLevels = i;	
	}
}

#if SGP_MSVC
 #pragma warning (pop)
#endif