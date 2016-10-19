
#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif

//! constructor for usual textures
COpenGLTexture::COpenGLTexture(ISGPImage* origImage, const String& name, COpenGLRenderDevice* renderdevice, bool bHasMipmaps)
	: ISGPTexture(name), 
	m_ColorFormat(SGPPF_A8R8G8B8), RenderDevice(renderdevice), m_Image(0), /*m_MipImage(0),*/
	OpenGLTextureID(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE), HasMipMaps(bHasMipmaps),
	MipMapLevels(0),
	IsRenderTarget(false), 	/*ReadOnlyLock(false),*/
	TextureTarget(GL_TEXTURE_2D),
	TextureMagFilter(TEXTURE_FILTER_MAG_BILINEAR), TextureMinFilter(TEXTURE_FILTER_MIN_BILINEAR),
	TextureBorderColor(0,0,0,0.0f),
	TextureAnisotropicFilter(1),
	TextureLODBias(0.0f),
	TextureMaxMipLevel(1000)
{
	TextureWrap[0] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[1] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[2] = TEXTURE_ADDRESS_REPEAT;

	if( origImage->IsDDSImage() )
	{
		updateDDSTexture(origImage);
		return;
	}

	getImageValues(origImage);

	glGenTextures(1, &OpenGLTextureID);

	if( (ImageSize.Width == TextureSize.Width) && (ImageSize.Height == TextureSize.Height) )
	{
		m_Image = RenderDevice->GetTextureManager()->createImage(m_ColorFormat, ImageSize);
		origImage->copyTo(m_Image, SDimension2D(0,0));
	}
	else
	{
		m_Image = RenderDevice->GetTextureManager()->createImage(m_ColorFormat, TextureSize);
		// scale texture
		origImage->copyToScaling(m_Image);
	}

	uploadTexture();
}


//! constructor for basic setup (only for derived classes)
COpenGLTexture::COpenGLTexture(const String& name, COpenGLRenderDevice* renderdevice)
	: ISGPTexture(name), 
	m_ColorFormat(SGPPF_A8R8G8B8), RenderDevice(renderdevice), m_Image(0), /*m_MipImage(0),*/
	OpenGLTextureID(0), InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
	PixelType(GL_UNSIGNED_BYTE), MipMapLevels(0), HasMipMaps(false),
	IsRenderTarget(false),
	/*ReadOnlyLock(false), */TextureTarget(GL_TEXTURE_2D),
	TextureMagFilter(TEXTURE_FILTER_MAG_BILINEAR), TextureMinFilter(TEXTURE_FILTER_MIN_BILINEAR),
	TextureBorderColor(0,0,0,0.0f),
	TextureAnisotropicFilter(1),
	TextureLODBias(0.0f),
	TextureMaxMipLevel(1000)
{
	TextureWrap[0] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[1] = TEXTURE_ADDRESS_REPEAT;
	TextureWrap[2] = TEXTURE_ADDRESS_REPEAT;
}


//! destructor
COpenGLTexture::~COpenGLTexture()
{
	if( OpenGLTextureID )
		glDeleteTextures(1, &OpenGLTextureID);
	if( m_Image )
	{
		delete m_Image;
		m_Image = NULL;
	}
}

