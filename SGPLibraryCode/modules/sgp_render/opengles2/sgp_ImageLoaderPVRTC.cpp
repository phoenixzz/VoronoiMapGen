
 bool CSGPImageLoaderPVRTC::isALoadableFileExtension(const String& filename) const
 {
	 return filename.getLastCharacters(4).compareIgnoreCase( String(".pvr") ) == 0;
 }

 bool CSGPImageLoaderPVRTC::isALoadableFileFormat(File* file)
 {
	if( !file )
		return false;

	ScopedPointer <InputStream> input(file->createInputStream());
	if(input == nullptr)
    {
		return false;
	}

	// Texture header to check against.
	PVRTextureHeaderV3 sTextureHeader;
	input->read(&sTextureHeader, sizeof(PVRTextureHeaderV3));

	// Check the first word of the file and see if it's equal to the current identifier (or reverse identifier)
	if( (*(PVRTuint32*)(&sTextureHeader) != PVRTEX_CURR_IDENT) && (*(PVRTuint32*)(&sTextureHeader) != PVRTEX_CURR_IDENT_REV) )
	{
		return false;
	}

	return true;
 }


 ISGPImage* CSGPImageLoaderPVRTC::loadImage(File* file)
 {
	SGPImagePVRTC* pimage = NULL;

	ScopedPointer <InputStream> input(file->createInputStream());
	if(input == nullptr)
		return pimage;

	
	PVRTuint32 iFileSize = (PVRTuint32)input->getTotalLength();

	PVRTuint8 * ucpBuffer = new PVRTuint8 [iFileSize];

	input->read(ucpBuffer, iFileSize);

	// Header size.
	PVRTuint32 u32HeaderSize = 0;

	// Boolean whether to byte swap the texture data or not.
	bool bSwapDataEndianness = false;

	// Texture header to check against.
	PVRTextureHeaderV3 sTextureHeader;

	// The channel type for endian swapping.
	EPVRTVariableType u32CurrentChannelType = ePVRTVarTypeUnsignedByte;

	// Check the first word of the file and see if it's equal to the current identifier (or reverse identifier)
	if( (*(PVRTuint32*)ucpBuffer != PVRTEX_CURR_IDENT) && (*(PVRTuint32*)ucpBuffer != PVRTEX_CURR_IDENT_REV) )
	{
		// Swap the header bytes if necessary.
		if( !PVRTIsLittleEndian() )
		{
			bSwapDataEndianness = true;
			PVRTuint32 u32HeaderSize = PVRTByteSwap32(*(PVRTuint32*)ucpBuffer);

			for(PVRTuint32 i=0; i<u32HeaderSize; ++i)
			{
				PVRTByteSwap( (PVRTuint8*)( ( (PVRTuint32*)ucpBuffer )+i), sizeof(PVRTuint32) );
			}
		}

		// Get a pointer to the header.
		PVR_Texture_Header* sLegacyTextureHeader = (PVR_Texture_Header*)ucpBuffer;

		// Set the header size.
		u32HeaderSize = sLegacyTextureHeader->dwHeaderSize;

		// We only really need the channel type.
		PVRTuint64 tempFormat;
		EPVRTColourSpace tempColourSpace;
		bool tempIsPreMult;

		// Map the enum to get the channel type.
		PVRTMapLegacyTextureEnumToNewFormat( (PVRTPixelType)( sLegacyTextureHeader->dwpfFlags&0xff),tempFormat,tempColourSpace, u32CurrentChannelType, tempIsPreMult);
	}
	// If the header file has a reverse identifier, then we need to swap endianness
	else if( *(PVRTuint32*)ucpBuffer == PVRTEX_CURR_IDENT_REV )
	{
		// Setup the texture header
		sTextureHeader = *(PVRTextureHeaderV3*)ucpBuffer;

		bSwapDataEndianness = true;
		PVRTextureHeaderV3* pTextureHeader = (PVRTextureHeaderV3*)ucpBuffer;

		pTextureHeader->u32ChannelType = PVRTByteSwap32(pTextureHeader->u32ChannelType);
		pTextureHeader->u32ColourSpace = PVRTByteSwap32(pTextureHeader->u32ColourSpace);
		pTextureHeader->u32Depth = PVRTByteSwap32(pTextureHeader->u32Depth);
		pTextureHeader->u32Flags = PVRTByteSwap32(pTextureHeader->u32Flags);
		pTextureHeader->u32Height = PVRTByteSwap32(pTextureHeader->u32Height);
		pTextureHeader->u32MetaDataSize = PVRTByteSwap32(pTextureHeader->u32MetaDataSize);
		pTextureHeader->u32MIPMapCount = PVRTByteSwap32(pTextureHeader->u32MIPMapCount);
		pTextureHeader->u32NumFaces = PVRTByteSwap32(pTextureHeader->u32NumFaces);
		pTextureHeader->u32NumSurfaces = PVRTByteSwap32(pTextureHeader->u32NumSurfaces);
		pTextureHeader->u32Version = PVRTByteSwap32(pTextureHeader->u32Version);
		pTextureHeader->u32Width = PVRTByteSwap32(pTextureHeader->u32Width);
		PVRTByteSwap( (PVRTuint8*)&pTextureHeader->u64PixelFormat, sizeof(PVRTuint64) );

		// Channel type.
		u32CurrentChannelType = (EPVRTVariableType)pTextureHeader->u32ChannelType;

		// Header size.
		u32HeaderSize = PVRTEX3_HEADERSIZE + sTextureHeader.u32MetaDataSize;
	}
	else
	{
		// Header size.
		u32HeaderSize = PVRTEX3_HEADERSIZE + sTextureHeader.u32MetaDataSize;
	}

	// Convert the data if needed
	if( bSwapDataEndianness )
	{
		//Get the size of the variables types.
		PVRTuint32 ui32VariableSize = 0;
		switch(u32CurrentChannelType)
		{
		case ePVRTVarTypeFloat:
		case ePVRTVarTypeUnsignedInteger:
		case ePVRTVarTypeUnsignedIntegerNorm:
		case ePVRTVarTypeSignedInteger:
		case ePVRTVarTypeSignedIntegerNorm:
			{
				ui32VariableSize = 4;
				break;
			}
		case ePVRTVarTypeUnsignedShort:
		case ePVRTVarTypeUnsignedShortNorm:
		case ePVRTVarTypeSignedShort:
		case ePVRTVarTypeSignedShortNorm:
			{
				ui32VariableSize = 2;
				break;
			}
		case ePVRTVarTypeUnsignedByte:
		case ePVRTVarTypeUnsignedByteNorm:
		case ePVRTVarTypeSignedByte:
		case ePVRTVarTypeSignedByteNorm:
			{
				ui32VariableSize = 1;
				break;
			}
		default:
			break;
		}
		
		// If the size of the variable type is greater than 1, then we need to byte swap.
		if( ui32VariableSize > 1 )
		{
			//Get the texture data.
			PVRTuint8* pu8OrigData = ( (PVRTuint8*)ucpBuffer + u32HeaderSize );

			//Get the size of the texture data.
			PVRTuint32 ui32TextureDataSize = PVRTGetTextureDataSize(sTextureHeader);
				
			//Loop through and byte swap all the data. It's swapped in place so no need to do anything special.
			for(PVRTuint32 i = 0; i < ui32TextureDataSize; i += ui32VariableSize)
			{
				PVRTByteSwap(pu8OrigData+i, ui32VariableSize);
			}
		}
	}

	pimage = new SGPImagePVRTC(ucpBuffer);

	// Malloc ucpBuffer data will be deleted when SGPImagePVRTC be deleted
	// Do not release it here

	return pimage;
 }

//! creates a loader which is able to load dds images
ISGPImageLoader* createImageLoaderPVRTC()
{
	return new CSGPImageLoaderPVRTC();
}
