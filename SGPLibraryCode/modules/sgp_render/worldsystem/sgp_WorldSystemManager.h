#ifndef __SGP_WORLDSYSTEMMANAGER_HEADER__
#define __SGP_WORLDSYSTEMMANAGER_HEADER__

class CStaticMeshInstance;


class ISGPWorldSystemManager
{
public:
	ISGPWorldSystemManager() : m_SceneIDToInstanceMap(INIT_SCENEOBJECTARRAYSIZE)
	{
		m_pWorldMapConfig = CSGPWorldConfig::getInstance();
		CSGPLightMapGenConfig::getInstance();

		m_SenceObjectArray.ensureStorageAllocated(INIT_SCENEOBJECTARRAYSIZE);
		m_LightObjectArray.ensureStorageAllocated(INIT_SCENEOBJECTARRAYSIZE);
	}
	virtual ~ISGPWorldSystemManager() 
	{
		CSGPWorldConfig::deleteInstance();
		CSGPLightMapGenConfig::deleteInstance();

		HashMap<uint32, CStaticMeshInstance*>::Iterator i (m_SceneIDToInstanceMap);
		while( i.next() )
		{
			CStaticMeshInstance* pInstance = i.getValue();
			pInstance->destroyModel();
			delete pInstance;
			pInstance = NULL;
		}
	}

	inline void setWorldName( const String& worldname )
	{
		m_WorldName = worldname;
	}
	inline const String& getWorldName()
	{
		return m_WorldName;
	}
	inline int getSenceObjectCount()
	{
		return m_SenceObjectArray.size();
	}


	inline bool isUsingRefractionMap()
	{
		return m_pWorldMapConfig->m_bHavingWaterInWorld;
	}

	inline bool isUsingReflectionMap()
	{
		return m_pWorldMapConfig->m_bHavingWaterInWorld;
	}

	inline bool isUsingPostProcess()
	{
		return m_pWorldMapConfig->m_bPostFog || m_pWorldMapConfig->m_bDOF;
	}

	inline bool isUsingDepthTexture()
	{
		return m_pWorldMapConfig->m_bPostFog || m_pWorldMapConfig->m_bDOF;
	}


	virtual const CSGPTerrain* getTerrain() = 0;
	virtual const CSGPSkyDome* getSkydome() = 0;
	virtual const CSGPWorldSun* getWorldSun() = 0;
	virtual const CSGPWater* getWater() = 0;
	virtual const CSGPGrass* getGrass() = 0;
	virtual CSGPWorldMap* getWorldMap() = 0;

	// create world sun data
	virtual void createWorldSun() = 0;
	// set world sun new position in sky
	virtual void setWorldSunPosition( float fSunPosition ) = 0;

	// create terrain from scratch, usually used from Editor
	virtual void createTerrain( SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight ) = 0;
	

	// create / release skydome from MF1 file
	virtual void createSkydome( const String& skydomeMF1FileName ) = 0;
	virtual void releaseSkydome() = 0;
	
	// set skydome texture
	virtual void setSkydomeTexture( const String& SkyDomeColudTextureName, const String& SkyDomeNoiseTextureName ) = 0;


	// create water in the world with height
	virtual void createWater( float fWaterHeight, const String& WaterWaveTextureName ) = 0;
	// set one terrain chunk with water
	virtual void addWaterChunk( int32 terrainChunkIndex ) = 0;
	// whether need to render the water
	virtual bool needRenderWater() = 0;	

	// whether the current TerrainChunk is visible
	virtual bool isTerrainChunkVisible(CSGPTerrainChunk* pChunk) = 0;


	// create grass in the world 
	virtual void createGrass(const String& grassTextureName, uint16 TexAtlasX, uint16 TexAtlasY) = 0;
	// set one GrassCluster Data into world
	virtual void setGrassCluster(float fPosX, float fPosZ, const SGPGrassCluster& ClusterData) = 0;



	// create scene object
	virtual ISGPObject* createObject( const String& MF1FileNameStr, const String& SceneObjectName,
		const Vector3D& vPos = Vector3D(0,0,0), const Vector3D& vRotationXYZ = Vector3D(0,0,0),
		float fScale = 1.0f, float fAlpha = 1.0f ) = 0;

	// scene object Interface
	virtual void getAllSceneBuilding(Array<ISGPObject*>& BuildingObjectArray) = 0;
	virtual ISGPObject* getSceneObjectByName(const char* pSceneObjectNameStr) = 0;
	virtual void getAllSceneObjectByName(const char* pSceneObjectNameStr, Array<ISGPObject*>& SceneObjectArray) = 0;
	virtual ISGPObject* getSceneObjectBySceneID( uint32 nSceneObjectID ) = 0;
	virtual CStaticMeshInstance* getMeshInstanceBySceneID( uint32 nSceneObjectID ) = 0;

