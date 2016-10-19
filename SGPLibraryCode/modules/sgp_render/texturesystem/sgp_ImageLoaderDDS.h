#ifndef __SGP_IMAGELOADERDDS_HEADER__
#define __SGP_IMAGELOADERDDS_HEADER__


/* dds definition */
enum SGP_DDSPixelFormat
{
	DDS_PF_ARGB8888,
	DDS_PF_RGB888,
	DDS_PF_RGB565,
	DDS_PF_RGB555A1,
	DDS_PF_DXT1,
	DDS_PF_DXT2,
	DDS_PF_DXT3,
	DDS_PF_DXT4,
	DDS_PF_DXT5,
	DDS_PF_UNKNOWN
};

// ddsBuffer.pixelFormat.flags
#define DDPF_ALPHAPIXELS            0x00000001 
#define DDPF_FOURCC                 0x00000004 
#define DDPF_INDEXED                0x00000020 
#define DDPF_RGB                    0x00000040 


#define DDS_COMPLEX					0x00000008
#define DDS_CUBEMAP					0x00000200
#define DDS_VOLUME					0x00200000

/* 16bpp stuff */
#define DDS_LOW_5		0x001F;
#define DDS_MID_6		0x07E0;
#define DDS_HIGH_5		0xF800;
#define DDS_MID_555		0x03E0;
#define DDS_HI_555		0x7C00;

// byte-align structures
#include "../../sgp_core/common/sgp_PackStruct.h"

/* structures */
struct ddsColorKey
{
	uint32		colorSpaceLowValue;
	uint32		colorSpaceHighValue;
} PACK_STRUCT;

struct ddsCaps
{
	uint32		caps1;
	uint32		caps2;
	uint32		caps3;
	uint32		caps4;
} PACK_STRUCT;

struct ddsMultiSampleCaps
{
	uint16		flipMSTypes;
	uint16		bltMSTypes;
} PACK_STRUCT;


struct ddsPixelFormat
{
	uint32		size;
	uint32		flags;
	uint32		fourCC;
	union
	{
		uint32	rgbBitCount;
		uint32	yuvBitCount;
		uint32	zBufferBitDepth;
		uint32	alphaBitDepth;
		uint32	luminanceBitCount;
		uint32	bumpBitCount;
		uint32	privateFormatBitCount;
	};
	union
	{
		uint32	rBitMask;
		uint32	yBitMask;
		uint32	stencilBitDepth;
		uint32	luminanceBitMask;
		uint32	bumpDuBitMask;
		uint32	operations;
	};
	union
	{
		uint32	gBitMask;
		uint32	uBitMask;
		uint32	zBitMask;
		uint32	bumpDvBitMask;
		ddsMultiSampleCaps	multiSampleCaps;
	};
	union
	{
		uint32	bBitMask;
		uint32	vBitMask;
		uint32	stencilBitMask;
		uint32	bumpLuminanceBitMask;
	};
	union
	{
		uint32	rgbAlphaBitMask;
		uint32	yuvAlphaBitMask;
		uint32	luminanceAlphaBitMask;
		uint32	rgbZBitMask;
		uint32	yuvZBitMask;
	};
} PACK_STRUCT;


struct ddsBuffer
{
	/* magic: 'dds ' */
	char		magic[ 4 ];

	/* directdraw surface */
	uint32		size;
	uint32		flags;
	uint32		height;
	uint32		width;
	union
	{
		int32	pitch;
		uint32	linearSize;
	};
	uint32		backBufferCount;
	union
	{
		uint32	mipMapCount;
		uint32	refreshRate;
		uint32	srcVBHandle;
	};
	uint32		alphaBitDepth;
	uint32		reserved;
	void		*surface;
	union
	{
		ddsColorKey	ckDestOverlay;
		uint32		emptyFaceColor;
	};
	ddsColorKey		ckDestBlt;
	ddsColorKey		ckSrcOverlay;
	ddsColorKey		ckSrcBlt;
	union
	{
		ddsPixelFormat	pixelFormat;
		uint32			fvf;
	};
	ddsCaps			caps;
	uint32			textureStage;

	/* data (Varying size) */
	uint8			data[ 4 ];
} PACK_STRUCT;


struct ddsColorBlock
{
	uint16		colors[ 2 ];
	uint8		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlockExplicit
{
	uint16		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlock3BitLinear
{
	uint8		alpha0;
	uint8		alpha1;
	uint8		stuff[ 6 ];
} PACK_STRUCT;


struct ddsColor
{
	uint8		r, g, b, a;
} PACK_STRUCT;

// Default alignment
#include "../../sgp_core/common/sgp_UnPackStruct.h"

/*!
	Surface Loader for DDS images
*/
class CSGPImageLoaderDDS : public ISGPImageLoader
{
public:
	CSGPImageLoaderDDS() : m_bCubemap(false), m_bVolume(false) {}

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".dds")
	virtual bool isALoadableFileExtension(const String& filename) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(File* file);

	//! creates a surface from the file
	virtual ISGPImage* loadImage(File* file);


private:
	void DDSDecodePixelFormat( ddsBuffer *dds, SGP_DDSPixelFormat *pf );
	int32 DDSGetInfo( ddsBuffer *dds, int32 *width, int32 *height, SGP_DDSPixelFormat *pf );

private:
	bool	m_bCubemap;
	bool	m_bVolume;
};

#endif		// __SGP_IMAGELOADERDDS_HEADER__