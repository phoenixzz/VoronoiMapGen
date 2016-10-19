

static Array<PtrOffset> s_CollectPtrs;
static uint32 s_CurFileOffset = 0;



uint8* CSGPWorldMap::LoadWorldMap(CSGPWorldMap* &pOutWorldMap, const String& WorkingDir, const String& Filename)
{
	uint8 * ucpBuffer = 0;	
	uint32 iFileSize = 0;

	String AbsolutePath(Filename);
	// Identify by their absolute filenames if possible.
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = WorkingDir +	File::separatorString + String(Filename);
	}

	{
		ScopedPointer<FileInputStream> WorldMapFileStream( File(AbsolutePath).createInputStream() );
		if( WorldMapFileStream == nullptr )
		{
			Logger::getCurrentLogger()->writeToLog(String("Could not open Worldmap File:") + Filename, ELL_ERROR);
			return NULL;
		}
		iFileSize = (uint32)WorldMapFileStream->getTotalLength();

		ucpBuffer = new uint8 [iFileSize];

		WorldMapFileStream->read(ucpBuffer, iFileSize);
	}

	// Make sure header is valid
	pOutWorldMap = (CSGPWorldMap *)ucpBuffer;

	if(pOutWorldMap->m_Header.m_iId != 0xCAFEDBEE || pOutWorldMap->m_Header.m_iVersion != 1)
	{
		Logger::getCurrentLogger()->writeToLog(Filename + String(" is not a valid WorldMap File!"), ELL_ERROR);
		delete [] ucpBuffer;
		ucpBuffer = NULL;
		return NULL;
	}

	// terrain height map data
	{
		if( pOutWorldMap->m_Header.m_iHeightMapOffset )
			pOutWorldMap->m_pTerrainHeightMap = (uint16 *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iHeightMapOffset);
		else
			pOutWorldMap->m_pTerrainHeightMap = NULL;
	}
	// terrain normal data
	{
		if( pOutWorldMap->m_Header.m_iNormalOffset )
			pOutWorldMap->m_pTerrainNormal = (float *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iNormalOffset);
		else
			pOutWorldMap->m_pTerrainNormal = NULL;

		if( pOutWorldMap->m_Header.m_iTangentOffset )
			pOutWorldMap->m_pTerrainTangent = (float *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iTangentOffset);
		else
			pOutWorldMap->m_pTerrainTangent = NULL;

		if( pOutWorldMap->m_Header.m_iBinormalOffset )
			pOutWorldMap->m_pTerrainBinormal = (float *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iBinormalOffset);
		else
			pOutWorldMap->m_pTerrainBinormal = NULL;
	}

	// chunk texture file names
	{
		if( pOutWorldMap->m_Header.m_iChunkTextureNameOffset )
			pOutWorldMap->m_pChunkTextureNames = (SGPWorldMapChunkTextureNameTag *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iChunkTextureNameOffset);
		else
			pOutWorldMap->m_pChunkTextureNames = NULL;
	}

	// texture file names index
	{
		if( pOutWorldMap->m_Header.m_iChunkTextureIndexOffset )
			pOutWorldMap->m_pChunkTextureIndex = (SGPWorldMapChunkTextureIndexTag *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iChunkTextureIndexOffset);
		else
			pOutWorldMap->m_pChunkTextureIndex = NULL;
	}

	// alpha blend texture data
	{
		if( pOutWorldMap->m_Header.m_iChunkAlphaTextureOffset)
			pOutWorldMap->m_WorldChunkAlphaTextureData = (uint32 *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iChunkAlphaTextureOffset);
		else
			pOutWorldMap->m_WorldChunkAlphaTextureData = NULL;
	}

	// color minimap texture data
	{
		if( pOutWorldMap->m_Header.m_iChunkColorminiMapTextureOffset )
			pOutWorldMap->m_WorldChunkColorMiniMapTextureData = (uint32 *)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iChunkColorminiMapTextureOffset);
		else
			pOutWorldMap->m_WorldChunkColorMiniMapTextureData = NULL;
	}

	// Scene objects data
	{
		if( pOutWorldMap->m_Header.m_iSceneObjectOffset )
		{
			pOutWorldMap->m_pSceneObject = (ISGPObject*)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iSceneObjectOffset);
			for( uint32 i=0; i<pOutWorldMap->m_Header.m_iSceneObjectNum; i++ )
			{			
				if( pOutWorldMap->m_pSceneObject[i].m_pObjectInChunkIndex )
					pOutWorldMap->m_pSceneObject[i].m_pObjectInChunkIndex = (int32 *)(ucpBuffer + (intptr_t)pOutWorldMap->m_pSceneObject[i].m_pObjectInChunkIndex);
			}
		}
		else
			pOutWorldMap->m_pSceneObject = NULL;
	}

	// Light objects data
	{
		if( pOutWorldMap->m_Header.m_iLightObjectOffset )
		{
			pOutWorldMap->m_pLightObject = (ISGPLightObject*)(ucpBuffer + (intptr_t)pOutWorldMap->m_Header.m_iLightObjectOffset);
		}
		else
			pOutWorldMap->m_pLightObject = NULL;
	}

	// water chunk index
	{
		if( pOutWorldMap->m_WaterSettingData.m_pWaterChunkIndex )
			pOutWorldMap->m_WaterSettingData.m_pWaterChunkIndex = (int32 *)(ucpBuffer + (intptr_t)pOutWorldMap->m_WaterSettingData.m_pWaterChunkIndex);
		else
		{
			pOutWorldMap->m_WaterSettingData.m_pWaterChunkIndex = NULL;
			jassertfalse;
		}
	}

	// grass data
	{
		if( pOutWorldMap->m_GrassData.m_nChunkGrassClusterNum > 0 )
		{
			pOutWorldMap->m_GrassData.m_ppChunkGrassCluster = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag **)(ucpBuffer + (intptr_t)pOutWorldMap->m_GrassData.m_ppChunkGrassCluster);
			for( uint32 i=0; i<pOutWorldMap->m_GrassData.m_nChunkGrassClusterNum; i++ )
			{
				if( pOutWorldMap->m_GrassData.m_ppChunkGrassCluster[i] )
					pOutWorldMap->m_GrassData.m_ppChunkGrassCluster[i] = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag *)(ucpBuffer + (intptr_t)pOutWorldMap->m_GrassData.m_ppChunkGrassCluster[i]);
			}
		}
		else
			pOutWorldMap->m_GrassData.m_ppChunkGrassCluster = NULL;
	}

	return ucpBuffer;
}

