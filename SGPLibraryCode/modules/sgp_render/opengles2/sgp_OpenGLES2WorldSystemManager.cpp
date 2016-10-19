

COpenGLES2WorldSystemManager::COpenGLES2WorldSystemManager(COpenGLES2RenderDevice* pRenderDevice, Logger* pLogger)
	: m_pRenderDevice(pRenderDevice), m_pLogger(pLogger), 
	  m_pWorldMap(NULL), m_pTerrain(NULL), m_pSkydome(NULL), m_pWorldSun(NULL), m_pWater(NULL), m_pGrass(NULL),
	  m_pWorldMapRawMemoryAddress(NULL)
{
	m_VisibleSceneObjectArray.ensureStorageAllocated(INIT_SCENEOBJECTARRAYSIZE);
	m_VisibleChunkArray.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE / 3);
	m_WaterMirrorVisibleChunkArray.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE / 3);
}

COpenGLES2WorldSystemManager::~COpenGLES2WorldSystemManager()
{
	if( m_pTerrain )
		delete m_pTerrain;
	m_pTerrain = NULL;
	if( m_pSkydome )
		delete m_pSkydome;
	m_pSkydome = NULL;
	if( m_pWorldSun )
		delete m_pWorldSun;
	m_pWorldSun = NULL;
	if( m_pWater )
		delete m_pWater;
	m_pWater = NULL;
	if( m_pGrass )
		delete m_pGrass;
	m_pGrass = NULL;

	if( m_pWorldMapRawMemoryAddress )
		delete [] m_pWorldMapRawMemoryAddress;
	m_pWorldMapRawMemoryAddress = NULL;
}

void COpenGLES2WorldSystemManager::createTerrain( SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight )
{
	// Create Terrain
	m_pTerrain = new CSGPTerrain();
	m_pTerrain->InitializeCreateHeightmap( terrainsize, bUsePerlinNoise, maxTerrainHeight );
	m_pTerrain->CreateLODHeights();
	m_pTerrain->UpdateBoundingBox();

	// Create VB/IB render resource for Terrain
	initializeTerrainRenderer(false);
}

void COpenGLES2WorldSystemManager::createSkydome( const String& skydomeMF1FileName )
{
	// Create Skydome
	m_pSkydome = new CSGPSkyDome();
	m_pSkydome->m_MF1FileName = skydomeMF1FileName;

	// load MF1 SkyDome static mesh
	m_pSkydome->m_SkydomeMF1ModelResourceID = m_pRenderDevice->GetModelManager()->registerModel(skydomeMF1FileName, false);

	if( m_pSkydome->m_SkydomeMF1ModelResourceID == 0xFFFFFFFF )
	{
		m_pLogger->writeToLog(String("Unable to load SkyDome MF1 File : ") + skydomeMF1FileName, ELL_ERROR);
		return;
	}

	m_pSkydome->m_bUpdateModel = true;
}

void COpenGLES2WorldSystemManager::releaseSkydome()
{
	if( m_pSkydome )
	{
		// unregister Skydome MF1 Model and Textures
		if( m_pSkydome->m_SkydomeMF1ModelResourceID != 0xFFFFFFFF )
		{
			m_pRenderDevice->GetModelManager()->unRegisterModelByID(m_pSkydome->m_SkydomeMF1ModelResourceID);
			m_pRenderDevice->getOpenGLSkydomeRenderer()->resetSkydomeStaticBuffer();
		
			for( int i=0; i<m_pSkydome->m_nSkydomeTextureID.size(); i++ )
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByID( m_pSkydome->m_nSkydomeTextureID[i] );
		}
	}
}

void COpenGLES2WorldSystemManager::createWorldSun()
{
	m_pWorldSun = new CSGPWorldSun();
}

void COpenGLES2WorldSystemManager::setWorldSunPosition( float fSunPosition )
{
	if( m_pWorldSun )
	{
		m_pWorldSun->m_fSunPosition = fSunPosition;
		m_pWorldSun->updateSunDirection();

        // Compute thetaS dependencies
		if( m_pSkydome )
		{
			Vector3D vecZenith( 0.0f, 1.0f, 0.0f );
			float thetaS = acosf( m_pWorldSun->getSunDirection() * vecZenith );       
			m_pSkydome->GetScatter().computeAttenuation(thetaS);
		}
	}
}

void COpenGLES2WorldSystemManager::setSkydomeTexture( const String& SkyDomeColudTextureName, const String& SkyDomeNoiseTextureName )
{
	if( m_pSkydome )
	{
		m_pSkydome->m_cloudTextureFileName = SkyDomeColudTextureName;
		m_pSkydome->m_noiseTextureFileName = SkyDomeNoiseTextureName;

		for( int i=0; i<m_pSkydome->m_nSkydomeTextureID.size(); i++ )
		{
			if( m_pSkydome->m_nSkydomeTextureID[i] != 0 )
				m_pRenderDevice->GetTextureManager()->unRegisterTextureByID( m_pSkydome->m_nSkydomeTextureID[i] );
		}
		m_pSkydome->m_nSkydomeTextureID.clearQuick();

		m_pSkydome->m_nSkydomeTextureID.add( m_pRenderDevice->GetTextureManager()->registerTexture( SkyDomeColudTextureName ) );
		m_pSkydome->m_nSkydomeTextureID.add( m_pRenderDevice->GetTextureManager()->registerTexture( SkyDomeNoiseTextureName ) );


		m_pRenderDevice->getOpenGLSkydomeRenderer()->updateSkydomeMaterialSkin(m_pSkydome);
	}
}



