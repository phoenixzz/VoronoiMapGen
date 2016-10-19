#ifndef __SGP_IMAGEUNCOMPRESSED_HEADER__
#define __SGP_IMAGEUNCOMPRESSED_HEADER__

//! ISGPImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images.
class SGPImageUncompressed : public ISGPImage
{
public:

	//! constructor from raw image data
	/** \param useForeignMemory: If true, the image will use the data pointer
	directly and own it from now on, which means it will also try to delete [] the
	data when the image will be destructed. If false, the memory will by copied. */
	SGPImageUncompressed(SGP_PIXEL_FORMAT format, const SDimension2D& size,
		void* data, bool ownForeignMemory=true);

	//! constructor for empty image
	SGPImageUncompressed(SGP_PIXEL_FORMAT format, const SDimension2D& size);

	//! destructor
	virtual ~SGPImageUncompressed();

	//! Lock function.
	virtual void* lock()
	{
		return Data;
	}

	//! Unlock function.
	virtual void unlock() {}

	//! Returns width and height of image data.
	virtual const SDimension2D& getDimension() const;

	//! Returns bits per pixel.
	virtual uint32 getBitsPerPixel() const;

	//! Returns bytes per pixel
	virtual uint32 getBytesPerPixel() const;

	//! Returns image data size in bytes
	virtual uint32 getImageDataSizeInBytes() const;

	//! Returns image data size in pixels
	virtual uint32 getImageDataSizeInPixels() const;

	//! returns mask for red value of a pixel
	virtual uint32 getRedMask() const;

	//! returns mask for green value of a pixel
	virtual uint32 getGreenMask() const;

	//! returns mask for blue value of a pixel
	virtual uint32 getBlueMask() const;

	//! returns mask for alpha value of a pixel
	virtual uint32 getAlphaMask() const;

	//! returns a pixel
	virtual Colour getPixel(uint32 x, uint32 y) const;

	//! sets a pixel
	virtual void setPixel(uint32 x, uint32 y, const Colour &color, bool blend = false );

	//! returns the color format
	virtual SGP_PIXEL_FORMAT getColorFormat() const;

	//! returns pitch of image
	virtual uint32 getPitch() const { return Pitch; }

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(void* _target, uint32 _width, uint32 _height, SGP_PIXEL_FORMAT _format, uint32 _pitch=0);

	//! copies this surface into another, scaling it to fit.
	virtual void copyToScaling(ISGPImage* target);

	//! copies this surface into another
	virtual void copyTo(ISGPImage* target, const SDimension2D& pos);

	//! copies this surface into another
	virtual void copyTo(ISGPImage* target, const SDimension2D& pos, const SRect& sourceRect, const SRect* clipRect=0);

	//! fills the surface with given color
	virtual void fill(const Colour &color);

	//! return if ISGPImage is DDS data?
	virtual bool IsDDSImage() { return false; }

	//! return if ISGPImage is PVRTC data?
	virtual bool IsPVRTCImage() { return false; }

private:

	//! assumes format and size has been set and creates the rest
	void initData();

	inline Colour getPixelBox ( int32 x, int32 y, int32 fx, int32 fy, int32 bias ) const;

	uint8* Data;
	SDimension2D Size;
	uint32 BytesPerPixel;
	uint32 Pitch;
	SGP_PIXEL_FORMAT Format;

	bool DeleteMemory;
};


#endif		// __SGP_IMAGEUNCOMPRESSED_HEADER__