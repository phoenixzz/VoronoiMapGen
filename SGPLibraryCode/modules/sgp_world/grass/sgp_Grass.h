#ifndef __SGP_GRASS_HEADER__
#define __SGP_GRASS_HEADER__

#pragma pack(push, packing)
#pragma pack(1)

/*
	Grass Cluster Data Struct:
	XXXXXXXX			--------			--------					--------
	 size				UVindex			windRandomoffsetX			windRandomoffsetZ
	 0-255				0-255				0-255							0-255
*/
struct SGPGrassCluster
{
	float fPositionX, fPositionY, fPositionZ;	// Position in world space
	uint32 nPackedNormal;						// Packed Normal Vector in world space (X8Y8Z8|08)
	uint32 nData;								// Grass Cluster Packed data
};

#pragma pack(pop, packing)


class SGP_API CSGPGrass
{
public:
	CSGPGrass() : m_fGrassPeriod(0.01f), m_vWindDirectionAndStrength(0.8f, 0, -0.6f, 0.5f)
	{ 
		m_TerrainGrassChunks.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE);
	}
	~CSGPGrass() 
	{
	}

	void UpdateTerrainGrassChunks(CSGPTerrain* pTerrain);


	inline void SetGrassTextureName( const String& grassTex ) { m_GrassTextureName = grassTex; }
	inline const String& GetGrassTextureName() { return m_GrassTextureName; }

	inline void SetAtlasDimensions(uint16 nbX, uint16 nbY)
	{
		m_fTextureAtlasNbX = nbX;
		m_fTextureAtlasNbY = nbY;
		m_fTextureAtlasW = 1.0f / nbX;
		m_fTextureAtlasH = 1.0f / nbY;
	}

public:
	Array<CSGPTerrainChunk*>	m_TerrainGrassChunks;			// terrain chunks having grass

	// texture atlas info
	uint16						m_fTextureAtlasNbX;
	uint16						m_fTextureAtlasNbY;
	float						m_fTextureAtlasW;
	float						m_fTextureAtlasH;

	Vector4D					m_vWindDirectionAndStrength;	// wind direction (no need normalized, large will move more) and wind strength
	float						m_fGrassPeriod;					// grass movement period with wind (large will move more)

private:
	String						m_GrassTextureName;				// world grass texture file name



	SGP_DECLARE_NON_COPYABLE (CSGPGrass)
};


#endif		// __SGP_GRASS_HEADER__