bool COpenGLES2WorldSystemManager::refreshSceneObject( ISGPObject* obj )
{
	// Load Static Mesh Resource
	CStaticMeshInstance* pStaticModel = m_SceneIDToInstanceMap[obj->getSceneObjectID()];
	if( pStaticModel->getMF1ModelResourceID() == 0xFFFFFFFF ) 
		return false;

	obj->setTriangleCount( pStaticModel->getMeshTriangleCount() );
	obj->setBoundingBox( pStaticModel->getInstanceOBBox() );
	

	// Finding scene object in which terrain chunks
	Array<int> TerrainChunkIndex;
	const OBBox& ObjBoundingBoxOBB = obj->getBoundingBox();
	AABBox ObjBoundingBoxAABB;
	ObjBoundingBoxAABB.Construct(&ObjBoundingBoxOBB);

	for( int i=0; i<m_pTerrain->m_TerrainChunks.size(); i++ )
	{
		AABBox chunkAABB = m_pTerrain->m_TerrainChunks[i]->m_BoundingBox;
		chunkAABB += Vector3D(chunkAABB.vcMax.x, ObjBoundingBoxAABB.vcMax.y, chunkAABB.vcMax.z);
		chunkAABB += Vector3D(chunkAABB.vcMin.x, ObjBoundingBoxAABB.vcMin.y, chunkAABB.vcMin.z);

		if( OBBox(&chunkAABB).Intersects(ObjBoundingBoxOBB) )
			TerrainChunkIndex.add(i);
	}

	// delete old data
	if( (obj->m_iObjectInChunkIndexNum > 0) && obj->m_pObjectInChunkIndex )
	{
		delete [] obj->m_pObjectInChunkIndex;
		obj->m_pObjectInChunkIndex = NULL;
		obj->m_iObjectInChunkIndexNum = 0;
	}

	// Create new data
	obj->m_iObjectInChunkIndexNum = TerrainChunkIndex.size();
	if( obj->m_iObjectInChunkIndexNum > 0 )
		obj->m_pObjectInChunkIndex = new int32 [obj->m_iObjectInChunkIndexNum];
	else
		obj->m_pObjectInChunkIndex = NULL;
	for( uint32 j=0; j<obj->m_iObjectInChunkIndexNum; j++ )
		obj->m_pObjectInChunkIndex[j] = TerrainChunkIndex.getUnchecked(j);

	/////////////////////////////////////////////////////////////////////////
	// Add obj to terrain chunk
	for( uint32 i=0; i<obj->getObjectInChunkNum(); i++ )
	{
		m_pTerrain->AddSceneObject( obj, obj->getObjectInChunkIndex(i) );
	}

	return true;
}





void COpenGLES2WorldSystemManager::initializeQuadTree()
{
	// First Release QuadTree then Create Quadtree
	m_QuadTree.Shutdown();
	m_QuadTree.InitializeFromTerrain(m_pTerrain);
}



float COpenGLES2WorldSystemManager::getTerrainHeight(float positionX, float positionZ)
{
	if( m_pTerrain )
		return m_pTerrain->GetTerrainHeight(positionX, positionZ);

	return 0;
}

float COpenGLES2WorldSystemManager::getRealTerrainHeight(float positionX, float positionZ)
{
	if( m_pTerrain )
		return m_pTerrain->GetRealTerrainHeight(positionX, positionZ);

	return 0;
}

Vector3D COpenGLES2WorldSystemManager::getTerrainNormal(float positionX, float positionZ)
{
	if( m_pTerrain )
		return m_pTerrain->GetTerrainNormal(positionX, positionZ);
	return Vector3D(0,0,0);
}