bool CSGPWorldMap::SaveWorldMap(const String& WorkingDir, const String& szFilename)
{
	bool SaveResult = true;
	s_CurFileOffset = 0;
	s_CollectPtrs.clear();

	CollectPointers( &m_Header, sizeof(SGPWorldMapHeader) );

	CollectPointers( &m_pTerrainHeightMap, sizeof(uint16 *) );
	CollectPointers( &m_pTerrainNormal, sizeof(float *) );
	CollectPointers( &m_pTerrainTangent, sizeof(float *) );
	CollectPointers( &m_pTerrainBinormal, sizeof(float *) );
	CollectPointers( &m_pChunkTextureNames, sizeof(SGPWorldMapChunkTextureNameTag *) );
	CollectPointers( &m_pChunkTextureIndex, sizeof(SGPWorldMapChunkTextureIndexTag *) );
	CollectPointers( &m_WorldChunkAlphaTextureData, sizeof(uint32 *) );
	CollectPointers( &m_WorldChunkColorMiniMapTextureData, sizeof(uint32 *) );
	CollectPointers( &m_pSceneObject, sizeof(ISGPObject *) );
	CollectPointers( &m_pLightObject, sizeof(ISGPLightObject *) );
	CollectPointers( &m_WaterSettingData, sizeof(SGPWorldMapWaterTag) );
	CollectPointers( &m_GrassData, sizeof(SGPWorldMapGrassTag) );
	CollectPointers( &m_SkydomeData, sizeof(SGPWorldMapSunSkyTag) );
	CollectPointers( &m_WorldConfigTag, sizeof(SGPWorldConfigTag) );

	// terrain height map data
	{
		if( m_Header.m_iTerrainSize > 0 )
			m_Header.m_iHeightMapOffset = CollectPointers( m_pTerrainHeightMap, sizeof(uint16) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) );
	}

	// terrain normal data
	{
		if( m_Header.m_iTerrainSize > 0 )
		{
			m_Header.m_iNormalOffset = CollectPointers( m_pTerrainNormal, sizeof(float) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * 3 );
			m_Header.m_iTangentOffset = CollectPointers( m_pTerrainTangent, sizeof(float) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * 3 );
			m_Header.m_iBinormalOffset = CollectPointers( m_pTerrainBinormal, sizeof(float) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * (m_Header.m_iTerrainSize * SGPTT_TILENUM + 1) * 3 );
		}
	}

	// chunk texture file names
	{
		if( m_Header.m_iChunkTextureNameNum > 0 )
			m_Header.m_iChunkTextureNameOffset = CollectPointers( m_pChunkTextureNames, sizeof(SGPWorldMapChunkTextureNameTag) * m_Header.m_iChunkTextureNameNum );
	}

	// texture file names index
	{
		if( m_Header.m_iChunkNumber > 0 )
			m_Header.m_iChunkTextureIndexOffset = CollectPointers( m_pChunkTextureIndex, sizeof(SGPWorldMapChunkTextureIndexTag) * m_Header.m_iChunkNumber );
	}

	// alpha blend texture data
	{
		if( m_Header.m_iChunkAlphaTextureSize > 0 )
			m_Header.m_iChunkAlphaTextureOffset = CollectPointers( m_WorldChunkAlphaTextureData, sizeof(uint32) * m_Header.m_iChunkAlphaTextureSize * m_Header.m_iChunkAlphaTextureSize );
	}

	// color minimap texture data
	{
		if( m_Header.m_iChunkColorminiMapSize > 0 )
			m_Header.m_iChunkColorminiMapTextureOffset = CollectPointers( m_WorldChunkColorMiniMapTextureData, sizeof(uint32) * m_Header.m_iChunkColorminiMapSize * m_Header.m_iChunkColorminiMapSize );
	}

	// scene object data
	{
		if( m_Header.m_iSceneObjectNum > 0 )
		{
			for( uint32 i = 0; i < m_Header.m_iSceneObjectNum; i++ )
			{
				if( m_pSceneObject[i].m_iObjectInChunkIndexNum > 0 )
					m_pSceneObject[i].m_pObjectInChunkIndex = (int32 *)CollectPointers(m_pSceneObject[i].m_pObjectInChunkIndex, sizeof(int32) * m_pSceneObject[i].m_iObjectInChunkIndexNum);
			}
			m_Header.m_iSceneObjectOffset = CollectPointers( m_pSceneObject, sizeof(ISGPObject) * m_Header.m_iSceneObjectNum );
		}
	}

	// light object data
	{
		if( m_Header.m_iLightObjectNum > 0 )
		{
			m_Header.m_iLightObjectOffset = CollectPointers( m_pLightObject, sizeof(ISGPLightObject) * m_Header.m_iLightObjectNum );
		}
	}

	// water chunk index
	{
		if( m_WaterSettingData.m_pWaterChunkIndex )
			m_WaterSettingData.m_pWaterChunkIndex = (int32*)CollectPointers( m_WaterSettingData.m_pWaterChunkIndex, sizeof(int32) * m_Header.m_iChunkNumber );
	}

	// Grass data
	{
		if( m_GrassData.m_nChunkGrassClusterNum > 0 )
		{
			SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag** ppOld = m_GrassData.m_ppChunkGrassCluster;
			m_GrassData.m_ppChunkGrassCluster = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag**)CollectPointers( m_GrassData.m_ppChunkGrassCluster, sizeof(SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*) * m_GrassData.m_nChunkGrassClusterNum );
			for( uint32 i = 0; i < m_GrassData.m_nChunkGrassClusterNum; i++ )
			{
				if( ppOld[i] == NULL )
					ppOld[i] = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*)CollectPointers( ppOld[i], sizeof(SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*) );
				else
					ppOld[i] = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*)CollectPointers( ppOld[i], sizeof(SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag) );
			}
		}
	}

	//-------------------------------------------------------------
	// Identify it by the absolute filenames if possible.
	String AbsolutePath = szFilename;	
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = WorkingDir +	File::separatorString + szFilename;
	}

	ScopedPointer<FileOutputStream> fileStream( File(AbsolutePath).createOutputStream() );
	if (fileStream != nullptr && fileStream->openedOk() )
    {
		fileStream->setPosition(0);
		fileStream->truncate();

		for( int i=0; i<s_CollectPtrs.size(); i++ )
		{
			fileStream->write( (const uint8*)s_CollectPtrs[i].pDataPointer, s_CollectPtrs[i].DataSize );
		}

		fileStream->flush();
	}
	else
		SaveResult = false;
	//-------------------------------------------------------------

	// Reset Pointers - For release memory correctly
	RelocateFirstPointers();							//SGPWorldMapHeader

	(uint16*)RelocateFirstPointers();					//m_pTerrainHeightMap
	(float*)RelocateFirstPointers();					//m_pTerrainNormal
	(float*)RelocateFirstPointers();					//m_pTerrainTangent
	(float*)RelocateFirstPointers();					//m_pTerrainBinormal
	(SGPWorldMapChunkTextureNameTag*)RelocateFirstPointers();
	(SGPWorldMapChunkTextureIndexTag*)RelocateFirstPointers();
	(uint32*)RelocateFirstPointers();
	(uint32*)RelocateFirstPointers();
	(ISGPObject*)RelocateFirstPointers();
	(ISGPLightObject*)RelocateFirstPointers();
	RelocateFirstPointers();							//SGPWorldMapWaterTag
	RelocateFirstPointers();							//SGPWorldMapGrassTag
	RelocateFirstPointers();							//SGPWorldMapSunSkyTag
	RelocateFirstPointers();							//SGPWorldConfigTag

	if( m_Header.m_iTerrainSize > 0 )
		(uint16*)RelocateFirstPointers();
	if( m_Header.m_iTerrainSize > 0 )
	{
		(float*)RelocateFirstPointers();
		(float*)RelocateFirstPointers();
		(float*)RelocateFirstPointers();
	}
	if( m_Header.m_iChunkTextureNameNum > 0 )
		(SGPWorldMapChunkTextureNameTag*)RelocateFirstPointers();
	if( m_Header.m_iChunkNumber > 0 )
		(SGPWorldMapChunkTextureIndexTag*)RelocateFirstPointers();
	if( m_Header.m_iChunkAlphaTextureSize > 0 )
		(uint32*)RelocateFirstPointers();
	if( m_Header.m_iChunkColorminiMapSize > 0 )
		(uint32*)RelocateFirstPointers();
	if( m_Header.m_iSceneObjectNum > 0 )
	{
		for( uint32 i = 0; i < m_Header.m_iSceneObjectNum; i++ )
		{
			if( m_pSceneObject[i].m_iObjectInChunkIndexNum > 0 )
				m_pSceneObject[i].m_pObjectInChunkIndex = (int32 *)RelocateFirstPointers();
		}
		(ISGPObject*)RelocateFirstPointers();
	}
	if( m_Header.m_iLightObjectNum > 0 )
	{
		(ISGPLightObject*)RelocateFirstPointers();
	}

	if( m_WaterSettingData.m_pWaterChunkIndex )
		m_WaterSettingData.m_pWaterChunkIndex = (int32 *)RelocateFirstPointers();

	if( m_GrassData.m_nChunkGrassClusterNum > 0 )
	{
		m_GrassData.m_ppChunkGrassCluster = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag**)RelocateFirstPointers();
		for( uint32 i = 0; i < m_GrassData.m_nChunkGrassClusterNum; i++ )
		{
			m_GrassData.m_ppChunkGrassCluster[i] = (SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*)RelocateFirstPointers();
		}
	}
	return SaveResult;
}

















//-------------------------------------------------------------
//	In this function you transfer the pDataBuffer pointers to file offset according to DataSize.
//  the pointers are adjusted to become an offset from the beginning of the block.
//-------------------------------------------------------------
uint32 CSGPWorldMap::CollectPointers( void* pDataBuffer, uint32 DataSize, uint32 RawOffset )
{
	if( !pDataBuffer || DataSize == 0 )
		return 0;

	PtrOffset DataOffset;
	DataOffset.pDataPointer = pDataBuffer;
	DataOffset.DataSize = DataSize;
	DataOffset.RawOffset = RawOffset;
	s_CollectPtrs.add(DataOffset);

	s_CurFileOffset += DataSize;

	return s_CurFileOffset - DataSize;
}

//Return recorded original pointer from Array
void* CSGPWorldMap::RelocateFirstPointers()
{
	PtrOffset DataOffset = s_CollectPtrs.remove(0);
	return (void*)((intptr_t)DataOffset.pDataPointer - DataOffset.RawOffset);
}