	// add SceneObject to world (usually called by Editor)
	// NOTE: ISGPObject class must be deleted in other place
	// If scene object has been exist, do nothing
	virtual void addSceneObject( ISGPObject* obj, uint32 ConfigIndex = 0 ) = 0;
	
	// After SceneObject loading to world or change pos/rot/scale in the world, refresh ISGPObject data
	// return true : this scene object is loaded and placed in right position
	// return false: this scene object is not loaded
	virtual bool refreshSceneObject( ISGPObject* obj ) = 0;

	// remove SceneObject from world
	// NOTE: ISGPObject class must be deleted in other place
	virtual void deleteSceneObject( ISGPObject* obj ) = 0;

	// create Quad Tree
	virtual void initializeQuadTree() = 0;
	// create CollisionSet
	virtual void initializeCollisionSet() = 0;


	// get terrain real height and Normal from terrain
	virtual float getTerrainHeight(float positionX, float positionZ) = 0;
	virtual float getRealTerrainHeight(float positionX, float positionZ) = 0;
	virtual Vector3D getTerrainNormal(float positionX, float positionZ) = 0;


	// update world info according to camera info
	virtual void updateWorld( float fDeltaTimeInSecond ) = 0;

	// load world info from map files
	//	\param bLoadObjs	create and load Scene objects from World Map (default is true, when in Editor, it should be false)
	virtual void loadWorldFromFile(const String& WorkingDir, const String& WorldMapFileName, bool bLoadObjs = true) = 0;
	
	// save world info to map files
	virtual void saveWorldToFile(const String& WorkingDir, const String& WorldMapFileName) = 0;

	// release world info ( include QuadTree and terrain ) (not include scene objects)
	virtual void shutdownWorld() = 0;

	// render whole world to build Terrain renderbatch
	virtual void renderWorld() = 0;



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
	virtual bool setHeightMap(uint32 index_x, uint32 index_z, uint16 iHeight) = 0;
	
	// After changing heightmap value in Terrain, Flush changed chunk height and all things related to the terrain height
	//	\param pChunkIndex		Specifies terrain chunk index array
	//	\param ichunkNum		Specifies number of terrain chunk index array	
	virtual void flushTerrainHeight(uint32* pChunkIndex, uint32 ichunkNum) = 0;
	
	// After changing heightmap value in Terrain, Recalculate terrain normal and all things related to the terrain normal
	//	\param pChunkIndex		Specifies terrain chunk index array
	//	\param ichunkNum		Specifies number of terrain chunk index array
	virtual void flushTerrainNormal(uint32* pChunkIndex, uint32 ichunkNum) = 0;

	
	// create WorldMap from scratch, usually used from Editor
	//	\param pWorldMap			External pointer to new CSGPWorldMap class, will be delete in other place
	//	\param WorldName			new worldmap name
	//	\param terrainsize			terrain chunk size of new world
	//	\param bUsePerlinNoise		generate terrain using PerlinNoise?
	//	\param maxTerrainHeight		max terrain height
	//	\param Diffuse0TextureName	default diffuse0 texture name (layer 0)
	virtual void createNewWorld(CSGPWorldMap* &pWorldMap, const char* WorldName, SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight, const String& Diffuse0TextureName) = 0;


	// Change terrain chunk textures blend value
	//	\param SrcX,SrcZ		Specifies a texel offset in the x and y direction within the texture array (left-top is 0,0)
	//	\param width,height		Specifies the width and height of the texture subimage
	//	\param pAlphaBlendData	new terrain alpha blend texture data	
	virtual void editTerrainChunkBlendTexture(uint32 SrcX, uint32 SrcZ, uint32 width, uint32 height, uint32* pAlphaBlendData) = 0;

	// Recalculate terrain chunk Color minimap textures according to current diffuse textures
	//	\param pChunkIndex		Specifies terrain chunk index array
	//	\param ichunkNum		Specifies number of terrain chunk index array
	// Return the least	terrain chunk Color minimap texture data
	virtual uint32* flushTerrainChunkColorMinimapTexture(uint32* pChunkIndex, uint32 ichunkNum) = 0;

	// set terrain chunk textures file name
	// NOTE: After this function, You must promise corrected data in CSGPWorldMap SGPWorldMapChunkTextureNameTag and SGPWorldMapChunkTextureIndexTag
	//	\param ChunkIndex				terrain chunk index
	//	\param nLayer					which channel will be changed
	//	\param TextureName				new texture file name
	virtual void setTerrainChunkLayerTexture(uint32 ChunkIndex, ESGPTerrainChunkTexture nLayer, const String& TextureName) = 0;

