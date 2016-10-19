#ifndef	__SGP_TEXTURE_HEADER__
#define __SGP_TEXTURE_HEADER__

//! Enumeration flags telling the video driver in which format textures should be created.
enum SGP_TEXTURE_CREATION_FLAG
{
	/** Discard any alpha layer and use non-alpha color format. */
	SGPTCF_NO_ALPHA_CHANNEL = 0x00000020,

	//! Allow the Driver to use Non-Power-2-Textures
	SGPTCF_ALLOW_NON_POWER_2 = 0x00000040,

	/** This flag is never used, it only forces the compiler to compile
	these enumeration values to 32 bit. */
	SGPTCF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

//! Enum for the mode for texture locking. Read-Only, write-only or read/write.
enum SGP_TEXTURE_LOCK_MODE
{
	//! The default mode. Texture can be read and written to.
	SGPTLM_READ_WRITE = 0,

	//! Read only. The texture is downloaded, but not uploaded again.
	/** Often used to read back shader generated textures. */
	SGPTLM_READ_ONLY,

	//! Write only. The texture is not downloaded and might be uninitialised.
	/** The updated texture is uploaded to the GPU.
	Used for initialising the shader from the CPU. */
	SGPTLM_WRITE_ONLY
};

enum SGP_TEXTURE_TARGET
{
	SGPTT_TEXTURE_2D = 0,
	SGPTT_TEXTURE_3D,
	SGPTT_TEXTURE_CUBE_MAP_POSITIVE_X,
	SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_X,
	SGPTT_TEXTURE_CUBE_MAP_POSITIVE_Y,
	SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	SGPTT_TEXTURE_CUBE_MAP_POSITIVE_Z,
	SGPTT_TEXTURE_CUBE_MAP_NEGATIVE_Z
};


enum SGP_TEXTURE_FILTERING
{
	TEXTURE_FILTER_MAG_NEAREST = 0,		// Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR,		// Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST,			// Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR,		// Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP,	// Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR,		// Bilinear criterion for minification on two closest mipmaps, then averaged
};

enum SGP_TEXTURE_ADDRESSING
{
	TEXTURE_ADDRESS_CLAMP_TO_EDGE = 0,		// clamped to the range [0.5/Tex Dimision, 1-0.5/Tex Dimision]
	TEXTURE_ADDRESS_CLAMP_TO_BORDER,		// clamped to border color
	TEXTURE_ADDRESS_MIRRORED_REPEAT,		// 
	TEXTURE_ADDRESS_REPEAT,					// the integer part of the tex coordinate to be ignored
};

//! Interface of a Video Driver dependent Texture.
/** An ISGPTexture is created by CSGPTextureManager of this Video Driver. 
The texture may only be used by this VideoDriver. As you can imagine, 
textures of the DirectX and the OpenGL device will, e.g., not be compatible.
If you try to use a texture created by one device with an other device, 
the device will refuse to do that and write a warning or an error message to the output buffer.
*/
class ISGPTexture
{
public:
	//! constructor
	ISGPTexture(const String& PathName) : m_TextureName(PathName)
	{
	}

	virtual ~ISGPTexture() {}

	//! load and init DDS texture if created from SGPImageDDS
	virtual void updateDDSTexture(ISGPImage* surface) = 0;

	//! Lock function.
	/** Locks the Texture and returns a pointer to access the
	pixels. After lock() has been called and all operations on the pixels
	are done, you must call unlock().
	Locks are not accumulating, hence one unlock will do for an arbitrary
	number of previous locks. You should avoid locking different levels without
	unlocking inbetween, though, because only the last level locked will be
	unlocked.
	The size of the i-th mipmap level is defined as max(getSize().Width>>i,1)
	and max(getSize().Height>>i,1)
	\param mode Specifies what kind of changes to the locked texture are
	allowed. Unspecified behavior will arise if texture is written in read
	only mode or read from in write only mode.
	Support for this feature depends on the driver, so don't rely on the
	texture being write-protected when locking with read-only, etc.
	\param mipmapLevel Number of the mipmapLevel to lock. 0 is main texture.
	Non-existing levels will silently fail and return 0.
	\return Returns a pointer to the pixel data. The format of the pixel can
	be determined by using getColorFormat(). 0 is returned, if
	the texture cannot be locked. */
	virtual void* lock(SGP_TEXTURE_LOCK_MODE mode=SGPTLM_READ_WRITE, uint32 mipmapLevel=0) = 0;

	//! Unlock function. Must be called after a lock() to the texture.
	/** One should avoid to call unlock more than once before another lock.
	The last locked mip level will be unlocked. */
	virtual void unlock() = 0;

