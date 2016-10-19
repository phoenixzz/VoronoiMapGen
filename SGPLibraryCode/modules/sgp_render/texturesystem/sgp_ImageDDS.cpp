

SGPImageDDS::SGPImageDDS( uint8* FileInMem, uint8* data, int32 width, int32 height, 
	int32 cubemap, int32 depth,	uint32 mipMapCount,
	bool compressed, bool swap,	int32 divSize,
	uint32 blockBytes, uint32 internalFormat, uint32 externalFormat, uint32 type )
{
	pFileInMemory = FileInMem;

	bVolume = (depth > 1);
	bCubemap = (cubemap > 1);
	bCompressed = compressed;
	bSwap = swap;
	iExternalFormat = externalFormat;
	iInternalFormat = internalFormat;
	PixelType = type;


	if( compressed )
	{
		// load all surfaces for the image (6 surfaces for cubemaps)
		for (int n = 0; n < cubemap; n++)
		{
			CubeMapData.add(data);

			int32 size = jmax( divSize, width )/divSize * jmax( divSize, height )/divSize * blockBytes * depth;

			int32 MipWidth = width;
			int32 MipHeight = height;
			int32 MipDepth = depth;

			// Mipmap 0
			MipmapData.add(data);
			MipmapDataBytes.add(size);	
			MipmapDepth.add(MipDepth);	
			MipmapSizes.add(SDimension2D(MipWidth, MipHeight));

			for( uint32 ix = 1; (ix < mipMapCount) && (MipWidth || MipHeight); ++ix ) 
			{	
				data += size;

				MipWidth = (MipWidth+1) >> 1;
				MipHeight = (MipHeight+1) >> 1;
				MipDepth = (MipDepth+1) >> 1;
				size = jmax( divSize, MipWidth )/divSize * jmax( divSize, MipHeight )/divSize * blockBytes * MipDepth;

				MipmapData.add(data);
				MipmapDataBytes.add(size);	
				MipmapDepth.add(MipDepth);	
				MipmapSizes.add(SDimension2D(MipWidth, MipHeight));
			}
			data += size;
		}
    }
	else
	{
		// load all surfaces for the image (6 surfaces for cubemaps)
		for (int n = 0; n < cubemap; n++)
		{
			CubeMapData.add(data);

			int32 size = width * height * blockBytes * depth;

			int32 MipWidth = width;
			int32 MipHeight = height;
			int32 MipDepth = depth;

			// Mipmap 0
			MipmapData.add(data);
			MipmapDataBytes.add(size);	
			MipmapDepth.add(MipDepth);	
			MipmapSizes.add(SDimension2D(MipWidth, MipHeight));

			for( uint32 ix = 1; (ix < mipMapCount) && (MipWidth || MipHeight); ++ix )
			{
				data += size;

				MipWidth = (MipWidth+1)>>1;
				MipHeight = (MipHeight+1)>>1;
				MipDepth = (MipDepth+1) >> 1;
				size = MipWidth * MipHeight * blockBytes * MipDepth;

				MipmapData.add(data);
				MipmapDataBytes.add(size);	
				MipmapDepth.add(MipDepth);	
				MipmapSizes.add(SDimension2D(MipWidth, MipHeight));
			}
			data += size;
		}

	}

    // swap cubemaps on y axis (since image is flipped in OGL)
    if( bCubemap )
    {
        uint8* tmp = CubeMapData[3];
        CubeMapData.getReference(3) = CubeMapData[2];
        CubeMapData.getReference(2) = tmp;
    }

}

SGPImageDDS::~SGPImageDDS()
{
	if( pFileInMemory )
		delete [] pFileInMemory;
	pFileInMemory = NULL;
}