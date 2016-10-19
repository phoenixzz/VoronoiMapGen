#ifndef __SGP_TERRAINCHUNK_HEADER__
#define __SGP_TERRAINCHUNK_HEADER__


struct SGPGrassCluster;

struct SGPTerrainHeight
{
	float fHeight;
};

struct SGPTerrainVertex
{
	float  x, y, z, w;			// z = lod0 hight ; w = lod1 height
	float  fNormal[3];
	float  tu0, tv0;
	float  tu1, tv1;
	float  fTangent[3];
	float  fBinormal[3];
};

enum ESGPTerrainChunkTexture
{
	eChunk_Diffuse0Texture = 0,			// 0: Chunk Diffuse Texture layer0
	eChunk_Diffuse1Texture,				// 1: Chunk Diffuse Texture layer1
	eChunk_Diffuse2Texture,				// 2: Chunk Diffuse Texture layer2
	eChunk_Diffuse3Texture,				// 3: Chunk Diffuse Texture layer3
	eChunk_AlphaTexture,				// 4: Chunk alpha Blend Texture
	eChunk_NormalMapTexture,			// 5: Chunk normal map texture
	eChunk_DetailMapTexture,			// 6: Chunk detail map texture
	eChunk_SlopeMapTexture,				// 7: Chunk Slope Texture
	eChunk_MiniColorMapTexture,			// 8: Chunk mini color map texture

	eChunk_NumTexture,
};

class SGP_API CSGPTerrainChunk
{
public:
	CSGPTerrainChunk(uint8 x, uint8 z, uint16 index, CSGPTerrain* pTerrain);
	~CSGPTerrainChunk() 
	{ 
		Shutdown(); 
	}

	void Shutdown();

	void UpdateAABB();
	void AddSceneObject(const ISGPObject* pObj);
	void RemoveSceneObject(const ISGPObject* pObj);

	// Calculate the normal vectors for the chunk
	void CreateChunkNormalTable();

	// Load Chunk Normal Tangent or Binormal vectors from memory
	void SetChunkNormalTable(const float* pNormalData, const float* pTangentData, const float* pBinormalData);
	// Get Chunk Normal Tangent or Binormal vectors from terrain chunk to outer memory
	void GetChunkNormalTable(float* pNormalData, float* pTangentData, float* pBinormalData);

	// Create some height in different LOD level for this chunk
	void CreateLODHeights();

	// Get terrain height from this chunk
	// IN param offsetx offsetz: the offset value from Left-Top of the chunk
	// return terrain height in this chunk
	// For example: (0,0) will be Chunk Left-Top height; 
	// (SGPTT_TILENUM*SGPTT_TILE_METER, SGPTT_TILENUM*SGPTT_TILE_METER) will be chunk Right-Bottom height
	float GetTerrainHeight(float offsetx, float offsetz);
	// Get real terrain height from this chunk
	// IN param Pos_x Pos_z: the x,z position in world space
	float GetRealTerrainHeight(float Pos_x, float Pos_z, float offsetx, float offsetz);

	// Get terrain Normal vector from this chunk
	// IN param offsetx offsetz: the offset value from Left-Top of the chunk
	Vector3D GetTerrainNormal(float offsetx, float offsetz) const;



	// Load Chunk Grass data from memory
	void SetChunkGrassCluster(SGPGrassCluster* pRawGrassData, uint32 nDataNum);

	// Set grass data from this terrain chunk in position (usually used for Editor)
	// IN param offsetx offsetz: the offset value from Left-Top of the chunk
	void SetChunkGrassClusterInPosition(float offsetx, float offsetz, const SGPGrassCluster& RawGrassData);

	// Get grass data from this terrain chunk
	inline const SGPGrassCluster* GetGrassClusterData() 
	{ return m_pGrassLayerData; }

	inline uint32 GetGrassClusterDataCount()
	{ return m_nGrassLayerDataNum; }


	inline uint32 GetTriangleCount() 
	{ return m_TerrainTriangleCount + m_ObjectTriangleCount; }

	inline uint32 GetVertexCount() const
	{ return (SGPTT_TILENUM+1)*(SGPTT_TILENUM+1); }

	inline Vector4D GetChunkCenter()
	{
		const uint32 VertIndex = (SGPTT_TILENUM+1)*(SGPTT_TILENUM/2)+(SGPTT_TILENUM/2);
		return Vector4D( m_ChunkTerrainVertex[ VertIndex ].x, m_ChunkTerrainVertex[ VertIndex ].y,
			m_ChunkTerrainVertex[ VertIndex ].z, m_ChunkTerrainVertex[ VertIndex ].w );
	}

	inline uint16 GetTerrainChunkIndex() { return m_TerrainChunkIndex; }

	inline Array<ISGPObject*>& GetTerrainChunkObject() { return m_ChunkObjects; }

	
	//==============================================================================
	// Editor Interface Function
	//==============================================================================
	// when heightmap in terrain changed, flush terrain chunk
	bool FlushTerrainChunkHeight();

	void AddTriangleCollisionSet(CollisionSet& coll);

public:
	AABBox					m_BoundingBox;

	SGPTerrainHeight		m_ChunkTerrainHeight[(SGPTT_TILENUM+1)*(SGPTT_TILENUM+1)];
	SGPTerrainVertex		m_ChunkTerrainVertex[(SGPTT_TILENUM+1)*(SGPTT_TILENUM+1)];

private:
	// Calculate the normal, tangent, and binormal vectors for the chunk
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(const SGPTerrainVertex& vertex1, const SGPTerrainVertex& vertex2, const SGPTerrainVertex& vertex3, Vector3D& tangent, Vector3D& binormal);

	uint8					m_ChunkIndex_x;
	uint8					m_ChunkIndex_z;
	uint16					m_TerrainChunkIndex;

	uint32					m_TerrainTriangleCount;
	uint32					m_ObjectTriangleCount;


	SGPGrassCluster*		m_pGrassLayerData;			// Grass Cluster Data Array
	uint32					m_nGrassLayerDataNum;		// Grass Cluster Data number


	CSGPTerrain*			m_pTerrain;

	Array<ISGPObject*>		m_ChunkObjects;
				
};

#endif			// __SGP_TERRAINCHUNK_HEADER__