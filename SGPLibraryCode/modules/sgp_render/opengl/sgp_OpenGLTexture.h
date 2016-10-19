#ifndef __SGP_OPENGLTEXTURE_HEADER__
#define __SGP_OPENGLTEXTURE_HEADER__

class COpenGLTexture : public ISGPTexture
{
public:
	//! constructor
	COpenGLTexture(ISGPImage* surface, const String& name, COpenGLRenderDevice* renderdevice, bool bHasMipmaps=false);

	//! destructor
	virtual ~COpenGLTexture();

	//! Create from DDS
	virtual void updateDDSTexture(ISGPImage* surface);

	//! lock function
	virtual void* lock(SGP_TEXTURE_LOCK_MODE mode=SGPTLM_READ_WRITE, uint32 mipmapLevel=0);

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture (image).
	virtual const SDimension2D& getOriginalSize() const
	{ return ImageSize; }

	//! Returns size of the texture.
	virtual const SDimension2D& getSize() const
	{ return TextureSize; }

	//! returns driver type of texture (=the driver, that created it)
	virtual SGP_DRIVER_TYPE getDriverType() const
	{ return SGPDT_OPENGL; }

	//! returns color format of texture
	virtual SGP_PIXEL_FORMAT getColorFormat() const
	{ return m_ColorFormat; }

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
	{ return IsRenderTarget; }

	virtual bool isTexture2D() const
	{ return TextureTarget == GL_TEXTURE_2D; }
	virtual bool isTexture3D() const
	{ return TextureTarget == GL_TEXTURE_3D; }
	virtual bool isCubeMap() const
	{ return TextureTarget == GL_TEXTURE_CUBE_MAP_ARB; }

	//! Getting access the pixels from texture.
	virtual bool getMipmapData(void* pixels, uint32 mipmapLevel, SGP_TEXTURE_TARGET target);

	//! Set Magnification and Minification of Texture
	virtual void setFiltering(SGP_TEXTURE_FILTERING Magnification, SGP_TEXTURE_FILTERING Minification);
	//! All kinds of texture parameter
	virtual void setWrapMode(SGP_TEXTURE_ADDRESSING s, SGP_TEXTURE_ADDRESSING t, SGP_TEXTURE_ADDRESSING r=TEXTURE_ADDRESS_REPEAT);
	virtual void setBorderColor(const Colour bordercolor);
	virtual void setAnisotropicFilter(int iAnisotropicFilter);
	virtual void setLODBias(float fLODBias);
	virtual void setMaxMipLevel(int MaxMipLevel);

	// OPENGL Texture virtual function

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const
	{ return false; }

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	//! sets whether this texture is intended to be used as a render target.
	void setIsRenderTarget(bool isTarget)
	{ IsRenderTarget = isTarget; }

	//! return open gl texture name
	GLuint getOpenGLTextureID() const { return OpenGLTextureID; }


	/* ! sets the current Texture
	//! Returns whether setting was a success or not.
	\params	iTextureUnit - texture unit to bind texture to
	*/
	virtual bool BindTexture2D(int iTextureUnit = 0);
	virtual bool BindTexture3D(int iTextureUnit = 0);
	virtual bool BindTextureCubeMap(int iTextureUnit = 0);
	virtual bool unBindTexture2D(int iTextureUnit = 0);
	virtual bool unBindTexture3D(int iTextureUnit = 0);
	virtual bool unBindTextureCubeMap(int iTextureUnit = 0);

public:
	GLenum GetTextureTarget() { return TextureTarget; }

protected:
	//! protected constructor with basic setup, no GL texture name created, for derived classes
	COpenGLTexture(const String& name, COpenGLRenderDevice* renderdevice);

	//! Get the OpenGL texture coordinate wrap mode based on SGP texture flags
	GLint getOpenGLFormatFromTextureWrapMode(SGP_TEXTURE_ADDRESSING wrap);

	//! get the desired color format based on texture creation flags and the input format.
	SGP_PIXEL_FORMAT getBestColorFormat(SGP_PIXEL_FORMAT format);

	//! Get the OpenGL color format parameters based on the given SGP color format
	GLint getOpenGLFormatAndParametersFromColorFormat(
		SGP_PIXEL_FORMAT format, GLint& filtering, GLenum& colorformat, GLenum& type);

	//! Get SGP color format from the given OpenGL color format
	SGP_PIXEL_FORMAT getColorFormatFromInternalFormat(GLint InternalFormat);

	//! get important numbers of the image and hw texture
	void getImageValues(ISGPImage* image);

	//! copies the texture into an OpenGL texture.
	void uploadTexture();



protected:
	SDimension2D ImageSize;
	SDimension2D TextureSize;
	SGP_PIXEL_FORMAT m_ColorFormat;
	COpenGLRenderDevice* RenderDevice;
	ISGPImage* m_Image;
	//ISGPImage* m_MipImage;

	GLuint OpenGLTextureID;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;

	//uint32 MipLevelStored;			// used for lock / unlock
	bool HasMipMaps;					// texture has mipmaps?
	bool IsRenderTarget;
	uint32 MipMapLevels;				// texture mipmap levels

	//bool ReadOnlyLock;

	// OpenGL Texture Parameters
	GLenum TextureTarget;
	SGP_TEXTURE_FILTERING TextureMagFilter;
	SGP_TEXTURE_FILTERING TextureMinFilter;
	Colour TextureBorderColor;
	GLint TextureAnisotropicFilter;
	float TextureLODBias;
	GLint TextureMaxMipLevel;
	SGP_TEXTURE_ADDRESSING TextureWrap[3];
};

#if 0
//! OpenGL FBO texture.
class COpenGLFBOTexture : public COpenGLTexture
{
public:

	//! FrameBufferObject constructor
	COpenGLFBOTexture(const SDimension2D& size, const String& name,
		COpenGLRenderDevice* renderdevice = 0, const SGP_PIXEL_FORMAT format = SGPPF_UNKNOWN);

	//! destructor
	virtual ~COpenGLFBOTexture();

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

public:
	ISGPTexture* DepthTexture;

protected:
	GLuint ColorFrameBuffer;
};

//! OpenGL FBO depth texture.
class COpenGLFBODepthTexture : public COpenGLTexture
{
public:
	//! FrameBufferObject depth constructor
	COpenGLFBODepthTexture(const SDimension2D& size, const String& name, COpenGLRenderDevice* renderdevice=0, bool useStencil=false);

	//! destructor
	virtual ~COpenGLFBODepthTexture();

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	bool attach(ISGPTexture*);

protected:
	GLuint DepthRenderBuffer;
	GLuint StencilRenderBuffer;
	bool UseStencil;
};
#endif

#endif		// __SGP_OPENGLTEXTURE_HEADER__