void COpenGLES2WorldSystemManager::loadWorldFromFile(const String& WorkingDir, const String& WorldMapFileName, bool bLoadObjs)
{
	m_pWorldMapRawMemoryAddress = CSGPWorldMap::LoadWorldMap(m_pWorldMap, WorkingDir, WorldMapFileName);

	setWorldName( File::getCurrentWorkingDirectory().getChildFile(String(m_pWorldMap->m_Header.m_cFilename)).getFileNameWithoutExtension() );

	// World Sun
	createWorldSun();

	// Skydome
	if( m_pWorldMap->m_SkydomeData.m_bHaveSkydome )
	{
		createSkydome( String(m_pWorldMap->m_SkydomeData.m_SkydomeMF1FileName) );
		setSkydomeTexture( String(m_pWorldMap->m_SkydomeData.m_CloudTextureFileName), String(m_pWorldMap->m_SkydomeData.m_NoiseTextureFileName) );
		
		// set Skydome Parameter
		m_pSkydome->m_coludMoveSpeed_x = m_pWorldMap->m_SkydomeData.m_coludMoveSpeed_x;
		m_pSkydome->m_coludMoveSpeed_z = m_pWorldMap->m_SkydomeData.m_coludMoveSpeed_z;
		m_pSkydome->m_coludNoiseScale = m_pWorldMap->m_SkydomeData.m_coludNoiseScale;
		m_pSkydome->m_coludBrightness = m_pWorldMap->m_SkydomeData.m_coludBrightness;
		m_pSkydome->SetScale(m_pWorldMap->m_SkydomeData.m_scale);

		// set Skydome atmosphere Parameter
		CSGPHoffmanPreethemScatter& scatter = m_pSkydome->GetScatter();
		scatter.m_fHGgFunction = m_pWorldMap->m_SkydomeData.m_fHGgFunction;
		scatter.m_fInscatteringMultiplier = m_pWorldMap->m_SkydomeData.m_fInscatteringMultiplier;
		scatter.m_fBetaRayMultiplier = m_pWorldMap->m_SkydomeData.m_fBetaRayMultiplier;
		scatter.m_fBetaMieMultiplier = m_pWorldMap->m_SkydomeData.m_fBetaMieMultiplier;
		scatter.m_fSunIntensity = m_pWorldMap->m_SkydomeData.m_fSunIntensity;
		scatter.m_fTurbitity = m_pWorldMap->m_SkydomeData.m_fTurbitity;
		scatter.calculateScatteringConstants();

		setWorldSunPosition(m_pWorldMap->m_SkydomeData.m_fSunPosition);
	}

	// Load terrain data and OpenGL Resource
	m_pTerrain = new CSGPTerrain();
	// Load height map
	m_pTerrain->LoadCreateHeightmap( static_cast<SGP_TERRAIN_SIZE>(m_pWorldMap->m_Header.m_iTerrainSize), m_pWorldMap->m_pTerrainHeightMap, m_pWorldMap->m_Header.m_iTerrainMaxHeight );
	m_pTerrain->CreateLODHeights();
	m_pTerrain->UpdateBoundingBox();

	// Load normal
	m_pTerrain->LoadCreateNormalTable(m_pWorldMap->m_pTerrainNormal, m_pWorldMap->m_pTerrainTangent, m_pWorldMap->m_pTerrainBinormal);

	// Create VB/IB render resource for Terrain
	initializeTerrainRenderer(true);

	// Create Scene Object Array in World
	if( bLoadObjs )
	{
		for( uint32 i=0; i<m_pWorldMap->m_Header.m_iSceneObjectNum; i++ )
		{
			ISGPObject* pObj = &(m_pWorldMap->m_pSceneObject[i]);

			if( pObj->getSceneObjectID() == (uint32)m_SenceObjectArray.size() )
			{
				m_SenceObjectArray.add(pObj);
				continue;
			}
		
			while( pObj->getSceneObjectID() != (uint32)m_SenceObjectArray.size() )
			{
				m_SenceObjectArray.add(NULL);
			}
			m_SenceObjectArray.add(pObj);
		}

		ISGPObject** pEnd = m_SenceObjectArray.end();
		for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
		{
			ISGPObject* obj = (*pBegin);
			if( !obj )
				continue;

			// create mesh instance and add it to map data struct
			CStaticMeshInstance *pStaticModel = new CStaticMeshInstance(m_pRenderDevice);
			pStaticModel->changeModel( String(obj->getMF1FileName()), obj->m_iConfigIndex );
			pStaticModel->setPosition( obj->m_fPosition[0], obj->m_fPosition[1], obj->m_fPosition[2] );
			pStaticModel->setRotationXYZ( obj->m_fRotationXYZ[0], obj->m_fRotationXYZ[1], obj->m_fRotationXYZ[2] );
			pStaticModel->setScale( obj->m_fScale );
			pStaticModel->setInstanceAlpha( obj->m_fAlpha );

			m_SceneIDToInstanceMap.set(obj->getSceneObjectID(), pStaticModel);

			// add to terrain chunk
			for( uint32 i=0; i<obj->getObjectInChunkNum(); i++ )
			{
				m_pTerrain->AddSceneObject( obj, obj->getObjectInChunkIndex(i) );
			}

			// Register object lightmap texture
			pStaticModel->registerLightmapTexture( getWorldName(), String(obj->getSceneObjectName())+String(".pvr") );
		}
	}

	// Create water for World
	if( m_pWorldMap->m_WaterSettingData.m_bHaveWater )
	{
		createWater( m_pWorldMap->m_WaterSettingData.m_fWaterHeight, String(m_pWorldMap->m_WaterSettingData.m_WaterWaveTextureName) );
		
		// set Water Parameter
		m_pWater->m_vWaterSurfaceColor.x = m_pWorldMap->m_WaterSettingData.m_vWaterSurfaceColor[0];
		m_pWater->m_vWaterSurfaceColor.y = m_pWorldMap->m_WaterSettingData.m_vWaterSurfaceColor[1];
		m_pWater->m_vWaterSurfaceColor.z = m_pWorldMap->m_WaterSettingData.m_vWaterSurfaceColor[2];
		m_pWater->m_fRefractionBumpScale = m_pWorldMap->m_WaterSettingData.m_fRefractionBumpScale;
		m_pWater->m_fReflectionBumpScale = m_pWorldMap->m_WaterSettingData.m_fReflectionBumpScale;
		m_pWater->m_fFresnelBias = m_pWorldMap->m_WaterSettingData.m_fFresnelBias;
		m_pWater->m_fFresnelPow = m_pWorldMap->m_WaterSettingData.m_fFresnelPow;
		m_pWater->m_fWaterDepthScale = m_pWorldMap->m_WaterSettingData.m_fWaterDepthScale;
		m_pWater->m_fWaterBlendScale = m_pWorldMap->m_WaterSettingData.m_fWaterBlendScale;
		m_pWater->m_vWaveDir.x = m_pWorldMap->m_WaterSettingData.m_vWaveDir[0];
		m_pWater->m_vWaveDir.y = m_pWorldMap->m_WaterSettingData.m_vWaveDir[1];
		m_pWater->m_fWaveSpeed = m_pWorldMap->m_WaterSettingData.m_fWaveSpeed;
		m_pWater->m_fWaveRate = m_pWorldMap->m_WaterSettingData.m_fWaveRate;
		m_pWater->m_fWaterSunSpecularPower = m_pWorldMap->m_WaterSettingData.m_fWaterSunSpecularPower;

		for( uint32 i=0; i<m_pWorldMap->m_Header.m_iChunkNumber; i++ )
		{
			if( m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex[i] != -1 )
				addWaterChunk( m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex[i] );
		}
	}

	// Create Water Reflection and Refraction FBO render resource for Terrain
	initializeWaterRenderer();

	// Create grass for World
	if( m_pWorldMap->m_GrassData.m_nChunkGrassClusterNum > 0 )
	{		
		createGrass( String(m_pWorldMap->m_GrassData.m_GrassTextureName), m_pWorldMap->m_GrassData.m_TextureAtlas[0], m_pWorldMap->m_GrassData.m_TextureAtlas[1] );
		
		// set Grass Parameter
		m_pGrass->m_vWindDirectionAndStrength.x = m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[0];
		m_pGrass->m_vWindDirectionAndStrength.y = m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[1];
		m_pGrass->m_vWindDirectionAndStrength.z = m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[2];
		m_pGrass->m_vWindDirectionAndStrength.w = m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[3];
		m_pGrass->m_fGrassPeriod = m_pWorldMap->m_GrassData.m_fGrassPeriod;
		
		CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart = m_pWorldMap->m_GrassData.m_fGrassFarFadingStart;
		CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd = m_pWorldMap->m_GrassData.m_fGrassFarFadingEnd;
	
		for( uint32 i=0; i<m_pWorldMap->m_GrassData.m_nChunkGrassClusterNum; i++ )
		{
			if( m_pWorldMap->m_GrassData.m_ppChunkGrassCluster[i] == NULL )
				continue;

			CSGPTerrainChunk* pGrassTerrainChunk = m_pTerrain->m_TerrainChunks[ m_pWorldMap->m_GrassData.m_ppChunkGrassCluster[i]->m_nChunkIndex ];
			
			m_pGrass->m_TerrainGrassChunks.add( pGrassTerrainChunk );
			pGrassTerrainChunk->SetChunkGrassCluster(
				m_pWorldMap->m_GrassData.m_ppChunkGrassCluster[i]->m_GrassLayerData,
				SGPTT_TILENUM * SGPTGD_GRASS_DIMISION * SGPTT_TILENUM * SGPTGD_GRASS_DIMISION );
		}
	}

	// set CSGPWorldConfig Parameter
	CSGPWorldConfig::getInstance()->m_bUsingQuadTree = m_pWorldMap->m_WorldConfigTag.m_bUsingQuadTree;
	CSGPWorldConfig::getInstance()->m_bVisibleCull = m_pWorldMap->m_WorldConfigTag.m_bVisibleCull;
	CSGPWorldConfig::getInstance()->m_bUsingTerrainLOD = m_pWorldMap->m_WorldConfigTag.m_bUsingTerrainLOD;
	CSGPWorldConfig::getInstance()->m_bPostFog = m_pWorldMap->m_WorldConfigTag.m_bPostFog;
	CSGPWorldConfig::getInstance()->m_bDOF = m_pWorldMap->m_WorldConfigTag.m_bDOF;

}