	// Add/Remove arrays of terrain chunks with water
	//	\param pChunkIndex				terrain chunk index array
	//	\param ChunkIndexNum			how many terrain chunks will be changed
	virtual void addWaterChunkForEditor( int32 *pChunkIndex, uint32 ChunkIndexNum ) = 0;
	virtual void removeWaterChunkForEditor( int32 *pChunkIndex, uint32 ChunkIndexNum ) = 0;

	// LIGHTMAP GENERATION & LIGHT 
	// create light object
	virtual ISGPLightObject* createLightObject( const String& LightName, SGP_LIGHT_TYPE nLightType = SGPLT_Point,
		const Vector3D& vPos = Vector3D(0,0,0), const Vector3D& vDirection = Vector3D(0,-1,0),
		float fLightSize = 0.5f, float fRange = 5.0f,
		const Colour& DiffuseColor = Colour(255, 255, 255) ) = 0;

	// add Light Object to world (usually called by Editor)
	// NOTE: ISGPLightObject class must be deleted in other place
	// If light object has been exist, do nothing
	virtual void addLightObject( ISGPLightObject* lightobj ) = 0;

	// delete Light Object from world (usually called by Editor)
	// NOTE: ISGPLightObject class must be deleted in other place
	// If light object has not been exist, do nothing
	virtual void deleteLightObject( ISGPLightObject* lightobj ) = 0;

	// load other ISGPObject Array from Editor created into world (usually called by Editor)
	//	\param pObjArray				ISGPObject array
	//	\param count					how many ISGPObject will be added
	virtual void loadObjectToWorldForEditor( ISGPObject* pObjArray, uint32 count ) = 0;

	// After adding or deleting or changing position etc. scene object to world, 
	// Recalculate terrain chunk bounding box and all things related to the Quadtree
	//	\param pObjArray		Specifies ISGPObject array
	//	\param iObjNum			Specifies number of scene object array
	//	\param bRemove			Specifies whether or not ISGPObject array be deleted
	// return true : all scene objects be changed, else return false
	virtual bool flushSceneObject( ISGPObject* pObjArray, uint32 iObjNum, bool bRemove=false ) = 0;
	
	// update terrain Lightmap texture
	//	\param fProgress		Specifies working progress
	//	\param pRandom			Specifies Random class
	// NOTE: return pointer are alloced within this function, it should be free in other place
	virtual uint32* updateTerrainLightmapTexture( float &fProgress, Random* pRandom ) = 0;
	
	// update one scene object Lightmap texture
	//	\param fProgress					Specifies working progress
	//	\param pObjArray					Specifies one ISGPObject 
	//	\param LMTexWidth nLMTexHeight		Light map texture width and height for this object
	//	\param pRandom						Specifies Random class	
	// NOTE: return pointer are alloced within this function, it should be free in other place
	virtual uint32* updateSceneObjectLightmapTexture( float &fProgress, ISGPObject* pSceneObj, uint32 nLMTexWidth, uint32 nLMTexHeight, Random* pRandom ) = 0;

	// get all lights which illuminate the scene object ( object is within light's Range )
	//	\param LightObjectArray				return Light Object Array 
	//	\param pSceneObj					Specifies one ISGPObject 
	virtual void getAllIlluminatedLight(Array<ISGPLightObject*>& LightObjectArray, ISGPObject* pSceneObj) = 0;

	// get all visible scene object
	virtual Array<ISGPObject*>& getVisibleObjectArray() = 0;

protected:
	String									m_WorldName;			// WorldName
	CSGPWorldConfig*						m_pWorldMapConfig;		// World config setting

	Array<ISGPObject*>						m_SenceObjectArray;		// sence object Array( array index is scene obj id )
	HashMap<uint32, CStaticMeshInstance*>	m_SceneIDToInstanceMap;	// scene obj id to MeshInstance map
	Array<ISGPLightObject*>					m_LightObjectArray;		// scene light object Array( array index is light obj id )

protected:
	// Init m_SenceObjectArray Array size also m_SceneIDToInstanceMap and m_SceneNameToObjMap hash map slot number
	// (so that the array won't have to keep dynamically resizing itself as the elements are added, 
	// and it'll therefore be more efficient. )
	static const int	INIT_SCENEOBJECTARRAYSIZE = 1024;
};


#endif		// __SGP_WORLDSYSTEMMANAGER_HEADER__