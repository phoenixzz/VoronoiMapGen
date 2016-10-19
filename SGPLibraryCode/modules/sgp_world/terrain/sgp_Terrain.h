#ifndef __SGP_TERRAIN_HEADER__
#define __SGP_TERRAIN_HEADER__

/*
	+z
	_________________________________
	|_0_|_1_|_2_|_3_|_4_|_5_|_6_|_7_|		terrain chunk index
	|_8_|_9_|_10|_11|_12|_13|_14|_15|		( for example one 8 * 8 chunks grid )
	|_16|_17|_18|_19|_20|_21|_22|_23|
	|_24|_25|_26|_27|_28|_29|_30|_31|
	|_32|_33|_34|_35|_36|_37|_38|_39|
	|_40|_41|_42|_43|_44|_45|_46|_47|
	|_48|_49|_50|_51|_52|_53|_54|_55|
	|_56|_57|_58|_59|_60|_61|_62|_63|
	__________________________________+x
  0,0

*/

/*
	+z						
0,0 |________			the layout of heightmap data
	|		|
	|		|
	|_______|___  +x
			1,1
*/

/*
	0   1    2	  3    4    5    6    7    8
	*---**---**---**---**---**---**---**---*
	| \ || \ || / || / || \ || \ || / || / |		terrain vertex grid in one chunk
   9*---**---**---**---**---**---**---**---*17		Row : 9 vertex	
	| \ || \ || / || / || \ || \ || / || / |		Col : 9 vertex
  18*---**---**---**---**---**---**---**---*26		
	| / || / || \ || \ || / || / || \ || \ |
  27*---**---**---**---**---**---**---**---*35
	| / || / || \ || \ || / || / || \ || \ |
  36*---**---**---**---**---**---**---**---*44
	| \ || \ || / || / || \ || \ || / || / |
  45*---**---**---**---**---**---**---**---*53
	| \ || \ || / || / || \ || \ || / || / |
  54*---**---**---**---**---**---**---**---*62
	| / || / || \ || \ || / || / || \ || \ |
  63*---**---**---**---**---**---**---**---*71
	| / || / || \ || \ || / || / || \ || \ |
  72*---**---**---**---**---**---**---**---*80
*/


// Default terrain size in SGP Engine
enum SGP_TERRAIN_SIZE
{
	SGPTS_SMALL = 16,			// small terrain is 16 * 16 chunks
	SGPTS_MEDIUM = 32,			// medium terrain is 32 * 32 chunks
	SGPTS_LARGE = 64,			// large terrain is 64 * 64 chunks
};

// Default terrain LOD num in SGP Engine
// Max 2 lods (0-1)
// 0 : max detailed mesh
// 1 : min detailed mesh
enum SGP_TERRAIN_LOD
{
	SGPTL_LOD0 = 0,
	SGPTL_LOD1 = 1,


	SGPTL_LOD0_CHUNKWIDTH = 10,



	SGPTL_LOD0_TRIANGLESINCHUNK = 8*8*2,
	SGPTL_LOD1_TRIANGLESINCHUNK = 4*4*2,

};

// Default terrain tile num of one terrain chunk in SGP Engine
enum SGP_TERRAIN_TILENUM
{
	SGPTT_TILENUM = 8,
};

// Default terrain tile meter in SGP Engine
enum SGP_TERRAIN_TILE_METER
{
	SGPTT_TILE_METER = 2,
};

// Default terrain Grass dimision in SGP Engine
enum SGP_TERRAIN_GRASS_DIMISION
{
	SGPTGD_GRASS_DIMISION = 4,
};

// Default terrain texture blend-alpha dimision in SGP Engine
enum SGP_TERRAIN_BLENDTEXTURE_DIMISION
{
	SGPTBD_BLENDTEXTURE_DIMISION = 2,
};

// Default terrain lightmap texture dimision in SGP Engine
enum SGP_TERRAIN_LIGHTMAPTEXTURE_DIMISION
{
	SGPTLD_LIGHTMAPTEXTURE_DIMISION = 8,
};







struct SGPGrassCluster;
class CSGPTerrainChunk;

