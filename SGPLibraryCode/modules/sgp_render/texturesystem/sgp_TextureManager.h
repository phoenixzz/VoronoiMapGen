#ifndef __SGP_TEXTUREMANAGER_HEADER__
#define __SGP_TEXTUREMANAGER_HEADER__

class ISGPRenderDevice;
class ISGPTexture;

class SGP_API CSGPTextureManager
{
public:
	CSGPTextureManager(ISGPRenderDevice *pdevice, Logger* logger);	
	~CSGPTextureManager();

	//! Retrieve the number of image loaders
	/** \return Number of image loaders */
	uint32 getImageLoaderCount() const;

	//! Retrieve the given image loader
	/** \param idx The index of the loader to retrieve. This parameter is an 0-based
	array index.
	\return A pointer to the specified loader, NULL if the index is incorrect. */
	ISGPImageLoader* getImageLoader(int idx);

	//! Creates an empty software image.
	/**
	\param format Desired color format of the image.
	\param size Size of the image to create.
	\return The created image.
	*/
	ISGPImage* createImage(SGP_PIXEL_FORMAT format, const SDimension2D& size);

	//! Creates a software image from a byte array.
	/** No hardware texture will be created for this image. This
	method is useful for example if you want to read a heightmap
	for a terrain renderer.
	\param format Desired color format of the texture
	\param size Desired size of the image
	\param data A byte array with pixel color information
	\param ownForeignMemory If true, the image will use the data
	pointer directly and own it afterwards. If false, the memory
	will by copied internally.
	\return The created image.
	*/
	ISGPImage* createImageFromData(SGP_PIXEL_FORMAT format,
		const SDimension2D& size, void *data,
		bool ownForeignMemory = false);

	//! Creates a software image from a file.
	//\param texturename Filename of the texture to be loaded.
	ISGPImage* createImageFromFile(const String& texturename);


	//! Enables or disables a texture creation flag.
	/** These flags define how textures should be created. By
	changing this value, you can influence for example the speed of
	rendering a lot. But please note that the video drivers take
	this value only as recommendation. 
	\param flag Texture creation flag.
	\param enabled Specifies if the given flag should be enabled or
	disabled. */
	void setTextureCreationFlag(SGP_TEXTURE_CREATION_FLAG flag, bool enabled=true);

	//! Returns if a texture creation flag is enabled or disabled.
	/** You can change this value using setTextureCreationFlag().
	\param flag Texture creation flag.
	\return The current texture creation flag enabled mode. */
	bool getTextureCreationFlag(SGP_TEXTURE_CREATION_FLAG flag) const;



	//! Returns amount of textures currently loaded
	/** \return Amount of textures currently loaded */
	uint32 getTextureCount() const;

	//! Find first empty ID in Texture Array
	uint32 getFirstEmptyID();

	//! Returns a texture index by name
	/** Loads the texture from disk if it is not already loaded and generates mipmap levels if desired.
	Texture loading can be influenced using the	setTextureCreationFlag() method.
	The texture can be in several imageformats, such as TGA, and DDS.
	DDS format support 3D texture and cubemap texture with mipmaps
	if DDS file no mipmap data, will NOT generate even bGenMipMap is TRUE
	if TGA file no mipmap data, will generate mipmap when bGenMipMap is TRUE
							else will NOT generate mipmap when bGenMipMap is FALSE
	\param filename Filename of the texture to be loaded.
	\param bGenMipMap setting if texture auto generating mipmaps
	\return ID of the ISGPTexture, or 0 if the texture
	could not be loaded.  */
	uint32 registerTexture(const String& texturename, bool bGenMipMap=false);



	//! Creates a texture from an ISGPImage.
	/** \param name A name for the texture. Later calls of
	getTextureByName() with this name will return this texture
	\param image Image the texture is created from.
	\param bHasMipmap Optional force Texture gen mipmap (gen from Image).
	\return ID of this ISGPTexture or 0 if it could not be created. */
	uint32 registerTextureFromImage(const String& name, ISGPImage* image, bool bHasMipmap=false);
	

	
	//! Adds a new render target texture to the texture manager.
	/** \param size Size of the texture, in pixels. Width and
	height should be a power of two (e.g. 64, 128, 256, 512, ...)
	and it should not be bigger than the backbuffer, because it
	shares the zbuffer with the screen buffer.
	\param name An unique name for the RTT.
	\param format The color format of the render target. Floating point formats are supported.
	\return Index to the created texture or 0 if the texture could not be created.  */
	//uint32 registerRenderTargetTexture(const SDimension2D& size,
	//		const String& name, const SGP_PIXEL_FORMAT format = SGPPF_UNKNOWN);