// constructor for DDS textures
void COpenGLTexture::updateDDSTexture(ISGPImage* surface)
{
	COpenGLConfig* pOpenGLConfig = COpenGLConfig::getInstance();
	SGPImageDDS *pSurface = static_cast<SGPImageDDS*>(surface);
	InternalFormat = pSurface->getInternalFormat();
	PixelFormat = pSurface->getExternalFormat();
	PixelType = pSurface->getPixelType();	

	ImageSize = surface->getDimension();
	TextureSize = ImageSize.getOptimalSize( !RenderDevice->queryDriverFeature(SGPVDF_TEXTURE_NPOT) );
	m_ColorFormat = getColorFormatFromInternalFormat(InternalFormat);

	glGenTextures(1, &OpenGLTextureID);

	// Loading a volume texture: 3D Texture
	if( pSurface->isVolume() )
	{
		TextureTarget = GL_TEXTURE_3D;
		bool bindsucceed = BindTexture3D(0);
		if( !bindsucceed || RenderDevice->testGLError() )
			Logger::getCurrentLogger()->writeToLog(String("Could not bind Texture"), ELL_ERROR);

		glTexImage3D(GL_TEXTURE_3D, 0, InternalFormat, 
			pSurface->getMipmapSize(0).Width, 
			pSurface->getMipmapSize(0).Height,
			pSurface->getMipmapDepth(0), 0, PixelFormat, 
			pSurface->getPixelType(), pSurface->getMipmapData(0));
		
		HasMipMaps = false;
		if( !pOpenGLConfig->Force_Disable_MIPMAPPING )
		{
			for(int i = 1; i < pSurface->getNumberOfMipmaps(); i++)
			{	
				HasMipMaps = true;
				glTexImage3D(GL_TEXTURE_3D, i, InternalFormat, 
					pSurface->getMipmapSize(i).Width, 
					pSurface->getMipmapSize(i).Height, 
					pSurface->getMipmapDepth(i), 0, PixelFormat, 
					pSurface->getPixelType(), pSurface->getMipmapData(i));
			}
			MipMapLevels = pSurface->getNumberOfMipmaps();
		}
		//unBindTexture3D(0);
	}
	else if( pSurface->isCubemap() )
	{
		TextureTarget = GL_TEXTURE_CUBE_MAP_ARB;
		//uncompressed cubemap texture
		bool bindsucceed = BindTextureCubeMap(0);
		if( !bindsucceed || RenderDevice->testGLError() )
			Logger::getCurrentLogger()->writeToLog(String("Could not bind Texture"), ELL_ERROR);
		
		uint32 MipMapNum = pSurface->getNumberOfMipmaps() / 6;

		for(int n = 0; n < 6; n++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+n, 0, InternalFormat,
				pSurface->getMipmapSize(0).Width, 
				pSurface->getMipmapSize(0).Height,
				0, PixelFormat, pSurface->getPixelType(), 
				pSurface->getCubemap(n));

			HasMipMaps = false;
			if( !pOpenGLConfig->Force_Disable_MIPMAPPING )
			{
				for(uint32 i = 1; i < MipMapNum; i++)
				{
					HasMipMaps = true;
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+n, i, InternalFormat, 
						pSurface->getMipmapSize(n*MipMapNum+i).Width, 
						pSurface->getMipmapSize(n*MipMapNum+i).Height,
						0, PixelFormat, pSurface->getPixelType(),
						pSurface->getMipmapData(n*MipMapNum+i));
				}
				MipMapLevels = MipMapNum;
			}
		}
		//unBindTextureCubeMap(0);
	}
	else	// 2D texture
	{
		TextureTarget = GL_TEXTURE_2D;
		bool bindsucceed = BindTexture2D(0);
		if( !bindsucceed || RenderDevice->testGLError() )
			Logger::getCurrentLogger()->writeToLog(String("Could not bind Texture"), ELL_ERROR);

		if( pSurface->isCompressed() )
		{
			RenderDevice->extGlCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, 
				pSurface->getMipmapSize(0).Width,
				pSurface->getMipmapSize(0).Height, 0, 
				pSurface->getMipmapDataBytes(0), 
				pSurface->getMipmapData(0));

			HasMipMaps = false;
			if( !pOpenGLConfig->Force_Disable_MIPMAPPING )
			{
				for (int i = 1; i < pSurface->getNumberOfMipmaps(); i++)
				{
					HasMipMaps = true;
					RenderDevice->extGlCompressedTexImage2D(GL_TEXTURE_2D, i, InternalFormat, 
						pSurface->getMipmapSize(i).Width,
						pSurface->getMipmapSize(i).Height, 0, 
						pSurface->getMipmapDataBytes(i), 
						pSurface->getMipmapData(i));
				}
				MipMapLevels = pSurface->getNumberOfMipmaps();
			}
		}
		else
		{
			if( pSurface->isSwap() )
				glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_TRUE );

			glPixelStorei( GL_UNPACK_ROW_LENGTH, pSurface->getMipmapSize(0).Width );
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, 
				pSurface->getMipmapSize(0).Width, 
				pSurface->getMipmapSize(0).Height, 0,
				PixelFormat, pSurface->getPixelType(), 
				pSurface->getMipmapData(0));

			HasMipMaps = false;
			if( !pOpenGLConfig->Force_Disable_MIPMAPPING )
			{
				for(int i = 1; i < pSurface->getNumberOfMipmaps(); i++)
				{
					HasMipMaps = true;
					glPixelStorei( GL_UNPACK_ROW_LENGTH, pSurface->getMipmapSize(i).Width );
					glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, 
						pSurface->getMipmapSize(i).Width,
						pSurface->getMipmapSize(i).Height, 0,
						PixelFormat, pSurface->getPixelType(), 
						pSurface->getMipmapData(i));
				}
				MipMapLevels = pSurface->getNumberOfMipmaps();
			}
			glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
			glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
		}

		if( HasMipMaps )
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, pSurface->getNumberOfMipmaps()-1 );
		
		//unBindTexture2D(0);
	}

	setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);
	//if( HasMipMaps )
	//	MaxMinificationLevel = TEXTURE_FILTER_MIN_TRILINEAR;
}






