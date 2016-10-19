#ifndef __SGP_OPENGLES2WORLDSYSTEMMANAGER_HEADER__
#define __SGP_OPENGLES2WORLDSYSTEMMANAGER_HEADER__

class COpenGLES2WorldSystemManager : public ISGPWorldSystemManager
{
public:
    COpenGLES2WorldSystemManager(COpenGLES2RenderDevice* pRenderDevice, Logger* pLogger);
	virtual ~COpenGLES2WorldSystemManager();

public:
	inline COpenGLES2RenderDevice*	GetDevice() 
	{ 
		return m_pRenderDevice;
	}
	inline virtual const CSGPTerrain* getTerrain()
	{
		return m_pTerrain;
	}
	inline virtual const CSGPSkyDome* getSkydome()
	{
		return m_pSkydome;
	}
	inline virtual const CSGPWorldSun* getWorldSun()
	{
		return m_pWorldSun;
	}
	inline virtual const CSGPWater* getWater()
	{
		return m_pWater;
	}
	inline virtual const CSGPGrass* getGrass()
	{
		return m_pGrass;
	}
	inline virtual CSGPWorldMap* getWorldMap()
	{
		return m_pWorldMap;
	}


	// create world sun data
	virtual void createWorldSun();

	// set world sun new position in sky
	virtual void setWorldSunPosition( float fSunPosition );

	// create terrain from scratch, usually used from Editor
	virtual void createTerrain( SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight );

	// create / release skydome from MF1 file
	virtual void createSkydome( const String& skydomeMF1FileName );
	virtual void releaseSkydome();
	
	// set skydome texture
	virtual void setSkydomeTexture( const String& SkyDomeColudTextureName, const String& SkyDomeNoiseTextureName );

	// create water in the world with height
	virtual void createWater( float fWaterHeight, const String& WaterWaveTextureName );
	// set one terrain chunk with water
	virtual void addWaterChunk( int32 terrainChunkIndex );
	// whether need to render the water
	virtual bool needRenderWater();


	// create grass in the world 
	virtual void createGrass(const String& grassTextureName, uint16 TexAtlasX, uint16 TexAtlasY);
	// set one GrassCluster Data into world
	virtual void setGrassCluster(float fPosX, float fPosZ, const SGPGrassCluster& ClusterData);



	// whether the current TerrainChunk is visible
	virtual bool isTerrainChunkVisible(CSGPTerrainChunk* pChunk);

	// create scene object
	virtual ISGPObject* createObject( const String& MF1FileNameStr, const String& SceneObjectName,
		const Vector3D& vPos = Vector3D(0,0,0), const Vector3D& vRotationXYZ = Vector3D(0,0,0),
		float fScale = 1.0f, float fAlpha = 1.0f ) { return NULL; }

	// scene object Interface
	virtual void getAllSceneBuilding(Array<ISGPObject*>& BuildingObjectArray);
	virtual ISGPObject* getSceneObjectByName(const char* pSceneObjectNameStr);
	virtual void getAllSceneObjectByName(const char* pSceneObjectNameStr, Array<ISGPObject*>& SceneObjectArray);
	virtual ISGPObject* getSceneObjectBySceneID( uint32 nSceneObjectID );
	virtual CStaticMeshInstance* getMeshInstanceBySceneID( uint32 nSceneObjectID );

	// add SceneObject to world
	// If scene object has been exist, do nothing
	virtual void addSceneObject( ISGPObject* obj, uint32 ConfigIndex = 0 ) {}
	// After SceneObject pos/rot/scale in the world, refresh ISGPObject data
	virtual bool refreshSceneObject( ISGPObject* obj );
	// remove SceneObject from world
	virtual void deleteSceneObject( ISGPObject* obj ) {}


	// create Quad Tree
	virtual void initializeQuadTree();
	// create CollisionSet
	virtual void initializeCollisionSet() {}

	// get terrain real height and Normal from terrain
	virtual float getTerrainHeight(float positionX, float positionZ);
	virtual float getRealTerrainHeight(float positionX, float positionZ);
	virtual Vector3D getTerrainNormal(float positionX, float positionZ);

	// update world info according to camera info
	virtual void updateWorld(float fDeltaTimeInSecond);