void COpenGLES2WorldSystemManager::updateWorld(float fDeltaTimeInSecond)
{
	// update all scene object firstly
	ISGPObject** pEnd = m_SenceObjectArray.end();
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin )
		{
			m_SceneIDToInstanceMap[(*pBegin)->getSceneObjectID()]->setVisible(false);
			m_SceneIDToInstanceMap[(*pBegin)->getSceneObjectID()]->update(fDeltaTimeInSecond);
		}
		// unloaded scene object need be Refreshed
		if( (*pBegin) && !(*pBegin)->m_bRefreshed )
			(*pBegin)->m_bRefreshed = refreshSceneObject( *pBegin );
	}

	// update Camera View Frustum
	Frustum ViewFrustum;
	ViewFrustum.setFrom( m_pRenderDevice->getOpenGLCamera()->m_mViewProjMatrix );

	// QUADTREE Cull and get visible terrain chunks
	m_VisibleChunkArray.clearQuick();
	m_QuadTree.GetVisibleTerrainChunk(m_QuadTree.GetRootNode(), ViewFrustum, m_VisibleChunkArray);


	// Update water
	m_pRenderDevice->getOpenGLWaterRenderer()->update(fDeltaTimeInSecond, m_pWater);

	// update water mirrored visible terrain chunks
	if( needRenderWater() )
	{
		Frustum MirroredViewFrustum;
		MirroredViewFrustum.setFrom( m_pRenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_pRenderDevice->getOpenGLCamera()->m_mProjMatrix );

		m_WaterMirrorVisibleChunkArray.clearQuick();
		m_QuadTree.GetVisibleTerrainChunk(m_QuadTree.GetRootNode(), MirroredViewFrustum, m_WaterMirrorVisibleChunkArray);

		// Appends mirrored view seeing terrain chunk at the end of m_VisibleChunkArray as long as the new terrain chunk doesn't already contain it
		CSGPTerrainChunk** pEnd = m_WaterMirrorVisibleChunkArray.end();
		for( CSGPTerrainChunk** pBegin = m_WaterMirrorVisibleChunkArray.begin(); pBegin < pEnd; pBegin++ )
		{
			m_VisibleChunkArray.addIfNotAlreadyThere( *pBegin );
		}
	}

	// Update every Visible terrain chunk
	if( m_VisibleChunkArray.size() > 0 )
	{
		CSGPTerrainChunk** pEnd = m_VisibleChunkArray.end();
		for( CSGPTerrainChunk** pBegin = m_VisibleChunkArray.begin(); pBegin < pEnd; pBegin++ )
		{
			m_pRenderDevice->getOpenGLTerrainRenderer()->updateChunkLODInfo( (*pBegin)->GetTerrainChunkIndex(), (*pBegin)->GetChunkCenter() );
		}
	}


	Vector4D CamPos;
	m_pRenderDevice->getCamreaPosition( &CamPos );
	
	// Update Sky dome
	m_pRenderDevice->getOpenGLSkydomeRenderer()->update(fDeltaTimeInSecond, m_pSkydome, CamPos);
	
	// Update Grass
	m_pRenderDevice->getOpenGLGrassRenderer()->update(fDeltaTimeInSecond, CamPos, ViewFrustum, m_pGrass);


	// get all visible Scene Object and update
	m_VisibleSceneObjectArray.clearQuick();
	getVisibleSceneObjectArray(ViewFrustum, m_VisibleChunkArray, m_VisibleSceneObjectArray);
	
	ISGPObject** pVisibleObjEnd = m_VisibleSceneObjectArray.end();
	for( ISGPObject** pVisibleObjBegin = m_VisibleSceneObjectArray.begin(); pVisibleObjBegin < pVisibleObjEnd; pVisibleObjBegin++ )
	{
		m_SceneIDToInstanceMap[(*pVisibleObjBegin)->getSceneObjectID()]->setVisible(true);
		m_SceneIDToInstanceMap[(*pVisibleObjBegin)->getSceneObjectID()]->update(fDeltaTimeInSecond);
	}
}