SGP_PIXEL_FORMAT COpenGLTexture::getColorFormatFromInternalFormat(GLint InFormat)
{
	SGP_PIXEL_FORMAT destFormat = SGPPF_A8R8G8B8;
	if( InFormat == GL_RGB8 )
		destFormat = SGPPF_R8G8B8;
	else if( InFormat == GL_RGB5_A1 )
		destFormat = SGPPF_A1R5G5B5;
	else if( InFormat == GL_RGB5 )
		destFormat = SGPPF_R5G6B5;
	return destFormat;
}

//! Choose best matching color format, based on texture creation flags
SGP_PIXEL_FORMAT COpenGLTexture::getBestColorFormat(SGP_PIXEL_FORMAT format)
{
	SGP_PIXEL_FORMAT destFormat = SGPPF_A8R8G8B8;
	switch(format)
	{
		case SGPPF_A4R4G4B4:
			destFormat = SGPPF_A4R4G4B4;
		break;
		case SGPPF_A1R5G5B5:
			destFormat = SGPPF_A1R5G5B5;
		break;
		case SGPPF_R5G6B5:
			destFormat = SGPPF_A1R5G5B5;
		break;

		default:
		break;
	}

	if( RenderDevice->GetTextureManager()->getTextureCreationFlag(SGPTCF_NO_ALPHA_CHANNEL) )
	{
		switch(destFormat)
		{
			case SGPPF_A1R5G5B5:
				destFormat = SGPPF_R5G6B5;
			break;
			case SGPPF_A8R8G8B8:
				destFormat = SGPPF_R8G8B8;
			break;
			case SGPPF_A4R4G4B4:
				destFormat = SGPPF_R5G6B5;
			break;
			default:
			break;
		}
	}
	return destFormat;
}

//! Get opengl values for the GPU texture storage
GLint COpenGLTexture::getOpenGLFormatAndParametersFromColorFormat(
	SGP_PIXEL_FORMAT format,
	GLint& filtering,
	GLenum& colorformat,
	GLenum& type)
{
	// default
	filtering = GL_LINEAR;
	colorformat = GL_RGBA;
	type = GL_UNSIGNED_BYTE;
	GLenum internalformat = GL_RGBA;

	switch(format)
	{
		case SGPPF_A1R5G5B5:
			colorformat = GL_BGRA_EXT;
			type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
			internalformat = GL_RGBA;
			break;
		case SGPPF_R5G6B5:
			colorformat = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_6_5;
			internalformat = GL_RGB;
			break;
		case SGPPF_R8G8B8:
			colorformat = GL_BGR;
			type = GL_UNSIGNED_BYTE;
			internalformat = GL_RGB;
			break;
		case SGPPF_A8R8G8B8:
			colorformat = GL_BGRA_EXT;
			if( RenderDevice->Version > 110 )
				type = GL_UNSIGNED_INT_8_8_8_8_REV;
			internalformat = GL_RGBA;
			break;
		case SGPPF_A4R4G4B4:
			colorformat = GL_BGRA_EXT;
			if( RenderDevice->Version > 110 )
				type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
			internalformat = GL_RGBA;
			break;
		// Floating Point texture formats. Thanks to Patryk "Nadro" Nadrowski.
		case SGPPF_R16F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RED;
			type = GL_FLOAT;

			internalformat =  GL_R16F;
#else
			m_ColorFormat = SGPPF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case SGPPF_G16R16F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RG;
			type = GL_FLOAT;

			internalformat =  GL_RG16F;
#else
			m_ColorFormat = SGPPF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case SGPPF_A16B16G16R16F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RGBA;
			type = GL_FLOAT;

			internalformat =  GL_RGBA16F_ARB;
#else
			m_ColorFormat = SGPPF_A8R8G8B8;
			internalformat =  GL_RGBA8;
#endif
		}
			break;
		case SGPPF_R32F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RED;
			type = GL_FLOAT;

			internalformat =  GL_R32F;
#else
			m_ColorFormat = SGPPF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case SGPPF_G32R32F:
		{
#ifdef GL_ARB_texture_rg
			filtering = GL_NEAREST;
			colorformat = GL_RG;
			type = GL_FLOAT;

			internalformat =  GL_RG32F;
#else
			m_ColorFormat = SGPPF_A8R8G8B8;
			internalformat =  GL_RGB8;
#endif
		}
			break;
		case SGPPF_A32B32G32R32F:
		{
#ifdef GL_ARB_texture_float
			filtering = GL_NEAREST;
			colorformat = GL_RGBA;
			type = GL_FLOAT;

			internalformat =  GL_RGBA32F_ARB;
#else
			m_ColorFormat = SGPPF_A8R8G8B8;
			internalformat =  GL_RGBA8;
#endif
		}
			break;
		default:
		{
			Logger::getCurrentLogger()->writeToLog(String("Unsupported texture format"), ELL_ERROR);
			internalformat =  GL_RGBA8;
		}
	}
