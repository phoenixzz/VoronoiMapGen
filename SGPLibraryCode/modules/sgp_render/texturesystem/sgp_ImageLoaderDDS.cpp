

/*
	Based on Code from Copyright (c) 2003 Randy Reddig
	Based on code from Nvidia's DDS example:
	http://www.nvidia.com/object/dxtc_decompression_code.html

	mainly c to cpp
*/


/* endian tomfoolery */
typedef union
{
	float	f;
	char	c[ 4 ];
} floatSwapUnion;

#if defined (SGP_BIG_ENDIAN)
	int32 DDSBigLong( int32 src ) { return src; }
	int16 DDSBigShort( int16 src ) { return src; }
	float DDSBigFloat( float src ) { return src; }

	int32 DDSLittleLong( int32 src )
	{
		return ((src & 0xFF000000) >> 24) |
			((src & 0x00FF0000) >> 8) |
			((src & 0x0000FF00) << 8) |
			((src & 0x000000FF) << 24);
	}

	int16 DDSLittleShort( int16 src )
	{
		return ((src & 0xFF00) >> 8) |
			((src & 0x00FF) << 8);
	}

	float DDSLittleFloat( float src )
	{
		floatSwapUnion in,out;
		in.f = src;
		out.c[ 0 ] = in.c[ 3 ];
		out.c[ 1 ] = in.c[ 2 ];
		out.c[ 2 ] = in.c[ 1 ];
		out.c[ 3 ] = in.c[ 0 ];
		return out.f;
	}
#else
	int32   DDSLittleLong( int32 src ) { return src; }
	int16	DDSLittleShort( int16 src ) { return src; }
	float	DDSLittleFloat( float src ) { return src; }

	int32 DDSBigLong( int32 src )
	{
		return ((src & 0xFF000000) >> 24) |
			((src & 0x00FF0000) >> 8) |
			((src & 0x0000FF00) << 8) |
			((src & 0x000000FF) << 24);
	}

	int16 DDSBigShort( int16 src )
	{
		return ((src & 0xFF00) >> 8) |
			((src & 0x00FF) << 8);
	}

	float DDSBigFloat( float src )
	{
		floatSwapUnion in,out;
		in.f = src;
		out.c[ 0 ] = in.c[ 3 ];
		out.c[ 1 ] = in.c[ 2 ];
		out.c[ 2 ] = in.c[ 1 ];
		out.c[ 3 ] = in.c[ 0 ];
		return out.f;
	}

#endif