	// load world info from map files
	//	\param bLoadObjs	create and load Scene objects from World Map (default is true, when in Editor, it should be false)
	virtual void loadWorldFromFile(const String& WorkingDir, const String& WorldMapFileName, bool bLoadObjs = true);
	
	// save world info to map files
	virtual void saveWorldToFile(const String& WorkingDir, const String& WorldMapFileName) {}

	// release world info ( include QuadTree and terrain ) (not include scene objects)
	virtual void shutdownWorld();

	// render whole world to build Terrain renderbatch
	virtual void renderWorld();


public:
	//==============================================================================
	// Editor Interface Function
	//==============================================================================
	// Change heightmap value in Terrain
	//			o------>X
	//			|
	//			|
	//			V Z
	//	\param index_x index_z heightmap index X-Z to change
	//	\param iHeight new value for heightmap
	// return true : heightmap be changed, else return false
	virtual bool setHeightMap(uint32 index_x, uint32 index_z, uint16 iHeight) { return false; }
	
	// After changing heightmap value in Terrain, Flush whole world and all things related to the terrain height
	//	\param pChunkIndex		Specifies terrain chunk index array
	//	\param ichunkNum		Specifies number of terrain chunk index array	
	virtual void flushTerrainHeight(uint32* pChunkIndex, uint32 ichunkNum) {}
	
	// After changing heightmap value in Terrain, Recalculate terrain normal and all things related to the terrain normal
	//	\param pChunkIndex		Specifies terrain chunk index array
	//	\param ichunkNum		Specifies number of terrain chunk index array
	virtual void flushTerrainNormal(uint32* pChunkIndex, uint32 ichunkNum) {}

	// create WorldMap from scratch, usually used from Editor
	//	\param pWorldMap			External pointer to new CSGPWorldMap class, will be delete in other place
	//	\param WorldName			new worldmap name
	//	\param terrainsize			terrain chunk size of new world
	//	\param bUsePerlinNoise		generate terrain using PerlinNoise?
	//	\param maxTerrainHeight		max terrain height
	//	\param Diffuse0TextureName	default diffuse0 texture name (layer 0)
	virtual void createNewWorld(CSGPWorldMap* &pWorldMap, const char* WorldName, SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight, const String& Diffuse0TextureName);	
	
	// Change terrain chunk textures blend value
	//	\param SrcX,SrcZ		Specifies a texel offset in the x and y direction within the texture array (left-top is 0,0)
	//	\param width,height		Specifies the width and height of the texture subimage
	//	\param pAlphaBlendData	new terrain alpha blend texture data	
	virtual void editTerrainChunkBlendTexture(uint32 SrcX, uint32 SrcZ, uint32 width, uint32 height, uint32* pAlphaBlendData) {}

	// Recalculate terrain chunk Color minimap textures according to current diffuse textures
	//	\param pChunkIndex		Specifies terrain chunk index array
	//	\param ichunkNum		Specifies number of terrain chunk index array
	// Return the least	terrain chunk Color minimap texture data
	virtual uint32* flushTerrainChunkColorMinimapTexture(uint32* pChunkIndex, uint32 ichunkNum) { return NULL; }

	// set terrain chunk textures file name
	// NOTE: After this function, You must promise corrected data in CSGPWorldMap SGPWorldMapChunkTextureNameTag and SGPWorldMapChunkTextureIndexTag
	//	\param ChunkIndex				terrain chunk index
	//	\param nLayer					which channel will be changed
	//	\param TextureName				new texture file name
	virtual void setTerrainChunkLayerTexture(uint32 ChunkIndex, ESGPTerrainChunkTexture nLayer, const String& TextureName) {}

	// Add/Remove arrays of terrain chunks with water
	//	\param pChunkIndex				terrain chunk index array
	//	\param ChunkIndexNum			how many terrain chunks will be changed
	virtual void addWaterChunkForEditor( int32 *pChunkIndex, uint32 ChunkIndexNum ) {}
	virtual void removeWaterChunkForEditor( int32 *pChunkIndex, uint32 ChunkIndexNum ) {}