#if defined(GL_ARB_framebuffer_sRGB) || defined(GL_EXT_framebuffer_sRGB)
	COpenGLConfig* pOpenGLConfig = COpenGLConfig::getInstance();
	if( pOpenGLConfig->HandleSRGB )
	{
		if (internalformat == GL_RGBA)
			internalformat = GL_SRGB_ALPHA_EXT;
		else if (internalformat == GL_RGB)
			internalformat = GL_SRGB_EXT;
	}
#endif
	return internalformat;
}

// prepare values ImageSize, TextureSize, and ColorFormat based on image
void COpenGLTexture::getImageValues(ISGPImage* image)
{
	COpenGLConfig* pOpenGLConfig = COpenGLConfig::getInstance();
	if (!image)
	{
		Logger::getCurrentLogger()->writeToLog(String("No image for OpenGL texture."), ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		Logger::getCurrentLogger()->writeToLog(String("Invalid size of image for OpenGL Texture."), ELL_ERROR);
		return;
	}

	const float ratio = (float)ImageSize.Width/(float)ImageSize.Height;
	if ( (ImageSize.Width > pOpenGLConfig->MaxTextureSize) && (ratio >= 1.0f) )
	{
		ImageSize.Width = pOpenGLConfig->MaxTextureSize;
		ImageSize.Height = (uint32)(pOpenGLConfig->MaxTextureSize/ratio);
	}
	else if (ImageSize.Height > pOpenGLConfig->MaxTextureSize)
	{
		ImageSize.Height = pOpenGLConfig->MaxTextureSize;
		ImageSize.Width = (uint32)(pOpenGLConfig->MaxTextureSize*ratio);
	}

	TextureSize = ImageSize.getOptimalSize( !RenderDevice->queryDriverFeature(SGPVDF_TEXTURE_NPOT) );

	m_ColorFormat = getBestColorFormat(image->getColorFormat());
}

//! copies the the texture into an open gl texture.
void COpenGLTexture::uploadTexture()
{
	COpenGLConfig* pOpenGLConfig = COpenGLConfig::getInstance();

	// check which image needs to be uploaded
	ISGPImage* image = m_Image;
	if( !image )
	{
		Logger::getCurrentLogger()->writeToLog(String("No image for OpenGL texture to upload"), ELL_ERROR);
		return;
	}

	// get correct opengl color data values
	
	GLint filtering;
	InternalFormat = getOpenGLFormatAndParametersFromColorFormat(m_ColorFormat, filtering, PixelFormat, PixelType);
	

	bool bindsucceed = BindTexture2D(0);
	if( !bindsucceed || RenderDevice->testGLError() )
		Logger::getCurrentLogger()->writeToLog(String("Could not bind Texture"), ELL_ERROR);


	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// now get image data and upload to GPU
	void* source = image->lock();

	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, image->getDimension().Width,
		image->getDimension().Height, 0, PixelFormat, PixelType, source);

	image->unlock();

	if (RenderDevice->testGLError())
		Logger::getCurrentLogger()->writeToLog(String("Could not glTexImage2D"), ELL_ERROR);


	if( pOpenGLConfig->Force_Disable_MIPMAPPING )
		HasMipMaps = false;
	if( HasMipMaps )
	{
/*	IMPORTANT NOTE:
		if OpenGL >= 3.0, use glGenerateMipmap
		else if OpenGL >= 1.4, use glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		else video card is pretty old at this point, fall back to gluBuild2DMipmaps?
		else cut off support
*/
		if( RenderDevice->queryDriverFeature(SGPVDF_MIP_MAP_GEN_HW) )
		{
			RenderDevice->extGlGenerateMipmap(GL_TEXTURE_2D);

			{
				uint32 width = image->getDimension().Width;
				uint32 height = image->getDimension().Height;
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
		else
			regenerateMipMapLevels();
	}

	setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);

	if (RenderDevice->testGLError())
		Logger::getCurrentLogger()->writeToLog(String("Could not generate Mipmap"), ELL_ERROR);
	
	// Restore
	glPixelStorei(GL_PACK_ALIGNMENT, 4);

}

GLint COpenGLTexture::getOpenGLFormatFromTextureWrapMode(SGP_TEXTURE_ADDRESSING wrap)
{
	switch(wrap)
	{
	case TEXTURE_ADDRESS_CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case TEXTURE_ADDRESS_CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;
	case TEXTURE_ADDRESS_MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
	case TEXTURE_ADDRESS_REPEAT:
		return GL_REPEAT;
	}
	return GL_REPEAT;
}

//! getting access the pixels from texture.
bool COpenGLTexture::getMipmapData(void* pixels, uint32 mipmapLevel, SGP_TEXTURE_TARGET target)
{
	bool bSucceed = true;

	if( mipmapLevel > getMipMapLevels() )
		return false;

	// we need to keep the correct texture bound later on
	GLint tmpTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmpTexture);
	

	// download GPU data as ARGB8 to pixels
	switch(target)
	{
	case SGPTT_TEXTURE_2D:
		glBindTexture(GL_TEXTURE_2D, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_2D, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_3D:
		glBindTexture(GL_TEXTURE_3D, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_3D, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_CUBE_MAP_POSITIVE_X:
		glBindTexture(GL_TEXTURE_CUBE_MAP, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_X:
		glBindTexture(GL_TEXTURE_CUBE_MAP, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_CUBE_MAP_POSITIVE_Y:
		glBindTexture(GL_TEXTURE_CUBE_MAP, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		glBindTexture(GL_TEXTURE_CUBE_MAP, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_CUBE_MAP_POSITIVE_Z:
		glBindTexture(GL_TEXTURE_CUBE_MAP, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	case SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		glBindTexture(GL_TEXTURE_CUBE_MAP, getOpenGLTextureID());
		glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
		break;
	default:
		bSucceed = false;
		break;
	}

	if( RenderDevice->testGLError() )
		bSucceed = false;

	//reset old bound texture
	glBindTexture(GL_TEXTURE_2D, tmpTexture);

	return bSucceed;
}


//! lock function
void* COpenGLTexture::lock(SGP_TEXTURE_LOCK_MODE mode, uint32 mipmapLevel)
{
	return NULL;
//	// store info about which image is locked
//	ISGPImage* image = (mipmapLevel==0) ? m_Image : m_MipImage;
//	ReadOnlyLock |= (mode==SGPTLM_READ_ONLY);
//	MipLevelStored = mipmapLevel;
//	if (!ReadOnlyLock && mipmapLevel)
//	{
////#ifdef GL_SGIS_generate_mipmap
////		if (RenderDevice->queryDriverFeature(SGPVDF_MIP_MAP_AUTO_UPDATE))
////		{
////			// do not automatically generate and update mipmaps
////			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
////		}
////#endif
//		AutomaticMipmapUpdate=false;
//	}
//
//	// if data not available or might have changed on GPU download it
//	if (!image || IsRenderTarget)
//	{
//		// prepare the data storage if necessary
//		if (!image)
//		{
//			if (mipmapLevel)
//			{
//				uint32 i=0;
//				uint32 width = TextureSize.Width;
//				uint32 height = TextureSize.Height;
//				do
//				{
//					if (width>1)
//						width>>=1;
//					if (height>1)
//						height>>=1;
//					++i;
//				}
//				while (i != mipmapLevel);
//				m_MipImage = image = RenderDevice->GetTextureManager()->createImage(SGPPF_A8R8G8B8, SDimension2D(width,height));
//			}
//			else
//				m_Image = image = RenderDevice->GetTextureManager()->createImage(SGPPF_A8R8G8B8, ImageSize);
//			m_ColorFormat = SGPPF_A8R8G8B8;
//		}
//		if (!image)
//			return 0;
//
//		if (mode != SGPTLM_WRITE_ONLY)
//		{
//			uint8* pixels = static_cast<uint8*>(image->lock());
//			if (!pixels)
//				return 0;
//
//			// we need to keep the correct texture bound later on
//			GLint tmpTexture;
//			glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmpTexture);
//			glBindTexture(GL_TEXTURE_2D, OpenGLTextureID);
//
//			// we need to flip textures vertical
//			// however, it seems that this does not hold for mipmap
//			// textures, for unknown reasons.
//
//			// allows to read pixels in top-to-bottom order
//#ifdef GL_MESA_pack_invert
//			if (!mipmapLevel && GLEE_MESA_pack_invert)
//				glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
//#endif
//
//			// download GPU data as ARGB8 to pixels;
//			glGetTexImage(GL_TEXTURE_2D, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
//
//			if (!mipmapLevel)
//			{
//#ifdef GL_MESA_pack_invert
//				if (GLEE_MESA_pack_invert)
//					glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
//				else
//#endif
//				{
//					// opengl images are horizontally flipped, so we have to fix that here.
//					const int32 pitch = image->getPitch();
//					uint8* p2 = pixels + (image->getDimension().Height - 1) * pitch;
//					uint8* tmpBuffer = new uint8[pitch];
//					for (uint32 i=0; i < image->getDimension().Height; i += 2)
//					{
//						memcpy(tmpBuffer, pixels, pitch);
//						memcpy(pixels, p2, pitch);
//						memcpy(p2, tmpBuffer, pitch);
//						pixels += pitch;
//						p2 -= pitch;
//					}
//					delete [] tmpBuffer;
//					tmpBuffer = NULL;
//				}
//			}
//			image->unlock();
//
//			//reset old bound texture
//			glBindTexture(GL_TEXTURE_2D, tmpTexture);
//		}
//	}
//	return image->lock();
}


//! unlock function
void COpenGLTexture::unlock()
{
	//// test if miplevel or main texture was locked
	//ISGPImage* image = m_MipImage ? m_MipImage : m_Image;
	//if (!image)
	//	return;
	//// unlock image to see changes
	//image->unlock();
	//// copy texture data to GPU
	//if (!ReadOnlyLock)
	//	uploadTexture(false, 0, MipLevelStored);
	//ReadOnlyLock = false;
	//// cleanup local image
	//if (m_MipImage)
	//{
	//	delete m_MipImage;
	//	m_MipImage = NULL;
	//}

	//// update information
	//if (m_Image)
	//	m_ColorFormat = m_Image->getColorFormat();
	//else
	//	m_ColorFormat = SGPPF_A8R8G8B8;
}

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void COpenGLTexture::regenerateMipMapLevels()
{
	if( !HasMipMaps || !m_Image )
		return;
	if( (m_Image->getDimension().Width==1) && (m_Image->getDimension().Height==1) )
		return;

	// Manually create mipmaps or use prepared version
	uint32 width = m_Image->getDimension().Width;
	uint32 height = m_Image->getDimension().Height;
	uint32 i=0;
	uint8* target = 0;
	do
	{
		if(width>1)
			width>>=1;
		if(height>1)
			height>>=1;
		++i;
		if(!target)
			target = new uint8[width*height*m_Image->getBytesPerPixel()];

		m_Image->copyToScaling(target, width, height, m_Image->getColorFormat());
		glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height,
				0, PixelFormat, PixelType, target);
	}
	while (width!=1 || height!=1);

	MipMapLevels = i;

	// cleanup
	if(target)
	{
		delete [] target;
		target = NULL;
	}
}

//! Bind Render Target Texture
void COpenGLTexture::bindRTT()
{
}


//! Unbind Render Target Texture
void COpenGLTexture::unbindRTT()
{
	BindTexture2D(0);

	// Copy Our ViewPort To The Texture
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
}


void COpenGLTexture::setFiltering(SGP_TEXTURE_FILTERING Magnification, SGP_TEXTURE_FILTERING Minification)
{
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

void COpenGLTexture::setWrapMode(SGP_TEXTURE_ADDRESSING s, SGP_TEXTURE_ADDRESSING t, SGP_TEXTURE_ADDRESSING r)
{
	glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, getOpenGLFormatFromTextureWrapMode(s));
	glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, getOpenGLFormatFromTextureWrapMode(t));
	if( TextureTarget == GL_TEXTURE_3D )
		glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, getOpenGLFormatFromTextureWrapMode(r));

	TextureWrap[0] = s;
	TextureWrap[1] = t;
	TextureWrap[2] = r;
}

void COpenGLTexture::setBorderColor(const Colour bordercolor)
{
	float fBorderColor[4] = {bordercolor.getFloatRed(), bordercolor.getFloatGreen(), bordercolor.getFloatBlue(), bordercolor.getFloatAlpha()};
	glTexParameterfv(TextureTarget, GL_TEXTURE_BORDER_COLOR, fBorderColor);
	TextureBorderColor = bordercolor;
}

void COpenGLTexture::setAnisotropicFilter(int iAnisotropicFilter)
{
#ifdef GL_EXT_texture_filter_anisotropic
	if(GLEE_EXT_texture_filter_anisotropic)
	{
		TextureMaxMipLevel = (iAnisotropicFilter>1) ? jmin((GLint)COpenGLConfig::getInstance()->MaxAnisotropy, iAnisotropicFilter) : 1;
		glTexParameteri(TextureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, TextureMaxMipLevel);
	}
#endif
}

void COpenGLTexture::setLODBias(float fLODBias)
{
#ifdef GL_EXT_texture_lod_bias
	if(GLEE_EXT_texture_lod_bias)
	{
		TextureLODBias = jlimit(
			-COpenGLConfig::getInstance()->MaxTextureLODBias, 
			COpenGLConfig::getInstance()->MaxTextureLODBias,
			fLODBias * 0.125f );
		glTexParameterf(TextureTarget, GL_TEXTURE_LOD_BIAS, TextureLODBias);
	}
#endif
}

void COpenGLTexture::setMaxMipLevel(int MaxMipLevel)
{
	glTexParameteri(TextureTarget, GL_TEXTURE_MAX_LEVEL, MaxMipLevel);
	TextureMaxMipLevel = MaxMipLevel;
}



bool COpenGLTexture::BindTexture2D(int iTextureUnit )
{
	if(iTextureUnit >= RenderDevice->MaxTextureUnits)
		return false;

	if(RenderDevice->MultiTextureExtension)
		RenderDevice->extGlActiveTexture(GL_TEXTURE0_ARB + iTextureUnit);

	glBindTexture(GL_TEXTURE_2D, OpenGLTextureID);
	
	return true;
}

bool COpenGLTexture::BindTexture3D(int iTextureUnit )
{
	if(iTextureUnit >= RenderDevice->MaxTextureUnits)
		return false;

	if(RenderDevice->MultiTextureExtension)
		RenderDevice->extGlActiveTexture(GL_TEXTURE0_ARB + iTextureUnit);

	glBindTexture(GL_TEXTURE_3D, OpenGLTextureID);
	
	return true;
}

bool COpenGLTexture::BindTextureCubeMap(int iTextureUnit )
{
	if( iTextureUnit >= RenderDevice->MaxTextureUnits )
		return false;
	if( !GLEE_ARB_texture_cube_map )
		return false;

	if(RenderDevice->MultiTextureExtension)
		RenderDevice->extGlActiveTexture(GL_TEXTURE0_ARB + iTextureUnit);


	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, OpenGLTextureID);
	
	return true;
}

bool COpenGLTexture::unBindTexture2D(int iTextureUnit)
{
	if(iTextureUnit >= RenderDevice->MaxTextureUnits)
		return false;
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool COpenGLTexture::unBindTexture3D(int iTextureUnit)
{
	if(iTextureUnit >= RenderDevice->MaxTextureUnits)
		return false;
	glBindTexture(GL_TEXTURE_3D, 0);
	return true;
}
bool COpenGLTexture::unBindTextureCubeMap(int iTextureUnit)
{
	if( iTextureUnit >= RenderDevice->MaxTextureUnits )
		return false;
	if( !GLEE_ARB_texture_cube_map )
		return false;
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
	
	return true;
}



#if 0
/* FBO Textures */

//! RTT ColorFrameBuffer constructor
COpenGLFBOTexture::COpenGLFBOTexture(const SDimension2D& size,
					const String& name, COpenGLRenderDevice* _renderdevice,
					SGP_PIXEL_FORMAT format)
	: COpenGLTexture(name, _renderdevice), DepthTexture(NULL), ColorFrameBuffer(0)
{
	ImageSize.Width = size.Width;
	ImageSize.Height = size.Height;
	TextureSize.Width = size.Width;
	TextureSize.Height = size.Height;

	if (SGPPF_UNKNOWN == format)
		format = getBestColorFormat(RenderDevice->getPixelFormat());

	m_ColorFormat = format;

	GLint FilteringType;
	InternalFormat = getOpenGLFormatAndParametersFromColorFormat(format, FilteringType, PixelFormat, PixelType);

	HasMipMaps = false;
	IsRenderTarget = true;

#ifdef GL_EXT_framebuffer_object
	// generate frame buffer
	RenderDevice->extGlGenFramebuffers(1, &ColorFrameBuffer);
	bindRTT();

	// generate color texture
	glGenTextures(1, &OpenGLTextureID);
	RenderDevice->setActiveTexture(0, this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
		ImageSize.Height, 0, PixelFormat, PixelType, 0);


	// attach color texture to frame buffer
	RenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT,
						GL_TEXTURE_2D,
						OpenGLTextureID,
						0);

#endif
	unbindRTT();
}


//! destructor
COpenGLFBOTexture::~COpenGLFBOTexture()
{
	if( DepthTexture )
	{
		delete DepthTexture;
		DepthTexture = NULL;
	}
	if( ColorFrameBuffer )
		RenderDevice->extGlDeleteFramebuffers(1, &ColorFrameBuffer);
}


bool COpenGLFBOTexture::isFrameBufferObject() const
{
	return true;
}


//! Bind Render Target Texture
void COpenGLFBOTexture::bindRTT()
{
#ifdef GL_EXT_framebuffer_object
	if (ColorFrameBuffer != 0)
		RenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
#endif
}


//! Unbind Render Target Texture
void COpenGLFBOTexture::unbindRTT()
{
#ifdef GL_EXT_framebuffer_object
	if (ColorFrameBuffer != 0)
		RenderDevice->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
#endif
}


/* FBO Depth Textures */

//! RTT DepthBuffer constructor
COpenGLFBODepthTexture::COpenGLFBODepthTexture(
		const SDimension2D& size,
		const String& name,
		COpenGLRenderDevice* _renderdevice,
		bool useStencil)
	: COpenGLTexture(name, _renderdevice), DepthRenderBuffer(0),
	StencilRenderBuffer(0), UseStencil(useStencil)
{

	ImageSize.Width = size.Width;
	ImageSize.Height = size.Height;
	TextureSize.Width = size.Width;
	TextureSize.Height = size.Height;

	InternalFormat = GL_RGBA;
	PixelFormat = GL_RGBA;
	PixelType = GL_UNSIGNED_BYTE;
	HasMipMaps = false;

	if (useStencil)
	{
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef GL_EXT_packed_depth_stencil
		if (GLEE_EXT_packed_depth_stencil)
		{
			// generate packed depth stencil texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_EXT, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, 0);
			StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
		}
		else // generate separate stencil and depth textures
#endif
		{
			// generate depth texture
			glTexImage2D(GL_TEXTURE_2D, 0, RenderDevice->getZBufferBits(), ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

			// generate stencil texture
			glGenTextures(1, &StencilRenderBuffer);
			glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, ImageSize.Width,
				ImageSize.Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
		}
	}
#ifdef GL_EXT_framebuffer_object
	else
	{
		// generate depth buffer
		RenderDevice->extGlGenRenderbuffers(1, &DepthRenderBuffer);
		RenderDevice->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, DepthRenderBuffer);
		RenderDevice->extGlRenderbufferStorage(GL_RENDERBUFFER_EXT,
			RenderDevice->getZBufferBits(), ImageSize.Width, ImageSize.Height);
	}
#endif
}


//! destructor
COpenGLFBODepthTexture::~COpenGLFBODepthTexture()
{
	if (DepthRenderBuffer && UseStencil)
		glDeleteTextures(1, &DepthRenderBuffer);
	else
		RenderDevice->extGlDeleteRenderbuffers(1, &DepthRenderBuffer);
	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		glDeleteTextures(1, &StencilRenderBuffer);
}


//combine depth texture and rtt
bool COpenGLFBODepthTexture::attach(ISGPTexture* renderTex)
{
	if (!renderTex)
		return false;
	COpenGLFBOTexture* rtt = static_cast<COpenGLFBOTexture*>(renderTex);
	rtt->bindRTT();
#ifdef GL_EXT_framebuffer_object
	if (UseStencil)
	{
		// attach stencil texture to stencil buffer
		RenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_STENCIL_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						StencilRenderBuffer,
						0);

		// attach depth texture to depth buffer
		RenderDevice->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_TEXTURE_2D,
						DepthRenderBuffer,
						0);
	}
	else
	{
		// attach depth renderbuffer to depth buffer
		RenderDevice->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,
						GL_DEPTH_ATTACHMENT_EXT,
						GL_RENDERBUFFER_EXT,
						DepthRenderBuffer);
	}
#endif

	rtt->DepthTexture = this;

	rtt->unbindRTT();
	return true;
}


//! Bind Render Target Texture
void COpenGLFBODepthTexture::bindRTT()
{
}
//! Unbind Render Target Texture
void COpenGLFBODepthTexture::unbindRTT()
{
}
#endif

#if SGP_MSVC
 #pragma warning (pop)
#endif