void COpenGLES2WorldSystemManager::shutdownWorld()
{
	m_QuadTree.Shutdown();

	releaseTerrainRenderer();
	releaseSkydome();

	m_pRenderDevice->getOpenGLWaterRenderer()->releaseWaterWaveTexture();
	m_pRenderDevice->getOpenGLGrassRenderer()->releaseGrassTexture();


	HashMap<uint32, CStaticMeshInstance*>::Iterator i (m_SceneIDToInstanceMap);
	while( i.next() )
	{
		CStaticMeshInstance* pInstance = i.getValue();
		pInstance->destroyModel();
		delete pInstance;
		pInstance = NULL;
	}
	m_SceneIDToInstanceMap.clear();
	m_SenceObjectArray.clear();
	m_LightObjectArray.clear();


	if( m_pTerrain )
		delete m_pTerrain;
	m_pTerrain = NULL;
	if( m_pSkydome )
		delete m_pSkydome;
	m_pSkydome = NULL;
	if( m_pWorldSun )
		delete m_pWorldSun;
	m_pWorldSun = NULL;
	if( m_pWater )
		delete m_pWater;
	m_pWater = NULL;
	if( m_pGrass )
		delete m_pGrass;
	m_pGrass = NULL;

	if( m_pWorldMapRawMemoryAddress )
		delete [] m_pWorldMapRawMemoryAddress;
	m_pWorldMapRawMemoryAddress = NULL;
}

void COpenGLES2WorldSystemManager::renderWorld()
{
	// Render Terrain Chunks
	if( m_VisibleChunkArray.size() > 0 )
	{
		CSGPTerrainChunk** pEnd = m_VisibleChunkArray.end();
		for( CSGPTerrainChunk** pBegin = m_VisibleChunkArray.begin(); pBegin < pEnd; pBegin++ )
		{
			m_pRenderDevice->getOpenGLTerrainRenderer()->renderTerrainChunk( (*pBegin)->GetTerrainChunkIndex() );
		}
	}

	// Render Skydome
	if( m_pSkydome )
		m_pRenderDevice->getOpenGLSkydomeRenderer()->render( m_pSkydome );

	// Render Grass
	if( m_pGrass )
		m_pRenderDevice->getOpenGLGrassRenderer()->render( m_pGrass );


	// Render Water
	if( m_pWater && needRenderWater() )
		m_pRenderDevice->getOpenGLWaterRenderer()->render( m_pWater );

	// Render Scene object
	if( m_VisibleSceneObjectArray.size() > 0 )
	{
		ISGPObject** pEnd = m_VisibleSceneObjectArray.end();
		for( ISGPObject** pBegin = m_VisibleSceneObjectArray.begin(); pBegin < pEnd; pBegin++ )
		{
			m_SceneIDToInstanceMap[(*pBegin)->getSceneObjectID()]->render();
		}
	}
}


void COpenGLES2WorldSystemManager::createWater( float fWaterHeight, const String& WaterWaveTextureName )
{
	CSGPWorldConfig::getInstance()->m_bHavingWaterInWorld = true;
	m_pWater = new CSGPWater(fWaterHeight);
	m_pWater->m_WaterWaveTextureName = WaterWaveTextureName;
	m_pRenderDevice->getOpenGLWaterRenderer()->createWaterWaveTexture( WaterWaveTextureName );
	
	// Create Water Reflection and Refraction FBO render resource for Terrain
	initializeWaterRenderer();
}

