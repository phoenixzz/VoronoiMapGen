

SGPImagePVRTC::SGPImagePVRTC( const void* pointer, const void * const texPtr, bool bReadMetaData ) :
		m_bReadMetaData(bReadMetaData), m_pTextureData(NULL),
		m_bAllowDecompress(true), m_nLoadFromLevel(0),
		m_bIsLegacyPVR(false)
{
	m_pFileInMemory = (PVRTuint8*)pointer;

	// Texture setup
	PVRTextureHeaderV3 sTextureHeader;
	PVRTuint8* pTextureData = NULL;

	// Just in case header and pointer for decompression.
	PVRTextureHeaderV3 sTextureHeaderDecomp;


	// Check if it's an old header format
	if( (*(PVRTuint32*)pointer) != PVRTEX3_IDENT )
	{
		// Convert the texture header to the new format.
		PVRTConvertOldTextureHeaderToV3( (PVR_Texture_Header*)pointer, sTextureHeader, (m_bReadMetaData ? &m_MetaData : NULL) );

		// Get the texture data.
		pTextureData = texPtr ? (PVRTuint8*)texPtr : (PVRTuint8*)pointer + *(PVRTuint32*)pointer;

		m_bIsLegacyPVR = true;
	}
	else
	{
		// Get the header from the main pointer.
		sTextureHeader = *(PVRTextureHeaderV3*)pointer;

		// Get the texture data.
		pTextureData = texPtr ? (PVRTuint8*)texPtr : (PVRTuint8*)pointer + PVRTEX3_HEADERSIZE + sTextureHeader.u32MetaDataSize;

		if(m_bReadMetaData)
		{
			// Read in all the meta data.
			PVRTuint32 metaDataSize = 0;
			while(metaDataSize < sTextureHeader.u32MetaDataSize)
			{
				//Read the DevFourCC and advance the pointer offset.
				PVRTuint32 DevFourCC = *(PVRTuint32*)((PVRTuint8*)pointer+PVRTEX3_HEADERSIZE+metaDataSize);
				metaDataSize += sizeof(DevFourCC);

				//Read the Key and advance the pointer offset.
				PVRTuint32 u32Key = *(PVRTuint32*)((PVRTuint8*)pointer+PVRTEX3_HEADERSIZE+metaDataSize);
				metaDataSize += sizeof(u32Key);

				//Read the DataSize and advance the pointer offset.
				PVRTuint32 u32DataSize = *(PVRTuint32*)((PVRTuint8*)pointer+PVRTEX3_HEADERSIZE+metaDataSize);
				metaDataSize += sizeof(u32DataSize);

				//Get the current meta data.
				MetaDataBlock& currentMetaData = m_MetaData[DevFourCC][u32Key];

				//Assign the values to the meta data.
				currentMetaData.DevFOURCC = DevFourCC;
				currentMetaData.u32Key = u32Key;
				currentMetaData.u32DataSize = u32DataSize;
				
				//Check for data, if there is any, read it into the meta data.
				if(u32DataSize > 0)
				{
					//Allocate memory.
					currentMetaData.Data = new PVRTuint8[u32DataSize];

					//Copy the data.
					memcpy(currentMetaData.Data, ((PVRTuint8*)pointer+PVRTEX3_HEADERSIZE+metaDataSize), u32DataSize);

					//Advance the meta data size.
					metaDataSize += u32DataSize;
				}
			}
		}

	}

	// Setting the PVRTextureHeader and TextureData.
	{
		m_sTextureHeader = sTextureHeader;
		m_pTextureData = pTextureData;

		m_Size.Width = m_sTextureHeader.u32Width;
		m_Size.Height = m_sTextureHeader.u32Height;
	}
}

SGPImagePVRTC::~SGPImagePVRTC()
{
	if( m_pFileInMemory )
	{
		delete [] m_pFileInMemory;
		m_pFileInMemory = NULL;
	}
}