/*!
	DDSDecodePixelFormat()
	determines which pixel format the dds texture is in
*/
void CSGPImageLoaderDDS::DDSDecodePixelFormat( ddsBuffer *dds, SGP_DDSPixelFormat *pf )
{
	/* dummy check */
	if(	dds == NULL || pf == NULL )
		return;

	/* extract fourCC */
	const uint32 fourCC = dds->pixelFormat.fourCC;

	/* test it */
	if( fourCC == 0 )
	{
		if( (dds->pixelFormat.flags & DDPF_RGB) && 
			!(dds->pixelFormat.flags & DDPF_ALPHAPIXELS) &&
			(dds->pixelFormat.rgbBitCount == 24) &&
			(dds->pixelFormat.rBitMask == 0xFF0000) && 
			(dds->pixelFormat.gBitMask == 0xFF00) && 
			(dds->pixelFormat.bBitMask == 0xFF) )
		{
			*pf = DDS_PF_RGB888;
		}
		else if( (dds->pixelFormat.flags & DDPF_RGB) && 
				!(dds->pixelFormat.flags & DDPF_ALPHAPIXELS) &&
				(dds->pixelFormat.rgbBitCount == 16) &&
				(dds->pixelFormat.rBitMask == 0x0000F800) && 
				(dds->pixelFormat.gBitMask == 0x000007E0) && 
				(dds->pixelFormat.bBitMask == 0x0000001F) )
			*pf = DDS_PF_RGB565;
		else if( (dds->pixelFormat.flags & DDPF_RGB) && 
				(dds->pixelFormat.flags & DDPF_ALPHAPIXELS) &&
				(dds->pixelFormat.rgbBitCount == 16) && 
				(dds->pixelFormat.rBitMask == 0x00007c00) && 
				(dds->pixelFormat.gBitMask == 0x000003e0) && 
				(dds->pixelFormat.bBitMask == 0x0000001f) &&
				(dds->pixelFormat.rgbAlphaBitMask == 0x00008000) )
			*pf = DDS_PF_RGB555A1;
		else if( (dds->pixelFormat.flags & DDPF_RGB) && 
				(dds->pixelFormat.flags & DDPF_ALPHAPIXELS) &&
				(dds->pixelFormat.rgbBitCount == 32) && 
				(dds->pixelFormat.rBitMask == 0xFF0000) && 
				(dds->pixelFormat.gBitMask == 0xFF00) && 
				(dds->pixelFormat.bBitMask == 0xFF) && 
				(dds->pixelFormat.rgbAlphaBitMask == 0xFF000000) )
			*pf = DDS_PF_ARGB8888;
		else
			*pf = DDS_PF_UNKNOWN;
		//else if( (dds->pixelFormat.flags & DDPF_RGB) && 
		//		!(dds->pixelFormat.flags & DDPF_ALPHAPIXELS) &&
		//		(dds->pixelFormat.rgbBitCount == 32) && 
		//		(dds->pixelFormat.rBitMask == 0xFF0000) && 
		//		(dds->pixelFormat.gBitMask == 0xFF00) && 
		//		(dds->pixelFormat.bBitMask == 0xFF) )
		//{
		//	*pf = DDS_PF_XRGB888;
		//}
	}
	else if( fourCC == *((uint32*) "DXT1") )
	{
		*pf = DDS_PF_DXT1;
	}
	else if( fourCC == *((uint32*) "DXT2") )
	{
		*pf = DDS_PF_DXT2;
	}
	else if( fourCC == *((uint32*) "DXT3") )
	{
		*pf = DDS_PF_DXT3;
	}
	else if( fourCC == *((uint32*) "DXT4") )
	{
		*pf = DDS_PF_DXT4;
	}
	else if( fourCC == *((uint32*) "DXT5") )
	{
		*pf = DDS_PF_DXT5;
	}
	else
		*pf = DDS_PF_UNKNOWN;
}

/*!
DDSGetInfo()
extracts relevant info from a dds texture, returns 0 on success, -1 on fail
*/
int32 CSGPImageLoaderDDS::DDSGetInfo( ddsBuffer *dds, int32 *width, int32 *height, SGP_DDSPixelFormat *pf )
{
	/* dummy test */
	if( dds == NULL )
		return -1;

	/* test dds header */
	if( *((int32*) dds->magic) != *((int32*) "DDS ") )
		return -1;
	if( DDSLittleLong( dds->size ) != 124 )
		return -1;

	/* extract width and height */
	if( width != NULL )
		*width = DDSLittleLong( dds->width );
	if( height != NULL )
		*height = DDSLittleLong( dds->height );

	/* get pixel format */
	DDSDecodePixelFormat( dds, pf );

	/* return ok */
	return 0;
}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".dds")
bool CSGPImageLoaderDDS::isALoadableFileExtension(const String& filename) const
{
	return filename.getLastCharacters(4).compareIgnoreCase( String(".dds") ) == 0;
}


//! returns true if the file maybe is able to be loaded by this class
bool CSGPImageLoaderDDS::isALoadableFileFormat(File* file)
{
	if (!file)
		return false;

	ddsBuffer header;

	ScopedPointer <InputStream> input(file->createInputStream());
	if(input == nullptr)
    {
		return false;
	}
	input->read(&header, sizeof(header));

	int32 width, height;
	SGP_DDSPixelFormat pixelFormat;

	return (0 == DDSGetInfo( &header, &width, &height, &pixelFormat));
}


