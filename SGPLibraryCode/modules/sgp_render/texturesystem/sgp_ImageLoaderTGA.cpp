
//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CSGPImageLoaderTGA::isALoadableFileExtension(const String& filename) const
{
	return filename.getLastCharacters(4).compareIgnoreCase( String(".tga") ) == 0;
}

//! returns true if the file maybe is able to be loaded by this class
bool CSGPImageLoaderTGA::isALoadableFileFormat(File* file)
{
	if(!file)
		return false;

	SGPTGAFooter footer;
	memset(&footer, 0, sizeof(SGPTGAFooter));


	ScopedPointer <InputStream> input(file->createInputStream());
    if(input != nullptr)
    {
		const int64 totalSize = input->getTotalLength();
		input->setPosition(totalSize - sizeof(SGPTGAFooter));
		input->read(&footer, sizeof(SGPTGAFooter));
		return (!strcmp(footer.Signature,"TRUEVISION-XFILE."));	// very old tgas are refused.
	}

	return false;
}


//! creates a surface from the file
ISGPImage* CSGPImageLoaderTGA::loadImage(File* file)
{
	SGPTGAHeader header;
	uint32 *palette = 0;

	ScopedPointer <InputStream> input(file->createInputStream());
    if(input == nullptr)
    {
		Logger::getCurrentLogger()->writeToLog( String("Can NOT open TGA file ")+file->getFullPathName(), ELL_ERROR);
		return 0;
	}
	input->read(&header, sizeof(SGPTGAHeader));

	header.ColorMapLength = ByteOrder::swapIfBigEndian(header.ColorMapLength);
	header.ImageWidth = ByteOrder::swapIfBigEndian(header.ImageWidth);
	header.ImageHeight = ByteOrder::swapIfBigEndian(header.ImageHeight);

	// skip image identification field
	if(header.IdLength)
		input->setPosition(header.IdLength + input->getPosition());

	if(header.ColorMapType)
	{
		// create 32 bit palette
		palette = new uint32[ header.ColorMapLength ];

		// read color map
		uint8 * colorMap = new uint8[header.ColorMapEntrySize/8 * header.ColorMapLength];
		input->read(colorMap,header.ColorMapEntrySize/8 * header.ColorMapLength);

		// convert to 32-bit palette
		switch ( header.ColorMapEntrySize )
		{
			case 16:
				CColorConverter::convert_A1R5G5B5toA8R8G8B8(colorMap, header.ColorMapLength, palette);
				break;
			case 24:
				CColorConverter::convert_B8G8R8toA8R8G8B8(colorMap, header.ColorMapLength, palette);
				break;
			case 32:
				CColorConverter::convert_B8G8R8A8toA8R8G8B8(colorMap, header.ColorMapLength, palette);
				break;
		}
		delete [] colorMap;
		colorMap = NULL;
	}

	// read image
	uint8* data = 0;

	if (	header.ImageType == 1 || // Uncompressed, color-mapped images.
			header.ImageType == 2 || // Uncompressed, RGB images
			header.ImageType == 3 // Uncompressed, black and white images
		)
	{
		const int32 imageSize = header.ImageHeight * header.ImageWidth * header.PixelDepth/8;
		data = new uint8[imageSize];
	  	input->read(data, imageSize);
	}
	else if(header.ImageType == 10)
	{
		// Runlength encoded RGB images
		data = loadCompressedImage(file, header, input->getPosition());
	}
	else
	{
		Logger::getCurrentLogger()->writeToLog( String("Unsupported TGA file type")+file->getFullPathName(), ELL_ERROR);
		delete [] palette;
		palette = NULL;
		return 0;
	}

	ISGPImage* image = 0;

	switch(header.PixelDepth)
	{
	case 8:
		{
			if (header.ImageType==3) // grey image
			{
				image = new (std::nothrow) SGPImageUncompressed(SGPPF_R8G8B8,
					SDimension2D(header.ImageWidth, header.ImageHeight));
				if (image)
					CColorConverter::convert8BitTo24Bit((uint8*)data,
						(uint8*)image->lock(),
						header.ImageWidth,header.ImageHeight,
						0, 0, (header.ImageDescriptor&0x20)==0);
			}
			else
			{
				image = new (std::nothrow) SGPImageUncompressed(SGPPF_A1R5G5B5,
					SDimension2D(header.ImageWidth, header.ImageHeight));
				if (image)
					CColorConverter::convert8BitTo16Bit((uint8*)data,
						(int16*)image->lock(),
						header.ImageWidth,header.ImageHeight,
						(int32*) palette, 0,
						(header.ImageDescriptor&0x20)==0);
			}
		}
		break;
	case 16:
		image = new (std::nothrow) SGPImageUncompressed(SGPPF_A1R5G5B5,
			SDimension2D(header.ImageWidth, header.ImageHeight));
		if (image)
			CColorConverter::convert16BitTo16Bit((int16*)data,
				(int16*)image->lock(), header.ImageWidth,	header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);
		break;
	case 24:
			image = new (std::nothrow) SGPImageUncompressed(SGPPF_R8G8B8,
				SDimension2D(header.ImageWidth, header.ImageHeight));
			if (image)
				CColorConverter::convert24BitTo24Bit(
					(uint8*)data, (uint8*)image->lock(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0, true);
		break;
	case 32:
			image = new (std::nothrow) SGPImageUncompressed(SGPPF_A8R8G8B8,
				SDimension2D(header.ImageWidth, header.ImageHeight));
			if (image)
				CColorConverter::convert32BitTo32Bit((int32*)data,
					(int32*)image->lock(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);
		break;
	default:
		Logger::getCurrentLogger()->writeToLog(String("Unsupported TGA format")+file->getFullPathName(), ELL_ERROR);
		break;
	}
	if (image)
		image->unlock();

	delete [] data;
	data = NULL;
	delete [] palette;
	palette = NULL;

	return image;
}






//! loads a compressed tga.
uint8* CSGPImageLoaderTGA::loadCompressedImage(File *file, const SGPTGAHeader& header, int64 position) const
{
	// This was written and sent in by Jon Pry, thank you very much!
	// I only changed the formatting a little bit.

	int32 bytesPerPixel = header.PixelDepth/8;
	int32 imageSize =  header.ImageHeight * header.ImageWidth * bytesPerPixel;
	uint8* data = new uint8[imageSize];
	int32 currentByte = 0;


	ScopedPointer <InputStream> input(file->createInputStream());
	input->setPosition(position);

	while(currentByte < imageSize)
	{
		uint8 chunkheader = 0;
		input->read(&chunkheader, sizeof(uint8));	// Read The Chunk's Header

		if(chunkheader < 128) // If The Chunk Is A 'RAW' Chunk
		{
			chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels

			input->read(&data[currentByte], bytesPerPixel * chunkheader);
			currentByte += bytesPerPixel * chunkheader;
		}
		else
		{
			// If It's An RLE Header
			chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit

			int32 dataOffset = currentByte;
			input->read(&data[dataOffset], bytesPerPixel);

			for(int32 counter = 0; counter < chunkheader; counter++)
			{
				for(int32 elementCounter=0; elementCounter < bytesPerPixel; elementCounter++)
					data[currentByte + elementCounter] = data[dataOffset + elementCounter];

				currentByte += bytesPerPixel;
			}
		}
	}

	return data;
}

//! creates a loader which is able to load tgas
ISGPImageLoader* createImageLoaderTGA()
{
	return new CSGPImageLoaderTGA();
}