void COpenGLES2WorldSystemManager::addWaterChunk( int32 terrainChunkIndex )
{
	if( m_pWater && m_pTerrain )
	{
		if( terrainChunkIndex < m_pTerrain->m_TerrainChunks.size() )
		{
			// Update chunk AABBox when terrain chunk having water
			if( m_pWater->m_fWaterHeight > m_pTerrain->m_TerrainChunks[terrainChunkIndex]->m_BoundingBox.vcMax.y )
			{
				m_pTerrain->m_TerrainChunks[terrainChunkIndex]->m_BoundingBox += 
					Vector3D(	m_pTerrain->m_TerrainChunks[terrainChunkIndex]->m_BoundingBox.vcMax.x,
								m_pWater->m_fWaterHeight,
								m_pTerrain->m_TerrainChunks[terrainChunkIndex]->m_BoundingBox.vcMax.z );
			}
			// if WaterHeight below terrain chunk's whole boundingbox, skip this chunk
			if( m_pWater->m_fWaterHeight >= m_pTerrain->m_TerrainChunks[terrainChunkIndex]->m_BoundingBox.vcMin.y )
				m_pWater->m_TerrainWaterChunks.add( m_pTerrain->m_TerrainChunks[terrainChunkIndex] );
		}
	}
}

bool COpenGLES2WorldSystemManager::needRenderWater()
{
	return m_pRenderDevice->getOpenGLWaterRenderer()->needRenderWater();
}

bool COpenGLES2WorldSystemManager::isTerrainChunkVisible(CSGPTerrainChunk* pChunk)
{
	return m_VisibleChunkArray.contains( pChunk );
}



void COpenGLES2WorldSystemManager::createGrass(const String& grassTextureName, uint16 TexAtlasX, uint16 TexAtlasY)
{
	m_pGrass = new CSGPGrass();
	m_pGrass->SetGrassTextureName( grassTextureName );
	m_pGrass->SetAtlasDimensions(TexAtlasX, TexAtlasY);

	m_pRenderDevice->getOpenGLGrassRenderer()->initializeFromGrass(m_pGrass);
}

void COpenGLES2WorldSystemManager::setGrassCluster(float fPosX, float fPosZ, const SGPGrassCluster& ClusterData)
{
	if( m_pTerrain && m_pGrass )
	{
		uint32 nChunkIndex = m_pTerrain->GetChunkIndex(fPosX, fPosZ);
		if( nChunkIndex != 0xFFFFFFFF )
		{
			if( m_pGrass->m_TerrainGrassChunks.contains(m_pTerrain->m_TerrainChunks[nChunkIndex]) )
				m_pTerrain->SetTerrainGrassLayerData(fPosX, fPosZ, ClusterData);
			else
			{
				SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* pNewGrassClusterData = new SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag();
				memset(pNewGrassClusterData, 0, sizeof(SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag));
				pNewGrassClusterData->m_nChunkIndex = nChunkIndex;

				m_pWorldMap->m_GrassData.m_ppChunkGrassCluster[nChunkIndex] = pNewGrassClusterData;

				m_pGrass->m_TerrainGrassChunks.add( m_pTerrain->m_TerrainChunks[nChunkIndex] );
				m_pTerrain->m_TerrainChunks[nChunkIndex]->SetChunkGrassCluster(pNewGrassClusterData->m_GrassLayerData, SGPTT_TILENUM * SGPTGD_GRASS_DIMISION * SGPTT_TILENUM * SGPTGD_GRASS_DIMISION);
				setGrassCluster(fPosX, fPosZ, ClusterData);
			}
		}
	}
}



void COpenGLES2WorldSystemManager::getAllSceneBuilding(Array<ISGPObject*>& BuildingObjectArray)
{
	ISGPObject** pEnd = m_SenceObjectArray.end();	
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin && (*pBegin)->getSceneObjectType() == SGPOT_Building )
			BuildingObjectArray.add( (*pBegin) );
	}
}

ISGPObject* COpenGLES2WorldSystemManager::getSceneObjectByName(const char* pSceneObjectNameStr)
{
	ISGPObject** pEnd = m_SenceObjectArray.end();	
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin && (strcmp( (*pBegin)->m_SceneObjectName, pSceneObjectNameStr ) == 0) )
			return *pBegin;
	}
	return NULL;
}

void COpenGLES2WorldSystemManager::getAllSceneObjectByName(const char* pSceneObjectNameStr, Array<ISGPObject*>& SceneObjectArray)
{
	ISGPObject** pEnd = m_SenceObjectArray.end();	
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin && (strcmp( (*pBegin)->m_SceneObjectName, pSceneObjectNameStr ) == 0) )
			SceneObjectArray.add( *pBegin );
	}
}

ISGPObject* COpenGLES2WorldSystemManager::getSceneObjectBySceneID( uint32 nSceneObjectID )
{
	return m_SenceObjectArray[nSceneObjectID];
}

CStaticMeshInstance* COpenGLES2WorldSystemManager::getMeshInstanceBySceneID( uint32 nSceneObjectID )
{
	return m_SceneIDToInstanceMap[nSceneObjectID];
}










//==============================================================================
// Editor Interface Function
//==============================================================================

