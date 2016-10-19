#ifndef __SGP_IMAGELOADERPVRTC_HEADER__
#define __SGP_IMAGELOADERPVRTC_HEADER__

/*!
	Surface Loader for PVRTC images
*/
class CSGPImageLoaderPVRTC : public ISGPImageLoader
{
public:
	CSGPImageLoaderPVRTC() {}

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".pvr")
	virtual bool isALoadableFileExtension(const String& filename) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(File* file);

	//! creates a surface from the file
	virtual ISGPImage* loadImage(File* file);
};


#endif		// __SGP_IMAGELOADERPVRTC_HEADER__