	//! Get original size of the texture.
	/** The texture is usually scaled, if it was created with an unoptimal
	size. For example if the size was not a power of two. This method
	returns the size of the texture it had before it was scaled. Can be
	useful when drawing 2d images on the screen, which should have the
	exact size of the original texture. Use ISGPTexture::getSize() if you want
	to know the real size it has now stored in the system.
	\return The original size of the texture. */
	virtual const SDimension2D& getOriginalSize() const = 0;

	//! Get dimension (=size) of the texture.
	/** \return The size of the texture. */
	virtual const SDimension2D& getSize() const = 0;

	//! Get driver type of texture.
	/** This is the driver, which created the texture. This method is used
	internally by the video devices, to check, if they may use a texture
	because textures may be incompatible between different devices.
	\return Driver type of texture. */
	virtual SGP_DRIVER_TYPE getDriverType() const = 0;

	//! Get the color format of texture.
	/** \return The color format of texture. */
	virtual SGP_PIXEL_FORMAT getColorFormat() const = 0;

	//! Get pitch of the main texture (in bytes).
	/** The pitch is the amount of bytes used for a row of pixels in a texture.
	\return Pitch of texture in bytes. */
	virtual uint32 getPitch() const = 0;

	//! Check whether the texture has MipMaps
	/** \return True if texture has MipMaps, else false. */
	virtual bool hasMipMaps() const { return false; }

	//! Returns if the texture has an alpha channel
	virtual bool hasAlpha() const 
	{
		return getColorFormat () == SGPPF_A8R8G8B8 || getColorFormat () == SGPPF_A1R5G5B5;
	}

	//! Get the mip map levels of the texture. if NO mipmaps, return 0
	virtual uint32 getMipMapLevels() const = 0;

	//! Regenerates the mip map levels of the texture.
	virtual void regenerateMipMapLevels() = 0;

	//! Returns if the texture is Texture 2d/3d/cubemap
	virtual bool isTexture2D() const = 0;
	virtual bool isTexture3D() const = 0;
	virtual bool isCubeMap() const = 0;

	//! getting access the pixels from texture.
	/*	
	The size of the i-th mipmap level is defined as max(getSize().Width>>i,1)
	and max(getSize().Height>>i,1)

	\param pixels A pointer to the pixel data
	\param mipmapLevel Number of the mipmapLevel to get. 0 is main texture.
	Non-existing levels will silently fail and return false.
	\param target Specifies what kind of target to get from texture. 
	The format of the pixel is ARGB8.
	\return false is returned if error. */
	virtual bool getMipmapData(void* pixels, uint32 mipmapLevel, SGP_TEXTURE_TARGET target) = 0;

	//! Check whether the texture is a render target
	/** Render targets can be set as such in the video driver, in order to
	render a scene into the texture. Once unbound as render target, they can
	be used just as usual textures again.
	\return True if this is a render target, otherwise false. */
	virtual bool isRenderTarget() const { return false; }


	// All kinds of texture parameter
	//! Set magnification and minification texture filter Level according to mipmap.
	/** \params	_Mag _Min - texture filtering for MAG/MIN*/
	virtual void setFiltering(SGP_TEXTURE_FILTERING _Mag, SGP_TEXTURE_FILTERING _Min) = 0;
	virtual void setWrapMode(SGP_TEXTURE_ADDRESSING s, SGP_TEXTURE_ADDRESSING t, SGP_TEXTURE_ADDRESSING r=TEXTURE_ADDRESS_REPEAT) = 0;
	virtual void setBorderColor(const Colour bordercolor) = 0;
	virtual void setAnisotropicFilter(int iAnisotropicFilter) = 0;
	virtual void setLODBias(float fLODBias) = 0;
	virtual void setMaxMipLevel(int MaxMipLevel) = 0;

	/* ! sets the current Texture
	//! Returns whether setting was a success or not.
	\params	iTextureUnit - texture unit to bind texture to
	*/
	virtual bool BindTexture2D(int iTextureUnit = 0) = 0;
	virtual bool BindTexture3D(int iTextureUnit = 0) = 0;
	virtual bool BindTextureCubeMap(int iTextureUnit = 0) = 0;
	virtual bool unBindTexture2D(int iTextureUnit = 0) = 0;
	virtual bool unBindTexture3D(int iTextureUnit = 0) = 0;
	virtual bool unBindTextureCubeMap(int iTextureUnit = 0) = 0;

	//! Get name of texture (in most cases this is the filename)
	const String& getName() const { return m_TextureName; }

protected:
	String	m_TextureName;

};

#endif		// __SGP_TEXTURE_HEADER__