void COpenGLES2WorldSystemManager::createNewWorld(CSGPWorldMap* &pWorldMap, const char* WorldName, SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight, const String& Diffuse0TextureName)
{
	setWorldName( String(WorldName) );

	m_pWorldMap = pWorldMap;

	// HEADER
	m_pWorldMap->m_Header.m_iTerrainSize = terrainsize;
	m_pWorldMap->m_Header.m_iTerrainMaxHeight = maxTerrainHeight;
	m_pWorldMap->m_Header.m_iChunkTextureNameNum = 1;
	m_pWorldMap->m_Header.m_iChunkNumber = terrainsize * terrainsize;
	m_pWorldMap->m_Header.m_iSceneObjectNum = 0;
	m_pWorldMap->m_Header.m_iLightObjectNum = 0;


	m_pWorldMap->m_Header.m_iChunkColorminiMapSize = terrainsize * SGPTT_TILENUM;
	m_pWorldMap->m_Header.m_iChunkAlphaTextureSize = terrainsize * SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION;


	m_pWorldMap->m_Header.m_iHeightMapOffset = 0;
	m_pWorldMap->m_Header.m_iChunkTextureNameOffset = 0;
	m_pWorldMap->m_Header.m_iChunkTextureIndexOffset = 0;		
	m_pWorldMap->m_Header.m_iChunkColorminiMapTextureOffset = 0;
	m_pWorldMap->m_Header.m_iChunkAlphaTextureOffset = 0;
	m_pWorldMap->m_Header.m_iSceneObjectOffset = 0;
	m_pWorldMap->m_Header.m_iLightObjectOffset = 0;

	

	m_pWorldMap->m_Header.m_iHeaderSize = sizeof(SGPWorldMapHeader);

	// WATER DATA
	m_pWorldMap->m_WaterSettingData.m_bHaveWater = false;
	m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex = new int32 [terrainsize * terrainsize];
	memset(m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex, -1, sizeof(int32) * terrainsize * terrainsize);

	// GRASS DATA
	m_pWorldMap->m_GrassData.m_nChunkGrassClusterNum = 0;
	m_pWorldMap->m_GrassData.m_ppChunkGrassCluster = new SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* [terrainsize * terrainsize];
	memset(m_pWorldMap->m_GrassData.m_ppChunkGrassCluster, 0, sizeof(SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*) * terrainsize * terrainsize);

	// CHUNK TEXTURE
	m_pWorldMap->m_pChunkTextureNames = new SGPWorldMapChunkTextureNameTag [m_pWorldMap->m_Header.m_iChunkTextureNameNum];
	strcpy( m_pWorldMap->m_pChunkTextureNames[0].m_ChunkTextureFileName, Diffuse0TextureName.toUTF8().getAddress() );


	m_pWorldMap->m_pChunkTextureIndex = new SGPWorldMapChunkTextureIndexTag [m_pWorldMap->m_Header.m_iChunkNumber];
	for( uint32 i=0; i<m_pWorldMap->m_Header.m_iChunkNumber; i++ )
	{
		m_pWorldMap->m_pChunkTextureIndex[i].m_ChunkTextureIndex[eChunk_Diffuse0Texture] = 0;
	}

	// Raw DATA
	m_pWorldMap->m_pTerrainHeightMap = new uint16 [(terrainsize*SGPTT_TILENUM + 1) * (terrainsize*SGPTT_TILENUM + 1)];
	memset(m_pWorldMap->m_pTerrainHeightMap, 0, sizeof(uint16) * (terrainsize*SGPTT_TILENUM + 1) * (terrainsize*SGPTT_TILENUM + 1));

	m_pWorldMap->m_pTerrainNormal = new float [(terrainsize * SGPTT_TILENUM + 1) * (terrainsize * SGPTT_TILENUM + 1) * 3];
	memset(m_pWorldMap->m_pTerrainNormal, 0, sizeof(float) * (terrainsize*SGPTT_TILENUM + 1) * (terrainsize*SGPTT_TILENUM + 1) * 3);
	m_pWorldMap->m_pTerrainTangent = new float [(terrainsize * SGPTT_TILENUM + 1) * (terrainsize * SGPTT_TILENUM + 1) * 3];
	memset(m_pWorldMap->m_pTerrainTangent, 0, sizeof(float) * (terrainsize*SGPTT_TILENUM + 1) * (terrainsize*SGPTT_TILENUM + 1) * 3);
	m_pWorldMap->m_pTerrainBinormal = new float [(terrainsize * SGPTT_TILENUM + 1) * (terrainsize * SGPTT_TILENUM + 1) * 3];
	memset(m_pWorldMap->m_pTerrainBinormal, 0, sizeof(float) * (terrainsize*SGPTT_TILENUM + 1) * (terrainsize*SGPTT_TILENUM + 1) * 3);

	// TEXTURE DATA
	m_pWorldMap->m_WorldChunkAlphaTextureData = new uint32 [m_pWorldMap->m_Header.m_iChunkAlphaTextureSize * m_pWorldMap->m_Header.m_iChunkAlphaTextureSize];
	memset(m_pWorldMap->m_WorldChunkAlphaTextureData, 0xFF000000, sizeof(uint32) * m_pWorldMap->m_Header.m_iChunkAlphaTextureSize * m_pWorldMap->m_Header.m_iChunkAlphaTextureSize);

	m_pWorldMap->m_WorldChunkColorMiniMapTextureData = new uint32 [m_pWorldMap->m_Header.m_iChunkColorminiMapSize * m_pWorldMap->m_Header.m_iChunkColorminiMapSize];
	memset(m_pWorldMap->m_WorldChunkColorMiniMapTextureData, 0xFF000000, sizeof(uint32) * m_pWorldMap->m_Header.m_iChunkColorminiMapSize * m_pWorldMap->m_Header.m_iChunkColorminiMapSize);
	
	// Create terrain and OpenGL Resource
	createTerrain(terrainsize, bUsePerlinNoise, maxTerrainHeight);

}


//==============================================================================
//==============================================================================