	//! Creates an empty texture of specified size.
	/** \param size: Size of the texture.
	\param name A name for the texture. Later calls to
	getTextureByName() with this name will return this texture
	\param format Desired color format of the texture. Please note
	that the driver may choose to create the texture in another
	color format.
	\return ID to ISGPTexture of the newly created texture or
		0 if it could not be created. */
	uint32 registerEmptyTexture(const SDimension2D& size,
			const String& name, SGP_PIXEL_FORMAT format = SGPPF_A8R8G8B8);



	//! Returns a texture by name String
	/** Return the texture from Texture manager if it is be found else 0
	\param filename Filename of the texture to find.
	\return ID of the texture, or 0 if the texture could not be found. */
	uint32 getTextureIDByName(const String& filename);

	//! Returns a texture by ID
	/** Return the texture from Texture manager if it is be found else NULL
	\param id Index of the texture to find.
	\return Pointer the texture, or NULL if the texture could not be found. */
	CTextureResource* getTextureByID(uint32 id);


	//! Removes a texture from the texture manager and deletes it.
	/** This method can free a lot of memory!
	Please note that after calling this, the pointer to the
	ISGPTexture may no longer be valid.
	\param texturename Texture name will be deleted from the manager. */
	void unRegisterTextureByName( const String& texturename );
	// \param id Texture id will be deleted from the manager. */
	void unRegisterTextureByID(uint32 id);


	//! Removes all textures from the texture manager and deletes them.
	/** This method can free a lot of memory!
	Please note that after calling this, the pointer to the
	ISGPTexture may no longer be valid. */
	void removeAllTextures();

	//! Register / Unregister Default texture in first Texture ID, mostly for Debug
	//! For later RegisterTexture, if failed, TextureManager will return index 0
	void createDefaultTexture();
	void releaseDefaultTexture();

	//! Register / Unregister pure white texture in texture ID 1
	void createWhiteTexture();
	void releaseWhiteTexture();
	//! Register / Unregister pure black texture in texture ID 1
	void createBlackTexture();
	void releaseBlackTexture();

	//! Setting Working Direction
	inline void setWorkingDirection(const String& workingDir)
	{ m_WorkingDir = workingDir; }



	//////////////////////////////////////////////////////////////////////////////
	// Multi-Thread version of Function registerTexture
	uint32 registerTextureMT(const String& texturename, bool bGenMipMap=false);

	// Multi-Thread version of unRegister Function
	void unRegisterTextureByNameMT( const String& texturename );
	void unRegisterTextureByIDMT(uint32 id);

	// Below two functions called by ResourceMuitiThreadLoader
	// called from render-thread, when background thread has loaded texture raw data,
	// creating / releasing render resource
	uint32 registerTextureFromResourceMT( SGPTextureRecord Record );
	void unRegisterTextureFromResourceMT( SGPTextureRecord Record );


private:
	// Init m_Textures Array size also m_StringToTextureIDMap hash map slot number
	// (so that the array won't have to keep dynamically resizing itself as the elements are added, 
	// and it'll therefore be more efficient. )
	static const int			INIT_TEXTUREARRAYSIZE = 2048;

	ISGPRenderDevice*			m_pRenderDevice;
	Logger*						m_pLogger;

	String						m_WorkingDir;

	uint32						m_TextureCreationFlags;

	// Picture Loader Array
	OwnedArray<ISGPImageLoader> m_ImageLoaderArray;

	// Created Texture Array
	OwnedArray<CTextureResource> m_Textures;

	// Hashmap of texture file path string to Index of Texture Array
	HashMap<uint64, uint32>		m_StringToTextureIDMap;
};



#endif		// __SGP_TEXTUREMANAGER_HEADER__