#ifndef __SGP_WORLDMAP_HEADER__
#define __SGP_WORLDMAP_HEADER__


#pragma pack(push, packing)
#pragma pack(1)

struct SGPWorldMapChunkTextureNameTag
{
	char m_ChunkTextureFileName[128];
};

struct SGPWorldMapChunkTextureIndexTag
{
	int32 m_ChunkTextureIndex[eChunk_NumTexture];

	SGPWorldMapChunkTextureIndexTag()
	{
		// Default base diffuse texture index is always 0
		memset(m_ChunkTextureIndex, -1, eChunk_NumTexture * sizeof(int32));
		m_ChunkTextureIndex[eChunk_Diffuse0Texture] = 0;
	}
};

// World Map Grass Data Struct
struct SGPWorldMapGrassTag
{
	struct SGPWorldMapChunkGrassClusterTag
	{
		uint32 m_nChunkIndex;
		SGPGrassCluster m_GrassLayerData[SGPTT_TILENUM * SGPTGD_GRASS_DIMISION * SGPTT_TILENUM * SGPTGD_GRASS_DIMISION];
	};

	char m_GrassTextureName[128];
	uint16 m_TextureAtlas[2];

	float m_fGrassFarFadingStart;
	float m_fGrassFarFadingEnd;

	float m_vWindDirectionAndStrength[4];
	float m_fGrassPeriod;

	uint32 m_nChunkGrassClusterNum;
	SGPWorldMapChunkGrassClusterTag** m_ppChunkGrassCluster;

	SGPWorldMapGrassTag()
	{
		m_nChunkGrassClusterNum = 0;
		m_ppChunkGrassCluster = NULL;
	}
};

struct SGPWorldMapWaterTag
{
	bool m_bHaveWater;

	char m_WaterWaveTextureName[128];
	float m_fWaterHeight;
	float m_vWaterSurfaceColor[3];
	float m_fRefractionBumpScale;
	float m_fReflectionBumpScale;
	float m_fFresnelBias;
	float m_fFresnelPow;
	float m_fWaterDepthScale;
	float m_fWaterBlendScale;
	float m_vWaveDir[2];
	float m_fWaveSpeed;
	float m_fWaveRate;
	float m_fWaterSunSpecularPower;

	int32* m_pWaterChunkIndex;
	SGPWorldMapWaterTag()
	{
		m_bHaveWater = false;
		m_fWaterHeight = 0;
		m_pWaterChunkIndex = NULL;
	}
};

struct SGPWorldMapSunSkyTag
{
	bool m_bHaveSkydome;

	char m_SkydomeMF1FileName[128];
	char m_CloudTextureFileName[128];
	char m_NoiseTextureFileName[128];

	float m_coludMoveSpeed_x;
	float m_coludMoveSpeed_z;
	float m_coludNoiseScale;
	float m_coludBrightness;

	float m_scale;// model scale
	// Atmosphere data
	float m_fHGgFunction;
	float m_fInscatteringMultiplier;
	float m_fBetaRayMultiplier;
	float m_fBetaMieMultiplier;
	float m_fSunIntensity;
	float m_fTurbitity;

	float m_fSunPosition;

	SGPWorldMapSunSkyTag() 
	{
		m_bHaveSkydome = false;
		m_fSunPosition = 0;
	}
};

struct SGPWorldConfigTag
{
	bool m_bUsingQuadTree;
	bool m_bVisibleCull;
	bool m_bUsingTerrainLOD;

	bool m_bPostFog;
	bool m_bDOF;
};



struct SGPWorldMapHeader
{
	uint32 m_iId;								//Must be 0xCAFEDBEE (magic number)
	uint32 m_iVersion;							//Must be 1
	char m_cFilename[128];						//Full filename (Relative Path to application's executable file, usually in Bin Floder)

	uint32 m_iTerrainSize;						//terrain size

	uint16 m_iTerrainMaxHeight;					//terrain heightmap max value
	uint16 m_DumpData;							//unusful Dump Data




	uint32 m_iChunkTextureNameNum;				//Number of Chunk Texture File Name String
	uint32 m_iChunkNumber;						//Number of terrain chunks (m_iTerrainSize*m_iTerrainSize)
	uint32 m_iSceneObjectNum;					//Number of Scene objects
	uint32 m_iLightObjectNum;					//Number of Light objects


	uint32 m_iChunkColorminiMapSize;			//terrain chunk ColorMinimap texture width and height
	uint32 m_iChunkAlphaTextureSize;			//terrain chunk AlphaBlend Texture width and height

	uint32 m_iHeaderSize;						//Size of this header

	uint32 m_iHeightMapOffset;					//File offset of terrain heightmap data
	uint32 m_iNormalOffset;						//File offset of terrain normal data
	uint32 m_iTangentOffset;					//File offset of terrain Tangent data
	uint32 m_iBinormalOffset;					//File offset of terrain Binormal data
	uint32 m_iChunkTextureNameOffset;			//File offset of Texture File Name
	uint32 m_iChunkTextureIndexOffset;			//File offset of chunk texture index
	uint32 m_iChunkAlphaTextureOffset;			//File offset of AlphaTexture data
	uint32 m_iChunkColorminiMapTextureOffset;	//File offset of color minimap texture data
	uint32 m_iSceneObjectOffset;				//File offset of scene object data
	uint32 m_iLightObjectOffset;				//File offset of light object data


