#ifndef __SGP_IMAGELOADER_HEADER__
#define __SGP_IMAGELOADER_HEADER__

//! Class which is able to create a image from a file.
/** If you want the SGP Engine be able to load textures of
currently unsupported file formats. */
class ISGPImageLoader
{
public:
	virtual ~ISGPImageLoader() {}
	//! Check if the file might be loaded by this class
	/** Check is based on the file extension (e.g. ".tga")
	\param filename Name of file to check.
	\return True if file seems to be loadable. */
	virtual bool isALoadableFileExtension(const String& filename) const = 0;

	//! Check if the file might be loaded by this class
	/** Check might look into the file.
	\param file File handle to check.
	\return True if file seems to be loadable. */
	virtual bool isALoadableFileFormat(File* file) = 0;

	//! Creates a surface from the file
	/** \param file File handle to check.
	\return Pointer to newly created image, or 0 upon error. */
	virtual ISGPImage* loadImage(File* file) = 0;
};



#endif		// __SGP_IMAGELOADER_HEADER__