	// create light object
	virtual ISGPLightObject* createLightObject( const String& LightName, SGP_LIGHT_TYPE nLightType = SGPLT_Point,
		const Vector3D& vPos = Vector3D(0,0,0), const Vector3D& vDirection = Vector3D(0,-1,0),
		float fLightSize = 0.5f, float fRange = 5.0f,
		const Colour& DiffuseColor = Colour(255, 255, 255) ) { return NULL; }

	// add Light Object to world (usually called by Editor)
	// NOTE: ISGPLightObject class must be deleted in other place
	// If light object has been exist, do nothing
	virtual void addLightObject( ISGPLightObject* lightobj ) {}

	// delete Light Object from world (usually called by Editor)
	// NOTE: ISGPLightObject class must be deleted in other place
	// If light object has not been exist, do nothing
	virtual void deleteLightObject( ISGPLightObject* lightobj ) {}

	// load other ISGPObject Array from Editor created into world (usually called by Editor)
	//	\param pObjArray				ISGPObject array
	//	\param count					how many ISGPObject will be added
	virtual void loadObjectToWorldForEditor( ISGPObject* pObjArray, uint32 count ) {}


	// After adding or deleting or changing position etc. scene object to world, 
	// Recalculate terrain chunk bounding box and all things related to the Quadtree
	//	\param pObjArray		Specifies ISGPObject array
	//	\param iObjNum			Specifies number of scene object array
	//	\param bRemove			Specifies whether or not ISGPObject array be deleted
	// return true : all scene objects be changed, else return false
	virtual bool flushSceneObject( ISGPObject* pObjArray, uint32 iObjNum, bool bRemove=false ) { return false; }

	// update terrain Lightmap texture
	//	\param fProgress		Specifies working progress
	//	\param pRandom			Specifies Random class
	// NOTE: return pointer are alloced within this function, it should be free in other place
	virtual uint32* updateTerrainLightmapTexture( float &fProgress, Random* pRandom ) { return NULL; }

	// update one scene object Lightmap texture
	//	\param fProgress					Specifies working progress
	//	\param pObjArray					Specifies one ISGPObject 
	//	\param LMTexWidth nLMTexHeight		Light map texture width and height for this object
	//	\param pRandom						Specifies Random class	
	// NOTE: return pointer are alloced within this function, it should be free in other place
	virtual uint32* updateSceneObjectLightmapTexture( float &fProgress, ISGPObject* pSceneObj, uint32 nLMTexWidth, uint32 nLMTexHeight, Random* pRandom ) { return NULL; }

	// get all lights which illuminate the scene object ( object is within light's Range )
	//	\param LightObjectArray				return Light Object Array 
	//	\param pSceneObj					Specifies one ISGPObject 
	virtual void getAllIlluminatedLight(Array<ISGPLightObject*>& LightObjectArray, ISGPObject* pSceneObj) {}
	
	// get all visible scene object
	virtual inline Array<ISGPObject*>& getVisibleObjectArray() { return m_VisibleSceneObjectArray; }

private:
	void initializeTerrainRenderer(bool bLoadFromMap = false);
	void releaseTerrainRenderer();

	void getVisibleSceneObjectArray(const Frustum& ViewFrustum, const Array<CSGPTerrainChunk*>& VisibleChunkArray, Array<ISGPObject*>& VisibleSceneObjectArray);
	void initializeWaterRenderer();

private:
	COpenGLES2RenderDevice*			m_pRenderDevice;
	Logger*							m_pLogger;

	CSGPQuadTree					m_QuadTree;

	CSGPWorldMap*					m_pWorldMap;
	CSGPTerrain*					m_pTerrain;
	CSGPSkyDome*					m_pSkydome;
	CSGPWorldSun*					m_pWorldSun;
	CSGPWater*						m_pWater;
	CSGPGrass*						m_pGrass;

	uint8*							m_pWorldMapRawMemoryAddress;

	Array<ISGPObject*>				m_VisibleSceneObjectArray;
	Array<CSGPTerrainChunk*>		m_VisibleChunkArray;
	Array<CSGPTerrainChunk*>		m_WaterMirrorVisibleChunkArray;

};

#endif		// __SGP_OPENGLES2WORLDSYSTEMMANAGER_HEADER__