void COpenGLES2WorldSystemManager::initializeTerrainRenderer(bool bLoadFromMap)
{
	// set terrain size
	m_pRenderDevice->getOpenGLTerrainRenderer()->setTerrainSize( m_pTerrain->GetTerrainChunkSize() );

	// Create terrain Chunk VAO
	for( uint32 j=0; j<m_pTerrain->GetTerrainChunkSize(); j++ )
	{
		for( uint32 i=0; i<m_pTerrain->GetTerrainChunkSize(); i++ )
		{
			m_pRenderDevice->getOpenGLTerrainRenderer()->createChunkVBO(j * m_pTerrain->GetTerrainChunkSize() + i);
			m_pRenderDevice->getOpenGLTerrainRenderer()->createChunkTextureFromWorldMap(m_pWorldMap, j * m_pTerrain->GetTerrainChunkSize() + i);
		}
	}
	
	// Register terrain alphablend texture
	m_pRenderDevice->getOpenGLTerrainRenderer()->registerBlendTexture(m_pWorldMap, String(L"Blendmap-")+getWorldName(), m_pTerrain->GetTerrainChunkSize());
	
	// Register terrain mini colormap texture
	//if( !bLoadFromMap )
	//{
		//uint32* pMinimapData = getWorldMap()->m_WorldChunkColorMiniMapTextureData;
		//for( uint32 row = 0; row < m_pTerrain->GetTerrainChunkSize(); row++ )
		//	for( uint32 col = 0; col < m_pTerrain->GetTerrainChunkSize(); col++ )
		//		m_pRenderDevice->getOpenGLTerrainRenderer()->generateChunkColorMinimapData(pMinimapData, m_pTerrain->GetTerrainChunkSize(), row, col);
	//}
	m_pRenderDevice->getOpenGLTerrainRenderer()->registerColorMinimapTexture(m_pWorldMap, String(L"ColorMinimap-")+getWorldName(), m_pTerrain->GetTerrainChunkSize());

	// Register terrain lightmap texture
	m_pRenderDevice->getOpenGLTerrainRenderer()->registerLightmapTexture(getWorldName(), String(L"TerrainLightmap.pvr"));
}

void COpenGLES2WorldSystemManager::releaseTerrainRenderer()
{
	// unRegister terrain alphablend texture
	m_pRenderDevice->getOpenGLTerrainRenderer()->unregisterBlendTexture(String(L"Blendmap-")+getWorldName());
	
	// unRegister terrain mini colormap texture
	m_pRenderDevice->getOpenGLTerrainRenderer()->unRegisterColorMinimapTexture(String(L"ColorMinimap-")+getWorldName());

	// unRegister terrain lightmap texture
	m_pRenderDevice->getOpenGLTerrainRenderer()->unregisterLightmapTexture();

	// Release Chunk VAO
	if( m_pTerrain )
	{
		for( uint32 j=0; j<m_pTerrain->GetTerrainChunkSize(); j++ )
		{
			for( uint32 i=0; i<m_pTerrain->GetTerrainChunkSize(); i++ )
				m_pRenderDevice->getOpenGLTerrainRenderer()->releaseChunkVBO(j * m_pTerrain->GetTerrainChunkSize() + i);
		}
	}
}

void COpenGLES2WorldSystemManager::getVisibleSceneObjectArray(const Frustum& ViewFrustum, const Array<CSGPTerrainChunk*>& VisibleChunkArray, Array<ISGPObject*>& VisibleSceneObjectArray)
{
	Frustum MirroredViewFrustum;
	if( needRenderWater() )
	{
		MirroredViewFrustum.setFrom( m_pRenderDevice->getOpenGLWaterRenderer()->m_MirrorViewMatrix * m_pRenderDevice->getOpenGLCamera()->m_mProjMatrix );
	}


	CSGPTerrainChunk** pEnd = VisibleChunkArray.end();
	for( CSGPTerrainChunk** pBegin = VisibleChunkArray.begin(); pBegin < pEnd; pBegin++ )
	{
		const Array<ISGPObject*>& chunkObjArray = (*pBegin)->GetTerrainChunkObject();
		
		ISGPObject** pObjEnd = chunkObjArray.end();
		for( ISGPObject** pObjBegin = chunkObjArray.begin(); pObjBegin < pObjEnd; pObjBegin++ )
		{
			AABBox objAABB;
			objAABB.Construct( &((*pObjBegin)->getBoundingBox()) );
			if( objAABB.Intersects(ViewFrustum) )
				VisibleSceneObjectArray.addIfNotAlreadyThere(*pObjBegin);
			if( needRenderWater() && objAABB.Intersects(MirroredViewFrustum) )
				VisibleSceneObjectArray.addIfNotAlreadyThere(*pObjBegin);
		}
	}
}

void COpenGLES2WorldSystemManager::initializeWaterRenderer()
{	
	if( isUsingReflectionMap() )
		m_pRenderDevice->getOpenGLWaterRenderer()->createWaterReflectionFBO(m_pRenderDevice->getViewPort().Width/2, m_pRenderDevice->getViewPort().Height/2);
	if( isUsingRefractionMap() )
	{
		m_pRenderDevice->getOpenGLWaterRenderer()->createWaterRefractionFBO(m_pRenderDevice->getViewPort().Width, m_pRenderDevice->getViewPort().Height);
		m_pRenderDevice->getOpenGLWaterRenderer()->createSceneBufferFBO(m_pRenderDevice->getViewPort().Width, m_pRenderDevice->getViewPort().Height);
	}
	if( (!isUsingReflectionMap()) && (!isUsingRefractionMap()) )
		m_pRenderDevice->getOpenGLWaterRenderer()->releaseWaterFBO();
}