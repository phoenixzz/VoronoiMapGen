#ifndef	__SGP_IMAGE_HEADER__
#define __SGP_IMAGE_HEADER__

//! Interface for SGP image data.
/** Image loaders create these images from files. ISGPRenderDevice convert
these images into their (hardware) textures.
*/
class ISGPImage
{
public:
	virtual ~ISGPImage() {}
	//! Lock function. Use this to get a pointer to the image data.
	/** After you don't need the pointer anymore, you must call unlock().
	\return Pointer to the image data. What type of data is pointed to
	depends on the color format of the image. For example if the color
	format is SGPPF_A8R8G8B8, it is of uint32. Be sure to call unlock() after
	you don't need the pointer any more. */
	virtual void* lock() = 0;

	//! Unlock function.
	/** Should be called after the pointer received by lock() is not
	needed anymore. */
	virtual void unlock() = 0;

	//! Returns width and height of image data.
	virtual const SDimension2D& getDimension() const = 0;

	//! Returns bits per pixel.
	virtual uint32 getBitsPerPixel() const = 0;

	//! Returns bytes per pixel
	virtual uint32 getBytesPerPixel() const = 0;

	//! Returns image data size in bytes
	virtual uint32 getImageDataSizeInBytes() const = 0;

	//! Returns image data size in pixels
	virtual uint32 getImageDataSizeInPixels() const = 0;

	//! Returns a pixel
	virtual Colour getPixel(uint32 x, uint32 y) const = 0;

	//! Sets a pixel
	virtual void setPixel(uint32 x, uint32 y, const Colour &_color, bool blend = false ) = 0;

	//! Returns the color format
	virtual SGP_PIXEL_FORMAT getColorFormat() const = 0;

	//! Returns mask for red value of a pixel
	virtual uint32 getRedMask() const = 0;

	//! Returns mask for green value of a pixel
	virtual uint32 getGreenMask() const = 0;

	//! Returns mask for blue value of a pixel
	virtual uint32 getBlueMask() const = 0;

	//! Returns mask for alpha value of a pixel
	virtual uint32 getAlphaMask() const = 0;

	//! Returns pitch of image
	virtual uint32 getPitch() const =0;

	//! Copies the image into the target, scaling the image to fit
	virtual void copyToScaling(void* target, uint32 width, uint32 height, SGP_PIXEL_FORMAT format=SGPPF_A8R8G8B8, uint32 pitch=0) = 0;

	//! Copies the image into the target, scaling the image to fit
	virtual void copyToScaling(ISGPImage* target) = 0;

	//! copies this surface into another
	virtual void copyTo(ISGPImage* target, const SDimension2D& pos) = 0;

	//! copies this surface into another
	virtual void copyTo(ISGPImage* target, const SDimension2D& pos, const SRect& sourceRect, const SRect* clipRect=0) = 0;

	//! fills the surface with given color
	virtual void fill(const Colour &_color) = 0;

	//! return if ISGPImage is DDS data?
	virtual bool IsDDSImage() = 0;

	//! return if ISGPImage is PVRTC data?
	virtual bool IsPVRTCImage() = 0;

	//! get the amount of Bits per Pixel of the given color format
	static uint32 getBitsPerPixelFromFormat(const SGP_PIXEL_FORMAT format)
	{
		switch(format)
		{
		case SGPPF_A1R5G5B5:
			return 16;
		case SGPPF_R5G6B5:
			return 16;
		case SGPPF_A4R4G4B4:
			return 16;
		case SGPPF_R8G8B8:
			return 24;
		case SGPPF_A8R8G8B8:
			return 32;
		case SGPPF_R16F:
			return 16;
		case SGPPF_G16R16F:
			return 32;
		case SGPPF_A16B16G16R16F:
			return 64;
		case SGPPF_R32F:
			return 32;
		case SGPPF_G32R32F:
			return 64;
		case SGPPF_A32B32G32R32F:
			return 128;
		default:
			return 0;
		}
	}

	//! test if the color format is only viable for RenderTarget textures
	/** Since we don't have support for e.g. floating point ISGPImage formats
	one should test if the color format can be used for arbitrary usage, or
	if it is restricted to RTTs. */
	static bool isRenderTargetOnlyFormat(const SGP_PIXEL_FORMAT format)
	{
		switch(format)
		{
			case SGPPF_A1R5G5B5:
			case SGPPF_R5G6B5:
			case SGPPF_R8G8B8:
			case SGPPF_A8R8G8B8:
			case SGPPF_A4R4G4B4:
				return false;
			default:
				return true;
		}
	}

};

#endif		// __SGP_IMAGE_HEADER__