class SGP_API CSGPTerrain
{
public:
	CSGPTerrain() : m_terrainChunkSize(SGPTS_SMALL), m_heightMap(NULL), m_TerrainMaxHeight(255.0f)
	{
		m_TerrainChunks.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE);
	}

	~CSGPTerrain()
	{
		Shutdown();
	}

	inline void Shutdown()
	{
		if( m_heightMap )
		{
			delete [] m_heightMap;
			m_heightMap = NULL;
		}

		m_TerrainChunks.clear( true );
	}

	// Create terrain heightmap from scratch ( usually called from Editor )
	// IN param ChunkSize: terrain size
	// IN param bPerlinNoise: Whether to use PerlinNoise?
	// IN param MaxHeight: Max terrain height(0-65535)
	void InitializeCreateHeightmap(SGP_TERRAIN_SIZE ChunkSize, bool bPerlinNoise, uint16 MaxHeight);
	
	// Load terrain heightmap from map data in memory
	// IN param ChunkSize: terrain size
	// IN param pHeightData: heightmap data in memory
	// IN param MaxHeight: Max terrain height(0-65535)
	void LoadCreateHeightmap(SGP_TERRAIN_SIZE ChunkSize, const uint16* pHeightData, uint16 MaxHeight);
	
	// Load terrain normal from map data in memory
	// IN param pNormalData: Normal vector data in memory
	// IN param pTangentData: Tangent vector data in memory
	// IN param pBinormalData: Binormal vector data in memory
	void LoadCreateNormalTable(const float* pNormalData, const float* pTangentData, const float* pBinormalData);
	
	// save terrain normal data to outer memory
	// IN param pNormalData: Normal vector data in outer memory
	// IN param pTangentData: Tangent vector data in outer memory
	// IN param pBinormalData: Binormal vector data in outer memory
	void SaveNormalTable(float* pNormalData, float* pTangentData, float* pBinormalData);

	// Get terrain chunk index according to Position in world space
	// IN param Pos_x Pos_z: the x,z position in world space
	// If the position is ouside of terrain, return 0xFFFFFFFF
	uint32 GetChunkIndex(float Pos_x, float Pos_z);

	// Get terrain chunk index according to Position in world space
	// IN param Pos_x Pos_z : the x,z position in world space
	// OUT param index_x index_z : the index of X/Z of this chunk
	// If the position is ouside of terrain, return false
	bool GetChunkIndex(uint8& index_x, uint8& index_z, float Pos_x, float Pos_z);

	// Get terrain height from this terrain
	// IN param Pos_x Pos_z: the x,z position in world space
	// If the position is ouside of terrain, return 0
	float GetTerrainHeight(float Pos_x, float Pos_z);
	float GetRealTerrainHeight(float Pos_x, float Pos_z);

	// Get terrain Normal from this terrain
	// IN param Pos_x Pos_z: the x,z position in world space
	// If the position is ouside of terrain, return (0,0,0)
	Vector3D GetTerrainNormal(float Pos_x, float Pos_z) const;


	// Set Grass layer data into this terrain
	// IN param Pos_x Pos_z: the x,z position in world space
	// If the position is ouside of terrain, do nothing 
	void SetTerrainGrassLayerData(float Pos_x, float Pos_z, const SGPGrassCluster& GrassData);

	// Update Terrain chunk AABB according to terrain vertex
	void UpdateBoundingBox();

	// Add Scene object to this terrain chunk ( update Chunk AABB, also save itself in chunk )
	void AddSceneObject( const ISGPObject* pObj, uint32 chunkIndex );
	// Remove Scene object from this terrain chunk ( update Chunk AABB and triangle count also remove itself from chunk )
	void RemoveSceneObject( const ISGPObject* pObj, uint32 chunkIndex );

	// Create some height in different LOD level for this terrain
	void CreateLODHeights();

	// Create terrain normal
	void CreateNormalTable();


	inline float GetTerrainWidth() const
	{
		return float(m_terrainChunkSize * SGPTT_TILE_METER * SGPTT_TILENUM);
	}

	// Get terrain center in world space (X-Z planes)
	inline Vector2D GetTerrainCenter()
	{
		return Vector2D(GetTerrainWidth(), GetTerrainWidth()) * 0.5f;
	}

	inline uint32 GetTerrainChunkSize()						{ return m_terrainChunkSize; }

	inline const uint16* GetHeightMap()						{ return m_heightMap; }
	// in order to speed, not to do security checks here
	inline void SetHeightMap(uint32 index, uint16 iHeight)	{ m_heightMap[index] = iHeight; }

	inline float GetTerrainMaxHeight()						{ return m_TerrainMaxHeight; }
	inline void SetTerrainMaxHeight(float height)			{ m_TerrainMaxHeight = height; }

	inline uint32 GetVertexCount()
	{
		return (m_terrainChunkSize*SGPTT_TILENUM + 1) * (m_terrainChunkSize*SGPTT_TILENUM + 1);
	}

private:
	void GeneratePerlinNoiseToHeightmap(float scale, int32 octaves, float falloff);

	void AverageChunkBorderNormal_Horizontal(uint32 chunkIdx0, uint32 chunkIdx1);
	void AverageChunkBorderNormal_Vertical(uint32 chunkIdx0, uint32 chunkIdx1);
	void NormalizeAllChunkNormal();

	void GenTerrainChunks();

	



private:
	SGP_TERRAIN_SIZE	m_terrainChunkSize;
	uint16*				m_heightMap;
	float				m_TerrainMaxHeight;


public:
	OwnedArray<CSGPTerrainChunk> m_TerrainChunks;
};

#endif			// __SGP_TERRAIN_HEADER__