	SGPWorldMapHeader() : m_iId(0xCAFEDBEE), m_iVersion(1), m_DumpData(0),
		m_iChunkTextureNameNum(0), m_iChunkNumber(0), m_iSceneObjectNum(0),m_iLightObjectNum(0),
		m_iChunkColorminiMapSize(0), m_iChunkAlphaTextureSize(0)
	{
		memset(m_cFilename, 0, 128);
	}
};

#pragma pack(pop, packing)





class CSGPWorldMap
{
public:
	CSGPWorldMap() : m_pTerrainHeightMap(NULL), m_pChunkTextureNames(NULL), m_pChunkTextureIndex(NULL),
		m_WorldChunkAlphaTextureData(NULL), m_WorldChunkColorMiniMapTextureData(NULL),
		m_pSceneObject(NULL), m_pLightObject(NULL)
	{}

	~CSGPWorldMap() 
	{}

	void Release()
	{
		if( m_pTerrainHeightMap )
		{
			delete [] m_pTerrainHeightMap;
			m_pTerrainHeightMap = NULL;
		}
		if( m_pTerrainNormal )
		{
			delete [] m_pTerrainNormal;
			m_pTerrainNormal = NULL;
		}
		if( m_pTerrainTangent )
		{
			delete [] m_pTerrainTangent;
			m_pTerrainTangent = NULL;
		}
		if( m_pTerrainBinormal )
		{
			delete [] m_pTerrainBinormal;
			m_pTerrainBinormal = NULL;
		}
		if( m_pChunkTextureNames )
		{
			delete [] m_pChunkTextureNames;
			m_pChunkTextureNames = NULL;
		}
		if( m_pChunkTextureIndex )
		{
			delete [] m_pChunkTextureIndex;
			m_pChunkTextureIndex = NULL;
		}
		if( m_WorldChunkAlphaTextureData )
		{
			delete [] m_WorldChunkAlphaTextureData;
			m_WorldChunkAlphaTextureData = NULL;
		}
		if( m_WorldChunkColorMiniMapTextureData )
		{
			delete [] m_WorldChunkColorMiniMapTextureData;
			m_WorldChunkColorMiniMapTextureData = NULL;
		}
		if( m_pSceneObject )
		{
			delete [] m_pSceneObject;
			m_pSceneObject = NULL;
		}
		if( m_pLightObject )
		{
			delete [] m_pLightObject;
			m_pLightObject = NULL;
		}
		if( m_WaterSettingData.m_pWaterChunkIndex )
		{
			delete [] m_WaterSettingData.m_pWaterChunkIndex;
			m_WaterSettingData.m_pWaterChunkIndex = NULL;
		}
		if( m_GrassData.m_ppChunkGrassCluster )
		{
			for( uint32 i=0; i<m_Header.m_iChunkNumber; i++ )
			{
				if( m_GrassData.m_ppChunkGrassCluster[i] )
				{
					delete m_GrassData.m_ppChunkGrassCluster[i];
					m_GrassData.m_ppChunkGrassCluster[i] = NULL;
				}
			}

			delete [] m_GrassData.m_ppChunkGrassCluster;
			m_GrassData.m_ppChunkGrassCluster = NULL;
		}		
	}

	//Load an World map file
	//Return the raw memory allocated from file
	static uint8* LoadWorldMap(CSGPWorldMap* &pOutWorldMap, const String& WorkingDir, const String& Filename);

	//Save an World map file
	bool SaveWorldMap(const String& WorkingDir, const String& szFilename);


	//This function is used when saving. 
	//In this function you transfer the pDataBuffer pointers to file offset according to DataSize.
	//the pointers are adjusted to become an offset from the beginning of the block.
	uint32 CollectPointers( void* pDataBuffer, uint32 DataSize, uint32 RawOffset = 0 );
	
	//Return recorded original pointer from Array
	//Because CollectPointers() function will adjust pointer to offset
	//Alloced memory in this pointer will miss when deleted
	//After save function, those pointers should be relocated 
	void * RelocateFirstPointers();

public:
	// File header
	SGPWorldMapHeader					m_Header;

	// terrain height map data (up to date)
	uint16*								m_pTerrainHeightMap;

	// terrain normal data (up to date)
	float*								m_pTerrainNormal;
	float*								m_pTerrainTangent;
	float*								m_pTerrainBinormal;

	// chunk texture file names (NOT up to date)
	SGPWorldMapChunkTextureNameTag*		m_pChunkTextureNames;

	// texture file names index of WorldMap for every chunk (array number is m_iChunkNumber = m_iTerrainSize*m_iTerrainSize)
	// (NOT up to date)
	SGPWorldMapChunkTextureIndexTag*	m_pChunkTextureIndex;

	// alpha blend texture raw data (NOT up to date)
	uint32*								m_WorldChunkAlphaTextureData;
	
	// color minimap texture raw data (NOT up to date)
	uint32*								m_WorldChunkColorMiniMapTextureData;

	// scene object (NOT up to date)
	ISGPObject*							m_pSceneObject;
	
	// light object
	ISGPLightObject*					m_pLightObject;

	// water chunk index (up to date)
	SGPWorldMapWaterTag					m_WaterSettingData;	

	// grass data (up to date)
	SGPWorldMapGrassTag					m_GrassData;

	// Skydome and Sun data (up to date)
	SGPWorldMapSunSkyTag				m_SkydomeData;
	
	// World Config data
	SGPWorldConfigTag					m_WorldConfigTag;
};

#endif		// __SGP_WORLDMAP_HEADER__