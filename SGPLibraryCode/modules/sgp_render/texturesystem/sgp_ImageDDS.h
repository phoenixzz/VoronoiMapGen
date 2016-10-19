#ifndef __SGP_IMAGEDDS_HEADER__
#define __SGP_IMAGEDDS_HEADER__


// Description:
// 
// Loads DDS images (DXTC1, DXTC3, DXTC5, RGB (888, 888X), and RGBA (8888) are
// supported). Image is flipped when its loaded as GL images
// are stored with different coordinate system. If file has mipmaps and/or 
// cubemaps then these are loaded as well. Volume textures can be loaded as 
// well but they must be uncompressed.
//
// When multiple textures are loaded (i.e a volume or cubemap texture), 
// additional texture data can be accessed using getMipmapData() . 
//
// The mipmaps for each face are also stored in a list and can be accessed like 
// so: getCubemap() and getMipmapData() (which accesses the first mipmap of the first 
// image). To get the number of mipmaps call the getNumberOfMipmaps function.
//
// Call isCompressed() function to check that a loaded image is compressed DXT1/3/5 formats
// Call the isVolume() or isCubemap() function to check that a loaded image
// is a volume or cubemap texture respectively. If a volume texture is loaded
// then the getMipmapDepth() function should return a number greater than 1. 
// Mipmapped volume textures and DXTC compressed volume textures are supported.
//



//! ISGPImage implementation with a lot of special image operations for DDS images.
class SGPImageDDS : public ISGPImage
{
public:
	SGPImageDDS( uint8* FileInMem, uint8* data, int32 width, int32 height, 
		int32 cubemap, int32 depth,	uint32 mipMapCount,
		bool compressed, bool swap,	int32 divSize, uint32 blockBytes, 
		uint32 internalFormat, uint32 externalFormat, uint32 type );

	virtual ~SGPImageDDS();

	//! return if ISGPImage is DDS data?
	virtual bool IsDDSImage() { return true; }

	//! return if ISGPImage is PVRTC data?
	virtual bool IsPVRTCImage() { return false; }

private:
	virtual void* lock() { return NULL; }
	virtual void unlock() {}
	virtual const SDimension2D& getDimension() const { return getMipmapSize(0); }
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

	//! Returns pitch of image
	virtual uint32 getPitch() const { return 0; }

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
	inline uint8* getMipmapData(int miplevel) const
	{ 
		jassert(miplevel < MipmapData.size());
		return MipmapData[miplevel]; 
	}
	inline uint32 getMipmapDepth(int miplevel) const
	{
		jassert(miplevel < MipmapDepth.size());
		return MipmapDepth[miplevel];
	}
	inline SDimension2D& getMipmapSize(int miplevel) const
	{
		jassert(miplevel < MipmapSizes.size());
		return MipmapSizes.getReference(miplevel);
	}
	inline uint32 getMipmapDataBytes(int miplevel) const
	{
		jassert(miplevel < MipmapDataBytes.size());
		return MipmapDataBytes[miplevel];
	}

	inline uint8* getCubemap(int idx) const
	{
		jassert(idx < CubeMapData.size());
		return CubeMapData[idx];
	}


    inline int getNumberOfMipmaps() { return MipmapData.size(); }

    inline int32 getExternalFormat() { return iExternalFormat; }
    inline int32 getInternalFormat() { return iInternalFormat; }
	inline int32 getPixelType() { return PixelType; }

    inline bool isCompressed() { return bCompressed; }
    inline bool isCubemap() { return bCubemap; }
    inline bool isVolume() { return bVolume; }
	inline bool isSwap() { return bSwap; }

private:
	SGPImageDDS() {}

private:

	bool					bVolume;
	bool					bCubemap;
	bool					bCompressed;
	bool					bSwap;

	uint32					iExternalFormat;
	uint32					iInternalFormat;
	uint32					PixelType;

	uint8*					pFileInMemory;

	Array<int32>			MipmapDepth;
	Array<SDimension2D>		MipmapSizes;
	Array<uint8*>			MipmapData;
	Array<uint32>			MipmapDataBytes;

	Array<uint8*>			CubeMapData;
};

#endif		// __SGP_IMAGEDDS_HEADER__