#ifndef __SGP_OPENGLES2TEXTURE_HEADER__
#define __SGP_OPENGLES2TEXTURE_HEADER__

class COpenGLES2Texture : public ISGPTexture
{
public:
	//! constructor
	COpenGLES2Texture(ISGPImage* surface, const String& name, COpenGLES2RenderDevice* renderdevice, bool bHasMipmaps=false);

	//! destructor
	virtual ~COpenGLES2Texture();

	//! Create from DDS
	virtual void updateDDSTexture(ISGPImage* surface) {}

	//! lock function
	virtual void* lock(SGP_TEXTURE_LOCK_MODE mode=SGPTLM_READ_WRITE, uint32 mipmapLevel=0) { return NULL; }

	//! unlock function
	virtual void unlock() {}

	//! Returns original size of the texture (image).
	virtual const SDimension2D& getOriginalSize() const
	{ return ImageSize; }

	//! Returns size of the texture.
	virtual const SDimension2D& getSize() const
	{ return TextureSize; }

	//! returns driver type of texture (=the driver, that created it)
	virtual SGP_DRIVER_TYPE getDriverType() const
	{ return SGPDT_OPENGLES2; }

	//! returns color format of texture
	virtual SGP_PIXEL_FORMAT getColorFormat() const
	{ return SGPPF_A8R8G8B8; }

	//! returns pitch of texture (in bytes)
	virtual uint32 getPitch() const
	{ 	
		if (m_Image)
			return m_Image->getPitch();
		else
			return 0;
	}

	//! return whether this texture has mipmaps
	virtual bool hasMipMaps() const 
	{ return HasMipMaps; }

	//! Get the mip map levels of the texture. if NO mipmaps, return 0
	virtual uint32 getMipMapLevels() const
	{ return MipMapLevels; }

	//! Regenerates the mip map levels of the texture.
	virtual void regenerateMipMapLevels();

	//! Is it a render target?
	virtual bool isRenderTarget() const
	{ return false; }

	virtual bool isTexture2D() const
	{ return TextureTarget == GL_TEXTURE_2D; }
	virtual bool isTexture3D() const
	{ return false; }
	virtual bool isCubeMap() const
	{ return TextureTarget == GL_TEXTURE_CUBE_MAP; }

	//! Getting access the pixels from texture.
	virtual bool getMipmapData(void* pixels, uint32 mipmapLevel, SGP_TEXTURE_TARGET target) { return false; }

	//! Set Magnification and Minification of Texture
	virtual void setFiltering(SGP_TEXTURE_FILTERING Magnification, SGP_TEXTURE_FILTERING Minification);
	
	//! All kinds of texture parameter
	virtual void setWrapMode(SGP_TEXTURE_ADDRESSING s, SGP_TEXTURE_ADDRESSING t, SGP_TEXTURE_ADDRESSING r=TEXTURE_ADDRESS_REPEAT);
	
	virtual void setBorderColor(const Colour bordercolor) {}
	virtual void setAnisotropicFilter(int iAnisotropicFilter) {}
	virtual void setLODBias(float fLODBias) {}
	virtual void setMaxMipLevel(int MaxMipLevel) {}

	// OPENGL Texture virtual function

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const
	{ return false; }

	//! Bind RenderTargetTexture
	virtual void bindRTT() {}

	//! Unbind RenderTargetTexture
	virtual void unbindRTT() {}


	//! return open gl texture name
	GLuint getOpenGLTextureID() const { return OpenGLTextureID; }


	/* ! sets the current Texture
	//! Returns whether setting was a success or not.
	\params	iTextureUnit - texture unit to bind texture to
	*/
	virtual bool BindTexture2D(int iTextureUnit = 0);
	virtual bool BindTextureCubeMap(int iTextureUnit = 0);
	virtual bool unBindTexture2D(int iTextureUnit = 0);
	virtual bool unBindTextureCubeMap(int iTextureUnit = 0);
	virtual bool BindTexture3D(int iTextureUnit = 0) { return false; }
	virtual bool unBindTexture3D(int iTextureUnit = 0) { return false; }

public:
	GLenum GetTextureTarget() { return TextureTarget; }

protected:
	//! protected constructor with basic setup, no GL texture name created, for derived classes
	COpenGLES2Texture(const String& name, COpenGLES2RenderDevice* renderdevice);

	//! Get the OpenGL ES2 texture format based on SGP Image
	void getOpenGLES2FormatFromImage(ISGPImage* pImage, GLenum& internalformat, GLenum& format, GLenum& type);

	//! Get the OpenGL ES2 texture coordinate wrap mode based on SGP texture flags
	GLint getOpenGLFormatFromTextureWrapMode(SGP_TEXTURE_ADDRESSING wrap);

	//! set texture parameters into an OpenGL texture.
	void setTextureParameter();



protected:

	COpenGLES2RenderDevice* RenderDevice;
	SGPImagePVRTC*	m_Image;
	ISGPImage*		m_OrigImage;

	GLuint OpenGLTextureID;

	//Setup GL Texture format values.
	GLenum eTextureFormat;
	GLenum eTextureInternalFormat;	// often this is the same as textureFormat, but not for BGRA8888 on iOS, for instance
	GLenum eTextureType;


	bool	bIsCompressedFormatSupported;
	bool	bIsCompressedFormat;


	SDimension2D ImageSize;
	SDimension2D TextureSize;



	bool bPVRTCTexture;					// texture is PVRTC Format?
	bool HasMipMaps;					// texture has mipmaps?

	uint32 MipMapLevels;				// texture mipmap levels


	// OpenGL Texture Parameters
	GLenum TextureTarget;
	SGP_TEXTURE_FILTERING TextureMagFilter;
	SGP_TEXTURE_FILTERING TextureMinFilter;

	SGP_TEXTURE_ADDRESSING TextureWrap[3];
};



#endif		// __SGP_OPENGLES2TEXTURE_HEADER__