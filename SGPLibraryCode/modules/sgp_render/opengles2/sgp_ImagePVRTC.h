#ifndef __SGP_IMAGEPVRTC_HEADER__
#define __SGP_IMAGEPVRTC_HEADER__

// Description:
// 
// Loads PVRTC images (	PVRTC 2bpp RGB, PVRTC 2bpp RGBA, PVRTC 4bpp RGB, PVRTC 4bpp RGBA, 
//						PVRTC-II 2bpp, PVRTC-II 4bpp, ETC1 for IPHONE are supported ).
// Image is flipped when its loaded as GL images are stored with different coordinate system. 
// If file has mipmaps and/or cubemaps then these are loaded as well.
// Volume textures are NOT supported in OpenGLES 2.0
// It Can load parts of a mip mapped texture (i.e. skipping the highest detailed levels). 
// In OpenGL Cube Map, each texture's up direction is defined as next (view direction, up direction),
//				(+x,-y)(-x,-y)(+y,+z)(-y,-z)(+z,-y)(-z,-y).
// SGPImagePVRTC class skip MetaData in PVRTC texture

//! ISGPImage implementation with a lot of special image operations for PVRTC images.
class SGPImagePVRTC : public ISGPImage
{
public:
	// texPtr If null, texture follows header, else texture is here.
	SGPImagePVRTC( const void* pointer, const void * const texPtr = NULL, bool bReadMetaData = false );


	//! destructor
	virtual ~SGPImagePVRTC();


	//! return if ISGPImage is DDS data?
	virtual bool IsDDSImage() { return false; }

	//! return if ISGPImage is PVRTC data?
	virtual bool IsPVRTCImage() { return true; }

	virtual const SDimension2D& getDimension() const { return m_Size; }

	//! Returns pitch of image
	virtual uint32 getPitch() const { return 0; }

private:
	virtual void* lock() { return NULL; }
	virtual void unlock() {}
	virtual uint32 getBitsPerPixel() const{ return 0; }
	virtual uint32 getBytesPerPixel() const { return 0; }
	virtual uint32 getImageDataSizeInBytes() const { return 0; }
	virtual uint32 getImageDataSizeInPixels() const { return 0; }
	virtual Colour getPixel(uint32 , uint32 ) const { return Colour(0); }
	virtual void setPixel(uint32 , uint32 , const Colour& , bool ) {}
	//! Returns the color format
	virtual SGP_PIXEL_FORMAT getColorFormat() const { return SGPPF_A8R8G8B8; }

	//! Returns mask for red value of a pixel
	virtual uint32 getRedMask() const { return 0; }

	//! Returns mask for green value of a pixel
	virtual uint32 getGreenMask() const { return 0; }

	//! Returns mask for blue value of a pixel
	virtual uint32 getBlueMask() const { return 0; }

	//! Returns mask for alpha value of a pixel
	virtual uint32 getAlphaMask() const { return 0; }

	//! Copies the image into the target, scaling the image to fit
	virtual void copyToScaling(void* , uint32 , uint32 , SGP_PIXEL_FORMAT , uint32 ) {}

	//! Copies the image into the target, scaling the image to fit
	virtual void copyToScaling(ISGPImage* ) {}

	//! copies this surface into another
	virtual void copyTo(ISGPImage* , const SDimension2D& ) {}

	//! copies this surface into another
	virtual void copyTo(ISGPImage* , const SDimension2D& , const SRect& , const SRect* ) {}

	//! fills the surface with given color
	virtual void fill(const Colour&) {}

public:

	inline PVRTextureHeaderV3&	getTextureHeader() { return m_sTextureHeader; }
	inline PVRTuint8*			getTextureData() { return m_pTextureData; }
	inline void					setAllowDecompress(bool bAllow) { m_bAllowDecompress = bAllow; }
	inline bool					getAllowDecompress() { return m_bAllowDecompress; }
	inline void					setLoadFromLevel(uint32 nLevel) { m_nLoadFromLevel = nLevel; }
	inline uint32				getLoadFromLevel() { return m_nLoadFromLevel; }
	inline bool					IsLegacyPVR() { return m_bIsLegacyPVR; }

private:
	SDimension2D			m_Size;
	PVRTextureHeaderV3		m_sTextureHeader;		// a PVRTextureHeaderV3 struct, contain the header data of the returned texture
	PVRTuint8*				m_pTextureData;
	PVRTuint8*				m_pFileInMemory;
	bool					m_bAllowDecompress;		// Allow decompression if PVRTC is not supported in hardware
	uint32					m_nLoadFromLevel;		// Which mip map level to start loading from (0=all)

	bool					m_bReadMetaData;
	CPVRTMap<uint32, CPVRTMap<uint32, MetaDataBlock>> m_MetaData;	// If a valid map is supplied and m_bReadMetaData is true, this will contain any and all 
																	// MetaDataBlocks stored in the texture, organised by DevFourCC then identifier. 

	bool					m_bIsLegacyPVR;
};

#endif		// __SGP_IMAGEPVRTC_HEADER__