//! creates a surface from the file
ISGPImage* CSGPImageLoaderDDS::loadImage(File* file)
{
	ScopedPointer <InputStream> input(file->createInputStream());
	if(input == nullptr)
		return 0;

	const uint32 FileSize = uint32(input->getTotalLength());
	uint8 *memFile = new uint8 [ FileSize ];
	input->read ( memFile, FileSize );

	ddsBuffer* header = (ddsBuffer*) memFile;
	SGPImageDDS* image = NULL;

	// store primary surface width/height/depth
	int32 width, height, depth;
	SGP_DDSPixelFormat pixelFormat;
	bool compressed = false;
	bool swap = false;
	int32 divSize = 0;
	uint32 blockBytes = 0;
	uint32 internalFormat = 0;
	uint32 externalFormat = 0;
	uint32 type = 0;

	if( 0 == DDSGetInfo( header, &width, &height, &pixelFormat) )
	{
		// check if image is a cubemap	
		if( header->caps.caps2 & DDS_CUBEMAP )
			m_bCubemap = true;
		else
			m_bCubemap = false;

		// check if image is a volume texture
		if( (header->caps.caps2 & DDS_VOLUME) && (header->backBufferCount > 0) )
			m_bVolume = true;
		else
			m_bVolume = false;

		depth = m_bVolume ? jmax(header->backBufferCount, (uint32)1) : 1;
		int32 cubemap = m_bCubemap ? 6 : 1;
		
		uint32 mipMapCount = header->mipMapCount;

		if( pixelFormat == DDS_PF_UNKNOWN )
		{

			delete [] memFile;
			memFile = NULL;
			return image;
		}
		else if( pixelFormat == DDS_PF_DXT1 )
		{
			compressed = true;
			swap = false;
			divSize = 4;
			blockBytes = 8;
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		}
		else if( pixelFormat == DDS_PF_DXT2 || pixelFormat == DDS_PF_DXT3 )
		{
			compressed = true;
			swap = false;
			divSize = 4;
			blockBytes = 16;
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		}
		else if( pixelFormat == DDS_PF_DXT4 || pixelFormat == DDS_PF_DXT5 )
		{
			compressed = true;
			swap = false;
			divSize = 4;
			blockBytes = 16;
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		else if( pixelFormat == DDS_PF_ARGB8888 )
		{
			compressed = false;
			swap = false;
			divSize = 1;
			blockBytes = 4;
			internalFormat = GL_RGBA8;
			externalFormat = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
		}
		else if( pixelFormat == DDS_PF_RGB888 )
		{
			compressed = false;
			swap = false;
			divSize = 1;
			blockBytes = 3;
			internalFormat = GL_RGB8;
			externalFormat = GL_BGR;
			type = GL_UNSIGNED_BYTE;
		}
		else if( pixelFormat == DDS_PF_RGB555A1 )
		{
			compressed = false;
			swap = true;
			divSize = 1;
			blockBytes = 2;
			internalFormat = GL_RGB5_A1;
			externalFormat = GL_BGRA;
			type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		}
		else if( pixelFormat == DDS_PF_RGB565 )
		{
			compressed = false;
			swap = false;
			divSize = 1;
			blockBytes = 2;
			internalFormat = GL_RGB5;
			externalFormat = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_6_5;
		}

		// memFile will be stored in class SGPImageDDS
		// memFile will be free when SGPImageDDS be deleted
		image = new SGPImageDDS(memFile, (uint8*)header->data, width, height, 
			cubemap, depth, mipMapCount, 
			compressed, swap, divSize, blockBytes,
			internalFormat, externalFormat, type);
	}
	else
	{
		delete [] memFile;
		memFile = NULL;
	}

	// Malloc memFile data will be deleted when SGPImageDDS be deleted
	// Do not release it here

	return image;
}


//! creates a loader which is able to load dds images
ISGPImageLoader* createImageLoaderDDS()
{
	return new CSGPImageLoaderDDS();
}

