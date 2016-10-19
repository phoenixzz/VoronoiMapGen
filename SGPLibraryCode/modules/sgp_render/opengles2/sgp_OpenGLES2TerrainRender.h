#ifndef __SGP_OPENGLES2TERRAINRENDER_HEADER__
#define __SGP_OPENGLES2TERRAINRENDER_HEADER__

class COpenGLES2TerrainRenderer
{
public:
	enum EOpenGLChunkRenderFlag
	{
		eCameraChunk = 0,						// chunk which camera is in
		eVeryHighDetailed = 1,					// chunk distance 0-1 using VeryHigh Detailed Shader
		eHighDetailed = SGPTL_LOD0_CHUNKWIDTH,	// chunk distance 2-9 using Detailed Shader
		eLowDetailed,							// chunk distance >= 10 using LowDetailed shader
	};

/*
	enum EOpenGLChunkPosition
	{
		eChunk_CENTER,

		eChunk_UP,
		eChunk_BOTTOM,
		eChunk_LEFT,
		eChunk_RIGHT,
		eChunk_LEFTUP,
		eChunk_RIGHTUP,
		eChunk_LEFTBOTTOM,
		eChunk_RIGHTBOTTOM,

		eChunk_SIDE,
	};
*/
	enum EOpenGLChunkLOD
	{
		eChunk_LOD0,
		eChunk_LOD0TOLOD1,
		eChunk_LOD1TOLOD0,
		eChunk_LOD1,
	};



	struct OpenGLChunkRenderInfo
	{
		GLuint					nVBOID;					// Chunk vertex VBO
		GLuint					nVAOID;					// Chunk vertex VAO


		uint32					ChunkTextureID[eChunk_NumTexture];	// max used 9 textures

		uint32					nIndexOffset;			// Vertex index buffer offset
		uint32					nIndexCount;			// Vertex index count
		EOpenGLChunkRenderFlag	nRenderFlag;			// Terrain chunk render level
		//float					fFirstInDistance;		// Camera to chunk center distance when chunk first in blending STATE
		//float					fChunkHeightBlend;		// Terrain chunk LOD height Blend
		EOpenGLChunkLOD			nLODLevel;				// current chunk LOD level
		Vector4D				vCamPosWithBlendWidth;	// Camera postion and chunk blend width ( x,y is camera postion X-Z coord; z is start blending distance; w is blending width )


		bool					bUseDetailMap;			// current chunk using Detail map?
		//bool					bUseSlopeMap;			// current chunk using Slope map?
		//bool					bUseTriplanarTex;		// current chunk using Triplanar Texturing?
		//bool					bUseNormalMap;			// current chunk using Normal map?
	};

	COpenGLES2TerrainRenderer(COpenGLES2RenderDevice *pRenderDevice);
	~COpenGLES2TerrainRenderer();


	void setTerrainSize(uint32 terrainsize);
	void createChunkVBO(uint32 chunkindex);

	void releaseChunkVBO(uint32 chunkindex);
	void renderTerrainChunk(uint32 chunkindex);
	void updateChunkLODInfo(uint32 chunkindex, const Vector4D &ChunkCenter);
	void setChunkTextures(uint32 chunkindex, uint8 textureslot, const String& texname);
	void setChunkTextures(uint32 chunkindex, uint8 textureslot, uint32 SGPtextureID);

	// Register terrain chunk textures from CSGPWorldMap struct
	void createChunkTextureFromWorldMap(const CSGPWorldMap* pWorldMap, uint32 chunkindex);

	void registerColorMinimapTexture(const CSGPWorldMap* pWorldMap, const String& MinimapTexName, uint32 terrainsize);
	void unRegisterColorMinimapTexture(const String& MinimapTexName);
	void registerBlendTexture(const CSGPWorldMap* pWorldMap, const String& BlendTexName, uint32 terrainsize);
	void unregisterBlendTexture(const String& BlendTexName);

	void registerLightmapTexture(const String& WorldMapName, const String& LightmapTexName);
	void unregisterLightmapTexture();
	uint32 getLightmapTextureID() { return m_TerrainChunkLightMapTexID; }


	void DoDrawTerrainRenderBatch();
	void DoDrawReflectionTerrainRenderBatch();
	void AfterDrawTerrainRenderBatch();

	inline uint32 getTerrainSize() { return m_nTerrainSize; }
	inline uint32 getVeryDetailedChunkNumber() { return m_nVeryDetailedChunkNumber; }
	inline uint32 getLOD0ChunkNumber() { return m_nLOD0ChunkNumber; }
	inline uint32 getLOD1ChunkNumber() { return m_nLOD1ChunkNumber; }
	inline uint32 getLODBlendChunkNumber() { return m_nLODBlendChunkNumber; }


	//	\param pMinimapData				Raw color minimap texture data for whole worldmap (left-top is 0,0)
	//	\param terrainsize				current terrain size
	//	\param chunkRow chunkCol		chunk index row and col
	void generateChunkColorMinimapData(uint32* pMinimapData, uint32 terrainsize, uint32 chunkRow, uint32 chunkCol);

private:

	void createChunkLODInfo(uint32 chunkindex);
	
	//EOpenGLChunkPosition getChunkPosition(float fDistance_X, float fDistance_Z );



private:
	COpenGLES2RenderDevice*			m_pRenderDevice;

	GLuint							m_nChunkIndexVBO;

	uint32							m_TerrainChunkLightMapTexID;
	uint32							m_nVeryDetailedChunkNumber;
	uint32							m_nLOD0ChunkNumber;
	uint32							m_nLOD1ChunkNumber;
	uint32							m_nLODBlendChunkNumber;
	uint32							m_nTerrainSize;
public:
	OwnedArray<OpenGLChunkRenderInfo> m_TerrainChunkRenderArray;

	Array<uint32>					m_VeryDetailedChunkArrayID;
	Array<uint32>					m_LOD0ChunkArrayID;
	Array<uint32>					m_LOD1ChunkArrayID;
	Array<uint32>					m_LODBlendChunkArrayID;
};


#endif		// __SGP_OPENGLES2TERRAINRENDER_HEADER__