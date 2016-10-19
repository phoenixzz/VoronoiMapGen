#ifndef __SGP_IMAGELOADERTGA_HEADER__
#define __SGP_IMAGELOADERTGA_HEADER__

// byte-align structures
#include "../../sgp_core/common/sgp_PackStruct.h"

// these structs are also used in the TGA writer
struct SGPTGAHeader
{
	uint8 IdLength;
	uint8 ColorMapType;
	uint8 ImageType;
	uint8 FirstEntryIndex[2];
	uint16 ColorMapLength;
	uint8 ColorMapEntrySize;
	uint8 XOrigin[2];
	uint8 YOrigin[2];
	uint16 ImageWidth;
	uint16 ImageHeight;
	uint8 PixelDepth;
	uint8 ImageDescriptor;
} PACK_STRUCT;

struct SGPTGAFooter
{
	uint32 ExtensionOffset;
	uint32 DeveloperOffset;
	char   Signature[18];
} PACK_STRUCT;

// Default alignment
#include "../../sgp_core/common/sgp_UnPackStruct.h"



/*!
	Surface Loader for targa images
*/
class CSGPImageLoaderTGA : public ISGPImageLoader
{
public:

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const String& filename) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(File* file);

	//! creates a surface from the file
	virtual ISGPImage* loadImage(File* file);

private:

	//! loads a compressed tga. Was written and sent in by Jon Pry, thank you very much!
	uint8* loadCompressedImage(File* file, const SGPTGAHeader& header, int64 position) const;
};


#endif		// __SGP_IMAGELOADERTGA_HEADER__