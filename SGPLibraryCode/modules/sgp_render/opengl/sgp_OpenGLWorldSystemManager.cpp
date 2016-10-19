

COpenGLWorldSystemManager::COpenGLWorldSystemManager(COpenGLRenderDevice* pRenderDevice, Logger* pLogger)
	: m_pRenderDevice(pRenderDevice), m_pLogger(pLogger), 
	  m_pWorldMap(NULL), m_pTerrain(NULL), m_pSkydome(NULL), m_pWorldSun(NULL), m_pWater(NULL), m_pGrass(NULL),
	  m_pWorldMapRawMemoryAddress(NULL)
{
	m_VisibleSceneObjectArray.ensureStorageAllocated(INIT_SCENEOBJECTARRAYSIZE);
	m_VisibleChunkArray.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE / 3);
	m_WaterMirrorVisibleChunkArray.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE / 3);

}

COpenGLWorldSystemManager::~COpenGLWorldSystemManager()
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

void COpenGLWorldSystemManager::createTerrain( SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight )
{
	// Create Terrain
	m_pTerrain = new CSGPTerrain();
	m_pTerrain->InitializeCreateHeightmap( terrainsize, bUsePerlinNoise, maxTerrainHeight );
	m_pTerrain->CreateLODHeights();
	m_pTerrain->UpdateBoundingBox();

	// Create VB/IB render resource for Terrain
	initializeTerrainRenderer(false);
}

void COpenGLWorldSystemManager::createSkydome( const String& skydomeMF1FileName )
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

void COpenGLWorldSystemManager::releaseSkydome()
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

void COpenGLWorldSystemManager::createWorldSun()
{
	m_pWorldSun = new CSGPWorldSun();
}

void COpenGLWorldSystemManager::setWorldSunPosition( float fSunPosition )
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

void COpenGLWorldSystemManager::setSkydomeTexture( const String& SkyDomeColudTextureName, const String& SkyDomeNoiseTextureName )
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


ISGPObject* COpenGLWorldSystemManager::createObject( const String& MF1FileNameStr, const String& SceneObjectName,
	const Vector3D& vPos, const Vector3D& vRotationXYZ,
	float fScale, float fAlpha )
{
	ISGPObject* pSceneObject = new ISGPObject();

	strcpy( pSceneObject->m_MF1FileName, MF1FileNameStr.toUTF8().getAddress() );
	strcpy( pSceneObject->m_SceneObjectName, SceneObjectName.toUTF8().getAddress() );

	pSceneObject->m_fPosition[0] = vPos.x;
	pSceneObject->m_fPosition[1] = vPos.y;
	pSceneObject->m_fPosition[2] = vPos.z;
	pSceneObject->m_fRotationXYZ[0] = vRotationXYZ.x;
	pSceneObject->m_fRotationXYZ[1] = vRotationXYZ.y;
	pSceneObject->m_fRotationXYZ[2] = vRotationXYZ.z;

	pSceneObject->m_fScale = fScale;
	pSceneObject->m_fAlpha = fAlpha;
	
	return pSceneObject;
}


void COpenGLWorldSystemManager::addSceneObject( ISGPObject* obj, uint32 ConfigIndex )
{
	// if have exist
	if( m_SenceObjectArray.contains(obj) )
		return;

	// Found empty Scene object ID
	int iSceneID = m_SenceObjectArray.indexOf(NULL);
	if( iSceneID == -1 )
	{
		iSceneID = m_SenceObjectArray.size();
		m_SenceObjectArray.add(obj);
	}
	else
	{
		m_SenceObjectArray.set(iSceneID, obj);
	}
	obj->m_iSceneID = iSceneID;
	obj->m_iConfigIndex = ConfigIndex;

	// create mesh instance and add it to map data struct
	CStaticMeshInstance *pStaticModel = new CStaticMeshInstance(m_pRenderDevice);
	pStaticModel->changeModel( String(obj->getMF1FileName()), obj->m_iConfigIndex );
	pStaticModel->setPosition( obj->m_fPosition[0], obj->m_fPosition[1], obj->m_fPosition[2] );
	pStaticModel->setRotationXYZ( obj->m_fRotationXYZ[0], obj->m_fRotationXYZ[1], obj->m_fRotationXYZ[2] );
	pStaticModel->setScale( obj->m_fScale );
	pStaticModel->setInstanceAlpha( obj->m_fAlpha );		// set Alpha
	
	m_SceneIDToInstanceMap.set(iSceneID, pStaticModel);

	// Register object lightmap texture
	pStaticModel->registerLightmapTexture( getWorldName(), String(obj->getSceneObjectName())+String(".dds") );
}

bool COpenGLWorldSystemManager::refreshSceneObject( ISGPObject* obj )
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

void COpenGLWorldSystemManager::deleteSceneObject( ISGPObject* obj )
{
	int iSceneID = m_SenceObjectArray.indexOf(obj);
	// if not have exist
	if( iSceneID == -1 )
		return;
	else
	{
		CStaticMeshInstance* pInstance = m_SceneIDToInstanceMap[iSceneID];
		pInstance->destroyModel();
		delete pInstance;
		pInstance = NULL;

		m_SceneIDToInstanceMap.remove(iSceneID);
		m_SenceObjectArray.set(iSceneID, NULL);
	}
}

bool COpenGLWorldSystemManager::flushSceneObject( ISGPObject* pObjArray, uint32 iObjNum, bool bRemove )
{
	jassert( m_pTerrain );

	bool bAllSceneObjectFlushed = true;

	if( bRemove )
	{
		// delete scene object
		for( uint32 i=0; i<iObjNum; i++ )
		{
			for( uint32 j=0; j<pObjArray[i].getObjectInChunkNum(); j++ )
			{
				m_pTerrain->RemoveSceneObject( &pObjArray[i], pObjArray[i].getObjectInChunkIndex(j) );
			}
		}
	}
	else
	{
		// add scene object or change scene object position
		for( uint32 i=0; i<iObjNum; i++ )
		{
			if( pObjArray[i].m_bRefreshed ) 
			{
				Array<int32> TerrainChunkIndex;

				CStaticMeshInstance* pInst = getMeshInstanceBySceneID(pObjArray[i].getSceneObjectID());

				Matrix4x4 ModelMatrix;
				ModelMatrix.Identity();		
				ModelMatrix._11 = ModelMatrix._22 = ModelMatrix._33 = pInst->getScale();		
				Matrix4x4 matTemp;
				Quaternion qRotation;
				qRotation.MakeFromEuler(pInst->getRotationXYZ().x, pInst->getRotationXYZ().y, pInst->getRotationXYZ().z);
				//Convert the quaternion to a rotation matrix
				qRotation.GetMatrix(&matTemp);
				ModelMatrix = ModelMatrix * matTemp;
				ModelMatrix.SetTranslation( pInst->getPosition() );		
				// Update Instance OOBB boundingbox
				OBBox ObjBoundingBoxOBB;
				ObjBoundingBoxOBB.DeTransform( pInst->getStaticMeshOBBox(), ModelMatrix );
				pObjArray[i].setBoundingBox( ObjBoundingBoxOBB );

				AABBox ObjBoundingBoxAABB;
				ObjBoundingBoxAABB.Construct(&ObjBoundingBoxOBB);

				for( int32 j =0; j<m_pTerrain->m_TerrainChunks.size(); j++ )
				{
					AABBox chunkAABB = m_pTerrain->m_TerrainChunks[j]->m_BoundingBox;
					chunkAABB += Vector3D(chunkAABB.vcMax.x, ObjBoundingBoxAABB.vcMax.y, chunkAABB.vcMax.z);
					chunkAABB += Vector3D(chunkAABB.vcMin.x, ObjBoundingBoxAABB.vcMin.y, chunkAABB.vcMin.z);

					if( OBBox(&chunkAABB).Intersects(ObjBoundingBoxOBB) )
						TerrainChunkIndex.add(j);
				}

				Array<int32> oldTerrainChunkIndex = Array<int32>(pObjArray[i].m_pObjectInChunkIndex, pObjArray[i].m_iObjectInChunkIndexNum);
				Array<int32> oldTerrainChunkIndex1 = oldTerrainChunkIndex;
				if( TerrainChunkIndex == oldTerrainChunkIndex )
					continue;

				// delete old data
				if( (pObjArray[i].m_iObjectInChunkIndexNum > 0) && pObjArray[i].m_pObjectInChunkIndex )
				{
					delete [] pObjArray[i].m_pObjectInChunkIndex;
					pObjArray[i].m_pObjectInChunkIndex = NULL;
					pObjArray[i].m_iObjectInChunkIndexNum = 0;
				}

				// Create new data
				pObjArray[i].m_iObjectInChunkIndexNum = TerrainChunkIndex.size();
				if( pObjArray[i].m_iObjectInChunkIndexNum > 0 )
					pObjArray[i].m_pObjectInChunkIndex = new int32 [pObjArray[i].m_iObjectInChunkIndexNum];
				else
					pObjArray[i].m_pObjectInChunkIndex = NULL;
				for( uint32 j=0; j<pObjArray[i].m_iObjectInChunkIndexNum; j++ )
					pObjArray[i].m_pObjectInChunkIndex[j] = TerrainChunkIndex.getUnchecked(j);

				/////////////////////////////////////////////////////////////////////////
				oldTerrainChunkIndex.removeValuesIn(TerrainChunkIndex);
				// Remove obj from terrain chunk
				for( int k=0; k<oldTerrainChunkIndex.size(); k++ )
				{
					m_pTerrain->RemoveSceneObject( &pObjArray[i], oldTerrainChunkIndex.getUnchecked(k) );
				}

				TerrainChunkIndex.removeValuesIn(oldTerrainChunkIndex1);

				// Add obj to terrain chunk
				for( int k=0; k<TerrainChunkIndex.size(); k++ )
				{
					m_pTerrain->AddSceneObject( &pObjArray[i], TerrainChunkIndex.getUnchecked(k) );
				}

			}
			else
				bAllSceneObjectFlushed = false;
		}
	}


	return bAllSceneObjectFlushed;
}



void COpenGLWorldSystemManager::initializeQuadTree()
{
	// First Release QuadTree then Create Quadtree
	m_QuadTree.Shutdown();
	m_QuadTree.InitializeFromTerrain(m_pTerrain);
}

void COpenGLWorldSystemManager::initializeCollisionSet()
{
	m_CollisionTree.release();

	if( m_pTerrain )
	{
		for( uint32 j=0; j<m_pTerrain->GetTerrainChunkSize(); j++ )
			for( uint32 i=0; i<m_pTerrain->GetTerrainChunkSize(); i++ )
				m_pTerrain->m_TerrainChunks[j * m_pTerrain->GetTerrainChunkSize() + i]->AddTriangleCollisionSet(m_CollisionTree);
	}

	Array<ISGPObject*> BuildingObjectArray;
	getAllSceneBuilding(BuildingObjectArray);

	ISGPObject** pEnd = BuildingObjectArray.end();	
	for( ISGPObject** pBegin = BuildingObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin )
		{
			if( !(*pBegin)->m_bCastShadow )
				continue;

			CStaticMeshInstance *pInstance = m_SceneIDToInstanceMap[(*pBegin)->getSceneObjectID()];
			Matrix4x4 modelMatrix = pInstance->getModelMatrix();

			CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(pInstance->getMF1ModelResourceID());
			CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;

			jassert( pMF1Model );

			for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
			{
				for( uint32 j=0; j<pMF1Model->m_pLOD0Meshes[i].m_iNumIndices; j += 3 )
				{
					Vector3D v0(
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j  ]  ].vPos[0],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j  ]  ].vPos[1],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j  ]  ].vPos[2] );
					Vector3D v1(
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+1]  ].vPos[0],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+1]  ].vPos[1],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+1]  ].vPos[2] );
					Vector3D v2(
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+2]  ].vPos[0],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+2]  ].vPos[1],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[  pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+2]  ].vPos[2] );

					v0 = v0 * modelMatrix;
					v1 = v1 * modelMatrix;
					v2 = v2 * modelMatrix;

					m_CollisionTree.addTriangle(v0, v1, v2, NULL);
					m_CollisionTree.addTriangle(v0, v2, v1, NULL);
				}
			}
		}
	}

	m_pLogger->writeToLog(String("Start building Collision Tree..."), ELL_INFORMATION);
	m_CollisionTree.build(3, 1, 50);
	m_pLogger->writeToLog(String("Finish building Collision Tree..."), ELL_INFORMATION);
}

float COpenGLWorldSystemManager::getTerrainHeight(float positionX, float positionZ)
{
	if( m_pTerrain )
		return m_pTerrain->GetTerrainHeight(positionX, positionZ);

	return 0;
}

float COpenGLWorldSystemManager::getRealTerrainHeight(float positionX, float positionZ)
{
	if( m_pTerrain )
		return m_pTerrain->GetRealTerrainHeight(positionX, positionZ);

	return 0;

}

Vector3D COpenGLWorldSystemManager::getTerrainNormal(float positionX, float positionZ)
{
	if( m_pTerrain )
		return m_pTerrain->GetTerrainNormal(positionX, positionZ);
	return Vector3D(0,0,0);
}

void COpenGLWorldSystemManager::saveWorldToFile(const String& WorkingDir, const String& WorldMapFileName)
{
	strcpy( m_pWorldMap->m_Header.m_cFilename, WorldMapFileName.toUTF8().getAddress() );

	// Copy SceneObject data from m_SenceObjectArray in world to m_pWorldMap
	m_pWorldMap->m_Header.m_iSceneObjectNum = 0;
	for( int i=0; i<getSenceObjectCount(); i++ )
	{
		ISGPObject* pObj = getSceneObjectBySceneID(i);
		if( pObj )
			m_pWorldMap->m_Header.m_iSceneObjectNum++;
	}

	if( m_pWorldMap->m_Header.m_iSceneObjectNum > 0 )
		m_pWorldMap->m_pSceneObject = new ISGPObject [m_pWorldMap->m_Header.m_iSceneObjectNum];
	
	int idx = 0;
	for( int i=0; i<getSenceObjectCount(); i++ )
	{
		ISGPObject* pObj = getSceneObjectBySceneID(i);
		if( pObj )
			m_pWorldMap->m_pSceneObject[idx++].Clone(pObj);
	}

	// Copy LightObject data from m_LightObjectArray in world to m_pWorldMap
	m_pWorldMap->m_Header.m_iLightObjectNum = 0;
	for( int i=0; i<m_LightObjectArray.size(); ++i)
	{
		ISGPLightObject* pLightObj = m_LightObjectArray[i];
		if(pLightObj)
			m_pWorldMap->m_Header.m_iLightObjectNum++;
	}
	if( m_pWorldMap->m_Header.m_iLightObjectNum > 0 )
		m_pWorldMap->m_pLightObject = new ISGPLightObject [m_pWorldMap->m_Header.m_iLightObjectNum];

	idx = 0;
	for( int i=0; i<m_LightObjectArray.size(); i++ )
	{
		ISGPLightObject* pLightObj = m_LightObjectArray[i];
		if( pLightObj )
			m_pWorldMap->m_pLightObject[idx++].Clone(pLightObj);
	}

	// HeightMap from terrain
	memcpy( m_pWorldMap->m_pTerrainHeightMap, m_pTerrain->GetHeightMap(), sizeof(uint16) * m_pTerrain->GetVertexCount() );
	// Normal data from terrain chunk
	m_pTerrain->SaveNormalTable(m_pWorldMap->m_pTerrainNormal, m_pWorldMap->m_pTerrainTangent, m_pWorldMap->m_pTerrainBinormal);

	// Water data
	if( m_pWater && (m_pWater->m_TerrainWaterChunks.size() > 0) )
	{
		m_pWorldMap->m_WaterSettingData.m_bHaveWater = true;
		m_pWorldMap->m_WaterSettingData.m_fWaterHeight = m_pWater->m_fWaterHeight;
		strcpy( m_pWorldMap->m_WaterSettingData.m_WaterWaveTextureName, m_pWater->m_WaterWaveTextureName.toUTF8().getAddress() );
		
		// save Water Parameter
		m_pWorldMap->m_WaterSettingData.m_vWaterSurfaceColor[0] = m_pWater->m_vWaterSurfaceColor.x;
		m_pWorldMap->m_WaterSettingData.m_vWaterSurfaceColor[1] = m_pWater->m_vWaterSurfaceColor.y;
		m_pWorldMap->m_WaterSettingData.m_vWaterSurfaceColor[2] = m_pWater->m_vWaterSurfaceColor.z;
		m_pWorldMap->m_WaterSettingData.m_fRefractionBumpScale = m_pWater->m_fRefractionBumpScale;
		m_pWorldMap->m_WaterSettingData.m_fReflectionBumpScale = m_pWater->m_fReflectionBumpScale;
		m_pWorldMap->m_WaterSettingData.m_fFresnelBias = m_pWater->m_fFresnelBias;
		m_pWorldMap->m_WaterSettingData.m_fFresnelPow = m_pWater->m_fFresnelPow;
		m_pWorldMap->m_WaterSettingData.m_fWaterDepthScale = m_pWater->m_fWaterDepthScale;
		m_pWorldMap->m_WaterSettingData.m_fWaterBlendScale = m_pWater->m_fWaterBlendScale;
		m_pWorldMap->m_WaterSettingData.m_vWaveDir[0] = m_pWater->m_vWaveDir.x;
		m_pWorldMap->m_WaterSettingData.m_vWaveDir[1] = m_pWater->m_vWaveDir.y;
		m_pWorldMap->m_WaterSettingData.m_fWaveSpeed = m_pWater->m_fWaveSpeed;
		m_pWorldMap->m_WaterSettingData.m_fWaveRate = m_pWater->m_fWaveRate;
		m_pWorldMap->m_WaterSettingData.m_fWaterSunSpecularPower = m_pWater->m_fWaterSunSpecularPower;

		memset(m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex, -1, sizeof(int32) * m_pWorldMap->m_Header.m_iChunkNumber);
		for( int i=0; i<m_pWater->m_TerrainWaterChunks.size(); i++ )
			m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex[ m_pWater->m_TerrainWaterChunks[i]->GetTerrainChunkIndex() ] = m_pWater->m_TerrainWaterChunks[i]->GetTerrainChunkIndex();
	}
	else
	{
		m_pWorldMap->m_WaterSettingData.m_bHaveWater = false;
		memset(m_pWorldMap->m_WaterSettingData.m_pWaterChunkIndex, -1, sizeof(int32) * m_pWorldMap->m_Header.m_iChunkNumber);
	}

	// Grass data
	if( m_pGrass )
	{
		strcpy( m_pWorldMap->m_GrassData.m_GrassTextureName, m_pGrass->GetGrassTextureName().toUTF8().getAddress() );
		m_pWorldMap->m_GrassData.m_TextureAtlas[0] = m_pGrass->m_fTextureAtlasNbX;
		m_pWorldMap->m_GrassData.m_TextureAtlas[1] = m_pGrass->m_fTextureAtlasNbY;
		m_pWorldMap->m_GrassData.m_fGrassFarFadingStart = CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart;
		m_pWorldMap->m_GrassData.m_fGrassFarFadingEnd = CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd;
		m_pWorldMap->m_GrassData.m_nChunkGrassClusterNum = m_pTerrain->GetTerrainChunkSize() * m_pTerrain->GetTerrainChunkSize();
		// save Grass Parameter
		m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[0] = m_pGrass->m_vWindDirectionAndStrength.x;
		m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[1] = m_pGrass->m_vWindDirectionAndStrength.y;
		m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[2] = m_pGrass->m_vWindDirectionAndStrength.z;
		m_pWorldMap->m_GrassData.m_vWindDirectionAndStrength[3] = m_pGrass->m_vWindDirectionAndStrength.w;
		m_pWorldMap->m_GrassData.m_fGrassPeriod = m_pGrass->m_fGrassPeriod;
	}

	// Skydome data	
	if( m_pSkydome )
	{
		m_pWorldMap->m_SkydomeData.m_bHaveSkydome = true;
		strcpy( m_pWorldMap->m_SkydomeData.m_SkydomeMF1FileName, m_pSkydome->m_MF1FileName.toUTF8().getAddress() );
		strcpy( m_pWorldMap->m_SkydomeData.m_CloudTextureFileName, m_pSkydome->m_cloudTextureFileName.toUTF8().getAddress() );
		strcpy( m_pWorldMap->m_SkydomeData.m_NoiseTextureFileName, m_pSkydome->m_noiseTextureFileName.toUTF8().getAddress() );

		// save Skydome Parameter
		m_pWorldMap->m_SkydomeData.m_coludMoveSpeed_x = m_pSkydome->m_coludMoveSpeed_x;
		m_pWorldMap->m_SkydomeData.m_coludMoveSpeed_z = m_pSkydome->m_coludMoveSpeed_z;
		m_pWorldMap->m_SkydomeData.m_coludNoiseScale = m_pSkydome->m_coludNoiseScale;
		m_pWorldMap->m_SkydomeData.m_coludBrightness = m_pSkydome->m_coludBrightness;
		m_pWorldMap->m_SkydomeData.m_scale = m_pSkydome->GetScale();
		// save Skydome atmosphere Parameter
		CSGPHoffmanPreethemScatter& scatter = m_pSkydome->GetScatter();
		m_pWorldMap->m_SkydomeData.m_fHGgFunction = scatter.m_fHGgFunction;
		m_pWorldMap->m_SkydomeData.m_fInscatteringMultiplier = scatter.m_fInscatteringMultiplier;
		m_pWorldMap->m_SkydomeData.m_fBetaRayMultiplier = scatter.m_fBetaRayMultiplier;
		m_pWorldMap->m_SkydomeData.m_fBetaMieMultiplier = scatter.m_fBetaMieMultiplier;
		m_pWorldMap->m_SkydomeData.m_fSunIntensity = scatter.m_fSunIntensity;
		m_pWorldMap->m_SkydomeData.m_fTurbitity = scatter.m_fTurbitity;

		if( m_pWorldSun )
			m_pWorldMap->m_SkydomeData.m_fSunPosition = m_pWorldSun->m_fSunPosition;
	}
	else
		m_pWorldMap->m_SkydomeData.m_bHaveSkydome = false;

	// set CSGPWorldConfig Parameter
	m_pWorldMap->m_WorldConfigTag.m_bUsingQuadTree = CSGPWorldConfig::getInstance()->m_bUsingQuadTree;
	m_pWorldMap->m_WorldConfigTag.m_bVisibleCull = CSGPWorldConfig::getInstance()->m_bVisibleCull;
	m_pWorldMap->m_WorldConfigTag.m_bUsingTerrainLOD = CSGPWorldConfig::getInstance()->m_bUsingTerrainLOD;
	m_pWorldMap->m_WorldConfigTag.m_bPostFog = CSGPWorldConfig::getInstance()->m_bPostFog;
	m_pWorldMap->m_WorldConfigTag.m_bDOF = CSGPWorldConfig::getInstance()->m_bDOF;

	m_pWorldMap->SaveWorldMap( WorkingDir, WorldMapFileName );

	// release allocated Object and LightObject memory
	if( m_pWorldMap->m_Header.m_iSceneObjectNum > 0 )
	{
		if( m_pWorldMap->m_pSceneObject!=NULL )
		{
			delete [] m_pWorldMap->m_pSceneObject;
			m_pWorldMap->m_pSceneObject = NULL;
		}
	}
	if( m_pWorldMap->m_Header.m_iLightObjectNum > 0 )
	{
		if( m_pWorldMap->m_pLightObject!=NULL )
		{
			delete [] m_pWorldMap->m_pLightObject;
			m_pWorldMap->m_pLightObject = NULL;
		}
	}
}

void COpenGLWorldSystemManager::loadWorldFromFile(const String& WorkingDir, const String& WorldMapFileName, bool bLoadObjs)
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
			pStaticModel->registerLightmapTexture( getWorldName(), String(obj->getSceneObjectName())+String(".dds") );
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

	// recreate Frame Buffer Object
	m_pRenderDevice->recreateRenderToFrameBuffer(m_pRenderDevice->getViewPort().Width, m_pRenderDevice->getViewPort().Height, false);

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

void COpenGLWorldSystemManager::loadObjectToWorldForEditor(ISGPObject* pObjArray, uint32 count)
{
	for( uint32 i=0; i<count; i++)
	{
		ISGPObject* pObj = &(pObjArray[i]);

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

	for( uint32 i=0; i<count ;i++ )
	{
		ISGPObject* obj =  &(pObjArray[i]);
		// create mesh instance and add it to map data struct
		CStaticMeshInstance *pStaticModel = new CStaticMeshInstance(m_pRenderDevice);
		pStaticModel->changeModel( String(obj->getMF1FileName()), obj->m_iConfigIndex );
		pStaticModel->setPosition( obj->m_fPosition[0], obj->m_fPosition[1], obj->m_fPosition[2] );
		pStaticModel->setRotationXYZ( obj->m_fRotationXYZ[0], obj->m_fRotationXYZ[1], obj->m_fRotationXYZ[2] );
		pStaticModel->setScale( obj->m_fScale );
		pStaticModel->setInstanceAlpha(obj->m_fAlpha);	// set Alpha

		m_SceneIDToInstanceMap.set(obj->getSceneObjectID(), pStaticModel);

		// add to terrain chunk
		for( uint32 i=0; i<obj->getObjectInChunkNum(); i++ )
		{
			m_pTerrain->AddSceneObject( obj, obj->getObjectInChunkIndex(i) );
		}

		// Register object lightmap texture
		pStaticModel->registerLightmapTexture( getWorldName(), String(obj->getSceneObjectName())+String(".dds") );
	}
}

void COpenGLWorldSystemManager::updateWorld(float fDeltaTimeInSecond)
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

	// Update Sky dome
	Vector4D CamPos;
	m_pRenderDevice->getCamreaPosition( &CamPos );
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




void COpenGLWorldSystemManager::shutdownWorld()
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

void COpenGLWorldSystemManager::renderWorld()
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


void COpenGLWorldSystemManager::createWater( float fWaterHeight, const String& WaterWaveTextureName )
{
	CSGPWorldConfig::getInstance()->m_bHavingWaterInWorld = true;
	CSGPWorldConfig::getInstance()->m_bPostFog = true;
	m_pWater = new CSGPWater(fWaterHeight);
	m_pWater->m_WaterWaveTextureName = WaterWaveTextureName;
	m_pRenderDevice->getOpenGLWaterRenderer()->createWaterWaveTexture( WaterWaveTextureName );
}

void COpenGLWorldSystemManager::addWaterChunk( int32 terrainChunkIndex )
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

bool COpenGLWorldSystemManager::needRenderWater()
{
	return m_pRenderDevice->getOpenGLWaterRenderer()->needRenderWater();
}

bool COpenGLWorldSystemManager::isTerrainChunkVisible(CSGPTerrainChunk* pChunk)
{
	return m_VisibleChunkArray.contains( pChunk );
}



void COpenGLWorldSystemManager::createGrass(const String& grassTextureName, uint16 TexAtlasX, uint16 TexAtlasY)
{
	m_pGrass = new CSGPGrass();
	m_pGrass->SetGrassTextureName( grassTextureName );
	m_pGrass->SetAtlasDimensions(TexAtlasX, TexAtlasY);

	m_pRenderDevice->getOpenGLGrassRenderer()->initializeFromGrass(m_pGrass);
}

void COpenGLWorldSystemManager::setGrassCluster(float fPosX, float fPosZ, const SGPGrassCluster& ClusterData)
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



void COpenGLWorldSystemManager::getAllSceneBuilding(Array<ISGPObject*>& BuildingObjectArray)
{
	ISGPObject** pEnd = m_SenceObjectArray.end();	
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin && (*pBegin)->getSceneObjectType() == SGPOT_Building )
			BuildingObjectArray.add( (*pBegin) );
	}
}

ISGPObject* COpenGLWorldSystemManager::getSceneObjectByName(const char* pSceneObjectNameStr)
{
	ISGPObject** pEnd = m_SenceObjectArray.end();	
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin && (strcmp( (*pBegin)->m_SceneObjectName, pSceneObjectNameStr ) == 0) )
			return *pBegin;
	}
	return NULL;
}

void COpenGLWorldSystemManager::getAllSceneObjectByName(const char* pSceneObjectNameStr, Array<ISGPObject*>& SceneObjectArray)
{
	ISGPObject** pEnd = m_SenceObjectArray.end();	
	for( ISGPObject** pBegin = m_SenceObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		if( *pBegin && (strcmp( (*pBegin)->m_SceneObjectName, pSceneObjectNameStr ) == 0) )
			SceneObjectArray.add( *pBegin );
	}
}

ISGPObject* COpenGLWorldSystemManager::getSceneObjectBySceneID( uint32 nSceneObjectID )
{
	return m_SenceObjectArray[nSceneObjectID];
}

CStaticMeshInstance* COpenGLWorldSystemManager::getMeshInstanceBySceneID( uint32 nSceneObjectID )
{
	return m_SceneIDToInstanceMap[nSceneObjectID];
}










//==============================================================================
// Editor Interface Function
//==============================================================================
bool COpenGLWorldSystemManager::setHeightMap(uint32 index_x, uint32 index_z, uint16 iHeight)
{
	jassert( m_pTerrain );

	if( index_x > m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM )
		return false;
	if( index_z > m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM )
		return false;

	uint32 heightmap_index = index_z * (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM+1) + index_x;
	if( m_pTerrain->GetHeightMap()[heightmap_index] == iHeight )
		return false;

	m_pTerrain->SetHeightMap(heightmap_index, iHeight);
	if( iHeight > m_pTerrain->GetTerrainMaxHeight() )
		m_pTerrain->SetTerrainMaxHeight(iHeight);

	return true;
}

void COpenGLWorldSystemManager::flushTerrainHeight(uint32* pChunkIndex, uint32 ichunkNum)
{
	jassert( m_pTerrain );

	for( uint32 i=0; i<ichunkNum; i++ )
		m_pTerrain->m_TerrainChunks[pChunkIndex[i]]->FlushTerrainChunkHeight();


	for( uint32 i=0; i<ichunkNum; i++ )
	{
		// Terrain VBO update
		m_pRenderDevice->getOpenGLTerrainRenderer()->flushChunkVBO( pChunkIndex[i] );

		CSGPTerrainChunk* pTerrainChunk = m_pTerrain->m_TerrainChunks[pChunkIndex[i]];

		// water update
		if( m_pWater && m_pWater->m_TerrainWaterChunks.contains( pTerrainChunk ) )
		{
			if( m_pWater->m_fWaterHeight > pTerrainChunk->m_BoundingBox.vcMax.y )
			{
				pTerrainChunk->m_BoundingBox += 
					Vector3D(	pTerrainChunk->m_BoundingBox.vcMax.x,
								m_pWater->m_fWaterHeight,
								pTerrainChunk->m_BoundingBox.vcMax.z );
			}
		}

		// grass update
		for( uint32 j=0; j<pTerrainChunk->GetGrassClusterDataCount(); j++ )
		{
			const SGPGrassCluster& GrassData = pTerrainChunk->GetGrassClusterData()[j];
			if( GrassData.nData == 0 )
				continue;

			SGPGrassCluster newGrassData = GrassData;
			newGrassData.fPositionY = getRealTerrainHeight(newGrassData.fPositionX, newGrassData.fPositionZ);
			Vector3D terrainNorm = m_pTerrain->GetTerrainNormal(newGrassData.fPositionX, newGrassData.fPositionZ);
			newGrassData.nPackedNormal =	(uint32((terrainNorm.x * 0.5f + 0.5f) * 255) << 24) +
											(uint32((terrainNorm.y * 0.5f + 0.5f) * 255) << 16) +
											(uint32((terrainNorm.z * 0.5f + 0.5f) * 255) << 8);			

			m_pTerrain->SetTerrainGrassLayerData(GrassData.fPositionX, GrassData.fPositionZ, newGrassData);
		}		
	}
		
}

void COpenGLWorldSystemManager::flushTerrainNormal(uint32* pChunkIndex, uint32 ichunkNum)
{
	if( !pChunkIndex || (ichunkNum <= 0) )
		return;

	// normal update
	m_pTerrain->CreateNormalTable();

	for( uint32 i=0; i<ichunkNum; i++ )
	{
		// Terrain VBO update
		m_pRenderDevice->getOpenGLTerrainRenderer()->flushChunkVBO( pChunkIndex[i] );

		// grass normal update
		for( uint32 j=0; j<m_pTerrain->m_TerrainChunks[i]->GetGrassClusterDataCount(); j++ )
		{
			const SGPGrassCluster& GrassData = m_pTerrain->m_TerrainChunks[i]->GetGrassClusterData()[j];
			if( GrassData.nData == 0 )
				continue;

			SGPGrassCluster newGrassData = GrassData;
			Vector3D terrainNorm = m_pTerrain->GetTerrainNormal(newGrassData.fPositionX, newGrassData.fPositionZ);
			newGrassData.nPackedNormal = (uint32((terrainNorm.x * 0.5f + 0.5f) * 255) << 24) +
											(uint32((terrainNorm.y * 0.5f + 0.5f) * 255) << 16) +
											(uint32((terrainNorm.z * 0.5f + 0.5f) * 255) << 8);			

			m_pTerrain->SetTerrainGrassLayerData(GrassData.fPositionX, GrassData.fPositionZ, newGrassData);
		}
	}
}





void COpenGLWorldSystemManager::createNewWorld(CSGPWorldMap* &pWorldMap, const char* WorldName, SGP_TERRAIN_SIZE terrainsize, bool bUsePerlinNoise, uint16 maxTerrainHeight, const String& Diffuse0TextureName)
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
	memset(m_pWorldMap->m_WorldChunkAlphaTextureData, (uint32)0xFF000000, sizeof(uint32) * m_pWorldMap->m_Header.m_iChunkAlphaTextureSize * m_pWorldMap->m_Header.m_iChunkAlphaTextureSize);

	m_pWorldMap->m_WorldChunkColorMiniMapTextureData = new uint32 [m_pWorldMap->m_Header.m_iChunkColorminiMapSize * m_pWorldMap->m_Header.m_iChunkColorminiMapSize];
	memset(m_pWorldMap->m_WorldChunkColorMiniMapTextureData, (uint32)0xFF000000, sizeof(uint32) * m_pWorldMap->m_Header.m_iChunkColorminiMapSize * m_pWorldMap->m_Header.m_iChunkColorminiMapSize);
	
	// Create terrain and OpenGL Resource
	createTerrain(terrainsize, bUsePerlinNoise, maxTerrainHeight);

}

void COpenGLWorldSystemManager::editTerrainChunkBlendTexture(uint32 SrcX, uint32 SrcZ, uint32 width, uint32 height, uint32* pAlphaBlendData)
{
	m_pRenderDevice->getOpenGLTerrainRenderer()->updateBlendTexture(SrcX, SrcZ, width, height, pAlphaBlendData);
}

void COpenGLWorldSystemManager::setTerrainChunkLayerTexture(uint32 ChunkIndex, ESGPTerrainChunkTexture nLayer, const String& TextureName)
{
	m_pRenderDevice->getOpenGLTerrainRenderer()->updateTerrainChunkLayerTexture(ChunkIndex, nLayer, TextureName);
}

uint32* COpenGLWorldSystemManager::flushTerrainChunkColorMinimapTexture(uint32* pChunkIndex, uint32 ichunkNum)
{	
	return m_pRenderDevice->getOpenGLTerrainRenderer()->updateColorMinimapTexture(pChunkIndex, ichunkNum);
}

void COpenGLWorldSystemManager::addWaterChunkForEditor( int32 *pChunkIndex, uint32 ChunkIndexNum )
{
	if( pChunkIndex && m_pWater && m_pTerrain )
	{
		for( uint32 idx = 0; idx < ChunkIndexNum; idx++ )
		{
			if( pChunkIndex[idx] < m_pTerrain->m_TerrainChunks.size() )
			{
				// Update chunk AABBox when terrain chunk having water
				if( m_pWater->m_fWaterHeight > m_pTerrain->m_TerrainChunks[pChunkIndex[idx]]->m_BoundingBox.vcMax.y )
				{
					m_pTerrain->m_TerrainChunks[pChunkIndex[idx]]->m_BoundingBox += 
						Vector3D(	m_pTerrain->m_TerrainChunks[pChunkIndex[idx]]->m_BoundingBox.vcMax.x,
									m_pWater->m_fWaterHeight,
									m_pTerrain->m_TerrainChunks[pChunkIndex[idx]]->m_BoundingBox.vcMax.z );
				}
				// if WaterHeight below terrain chunk's whole boundingbox, skip this chunk
				if( m_pWater->m_fWaterHeight >= m_pTerrain->m_TerrainChunks[pChunkIndex[idx]]->m_BoundingBox.vcMin.y )
					m_pWater->m_TerrainWaterChunks.addIfNotAlreadyThere( m_pTerrain->m_TerrainChunks[pChunkIndex[idx]] );
			}
		}
	}

	// recreate Frame Buffer Object
	m_pRenderDevice->recreateRenderToFrameBuffer(m_pRenderDevice->getViewPort().Width, m_pRenderDevice->getViewPort().Height, false);
}

void COpenGLWorldSystemManager::removeWaterChunkForEditor( int32 *pChunkIndex, uint32 ChunkIndexNum )
{
	if( pChunkIndex && m_pWater && m_pTerrain )
	{
		for( uint32 idx = 0; idx < ChunkIndexNum; idx++ )
		{
			if( pChunkIndex[idx] < m_pTerrain->m_TerrainChunks.size() )
			{
				m_pWater->m_TerrainWaterChunks.removeAllInstancesOf( m_pTerrain->m_TerrainChunks[pChunkIndex[idx]] );
				m_pTerrain->m_TerrainChunks[pChunkIndex[idx]]->UpdateAABB();
			}
		}
	}

	if( m_pWater->m_TerrainWaterChunks.size() == 0 )
	{
		m_pRenderDevice->getOpenGLWaterRenderer()->releaseWaterWaveTexture();
		if( m_pWater )
			delete m_pWater;
		m_pWater = NULL;
		CSGPWorldConfig::getInstance()->m_bHavingWaterInWorld = false;
	}

	// recreate Frame Buffer Object
	m_pRenderDevice->recreateRenderToFrameBuffer(m_pRenderDevice->getViewPort().Width, m_pRenderDevice->getViewPort().Height, false);
}

ISGPLightObject* COpenGLWorldSystemManager::createLightObject( const String& LightName,
	SGP_LIGHT_TYPE nLightType, const Vector3D& vPos , const Vector3D& vDirection,
		float fLightSize, float fRange,	const Colour& DiffuseColor )
{
	ISGPLightObject* pLightObject = new ISGPLightObject();

	strcpy( pLightObject->m_SceneObjectName, LightName.toUTF8().getAddress() );
	pLightObject->m_iLightType = nLightType;
	pLightObject->m_fPosition[0] = vPos.x;
	pLightObject->m_fPosition[1] = vPos.y;
	pLightObject->m_fPosition[2] = vPos.z;
	pLightObject->m_fDirection[0] = vDirection.x;
	pLightObject->m_fDirection[1] = vDirection.y;
	pLightObject->m_fDirection[2] = vDirection.z;

	pLightObject->m_fLightSize = fLightSize;
	pLightObject->m_fRange = fRange;
	pLightObject->m_fDiffuseColor[0] = DiffuseColor.getFloatRed();
	pLightObject->m_fDiffuseColor[1] = DiffuseColor.getFloatGreen();
	pLightObject->m_fDiffuseColor[2] = DiffuseColor.getFloatBlue();
	
	return pLightObject;
}

void COpenGLWorldSystemManager::addLightObject( ISGPLightObject* lightobj )
{
	// if have exist
	if( m_LightObjectArray.contains(lightobj) )
		return;

	// Found empty Light object ID
	int iLightID = m_LightObjectArray.indexOf(NULL);
	if( iLightID == -1 )
	{
		iLightID = m_LightObjectArray.size();
		m_LightObjectArray.add(lightobj);
	}
	else
	{
		m_LightObjectArray.set(iLightID, lightobj);
	}
	lightobj->m_iLightID = iLightID;
}

void COpenGLWorldSystemManager::deleteLightObject( ISGPLightObject* lightobj )
{
	int iLightID = m_LightObjectArray.indexOf(lightobj);
	if(iLightID != -1)
	{
		m_LightObjectArray.set(iLightID, NULL);
	}
}

uint32* COpenGLWorldSystemManager::updateTerrainLightmapTexture( float &fProgress, Random* pRandom )
{
	if( !m_pWorldMap || !m_pTerrain )
		return NULL;

	// Create Sample direction
	Vector3D *samples = new Vector3D [CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count];
	for(uint32 x = 0; x < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; x++)
	{
		Vector3D v;
		do {
				v.x = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
				v.y = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
				v.z = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
		} while (v * v > 1.0f);
		samples[x] = v;
	}

	// Create lightmap Image in memory
	uint32 LMTexWidth = m_pTerrain->GetTerrainChunkSize() * SGPTT_TILENUM * SGPTLD_LIGHTMAPTEXTURE_DIMISION;
	uint32 LMTexHeight = m_pTerrain->GetTerrainChunkSize() * SGPTT_TILENUM * SGPTLD_LIGHTMAPTEXTURE_DIMISION;

	uint32 *lMap = new uint32 [LMTexWidth * LMTexHeight];
	memset(lMap, 0, LMTexWidth * LMTexHeight * sizeof(uint32));

	float du = m_pTerrain->GetTerrainWidth() / (LMTexWidth - 1);
	float dv = m_pTerrain->GetTerrainWidth() / (LMTexHeight - 1);

	float fLightColorRGB[3] = {0.0f};
	float fLightDistensy = 0.0f;
	Vector3D samplePos;
	Vector3D sampleNormal;

	Vector3D vertexPos;
	Vector3D lightPos;
	Vector3D lightVec;
	Vector3D lightVecNor;

	// Pass 1, illuminate using the main light
	for(uint32 t = 0; t < LMTexHeight; t++)
	{
		for(uint32 s = 0; s < LMTexWidth; s++)
		{
			samplePos.Set( s * du,	getRealTerrainHeight(s*du, m_pTerrain->GetTerrainWidth()-t*dv), m_pTerrain->GetTerrainWidth() - t * dv );
			sampleNormal = getTerrainNormal(s*du, m_pTerrain->GetTerrainWidth()-t*dv);
				
			fLightColorRGB[0] = fLightColorRGB[1] = fLightColorRGB[2] = 0.0f;
			

			ISGPLightObject** pEnd = m_LightObjectArray.end();
			for( ISGPLightObject** pBegin = m_LightObjectArray.begin(); pBegin < pEnd; pBegin++ )
			{
				fLightDistensy = 0.0f;

				vertexPos = samplePos;
				lightPos.Set( (*pBegin)->m_fPosition[0], (*pBegin)->m_fPosition[1], (*pBegin)->m_fPosition[2] );
				
				lightVec = lightPos - vertexPos;
				float fdistance = lightVec.GetLength();
				if( fdistance >= (*pBegin)->m_fRange )
					continue;

				float atten = 1.0f / ( (*pBegin)->m_fAttenuation0 +
					(*pBegin)->m_fAttenuation1 * fdistance +
					(*pBegin)->m_fAttenuation2 * fdistance * fdistance );


				lightVecNor = lightVec;
				lightVecNor.Normalize();
				float diffuse = lightVecNor * sampleNormal;
				if( diffuse > 0 )
				{
					//m_CollisionTree.pushSphere(vertexPos, CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset);
					vertexPos += sampleNormal * CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset;
					for(uint32 k = 0; k < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; k++)
					{
						if( !m_CollisionTree.intersect( lightPos + samples[k] * (*pBegin)->m_fLightSize, vertexPos ) )
							fLightDistensy += 1.0f / CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count;
					}

					fLightColorRGB[0] += (*pBegin)->m_fDiffuseColor[0] * atten * fLightDistensy * diffuse;
					fLightColorRGB[1] += (*pBegin)->m_fDiffuseColor[1] * atten * fLightDistensy * diffuse;
					fLightColorRGB[2] += (*pBegin)->m_fDiffuseColor[2] * atten * fLightDistensy * diffuse;
				}
			}
			lMap[t * LMTexWidth + s] =	0xFF000000 +
										((uint32)(255.0f * fLightColorRGB[0]) << 16) +
										((uint32)(255.0f * fLightColorRGB[1]) << 8)  +
										 (uint32)(255.0f * fLightColorRGB[2]);
			fProgress = float(t * LMTexWidth + s) / float(LMTexHeight * LMTexWidth) * 0.5f;
		}
	//	m_pLogger->writeToLog(String("illuminate Terrain...")+String(fProgress), ELL_INFORMATION);
	}

	// Pass 2, indirect lumination (AO)
	Vector3D v;
	for(uint32 t = 0; t < LMTexHeight; t++)
	{
		for(uint32 s = 0; s < LMTexWidth; s++)
		{
			samplePos.Set( s * du,	getRealTerrainHeight(s*du, m_pTerrain->GetTerrainWidth()-t*dv), m_pTerrain->GetTerrainWidth() - t * dv );
			sampleNormal = getTerrainNormal(s*du, m_pTerrain->GetTerrainWidth()-t*dv);
			//m_CollisionTree.pushSphere(samplePos, CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset);
			samplePos += sampleNormal * CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset;
			for(uint32 x = 0; x < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; x++)
			{				
				do {
						v.x = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
						v.y = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
						v.z = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
				} while ((v * v > 1.0f) || (sampleNormal * v < 0));
				samples[x] = v * CSGPLightMapGenConfig::getInstance()->m_fLightMap_AO_Distance;
			}

			float AO = 0.0f;
			for(uint32 k = 0; k < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; k++)
			{
				if( !m_CollisionTree.intersect(samplePos, samplePos + samples[k]) )
					AO += 1.0f / CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count;
			}
			lMap[t * LMTexWidth + s] = (lMap[t * LMTexWidth + s] & 0x00FFFFFF) + ((uint32)(AO * 255.0f) << 24);
		
			fProgress = 0.5f + float(t * LMTexWidth + s) / float(LMTexHeight * LMTexWidth) * 0.5f;
		}
	//	m_pLogger->writeToLog(String("illuminate Terrain...")+String(fProgress), ELL_INFORMATION);
	}


//	m_pRenderDevice->getOpenGLTerrainRenderer()->updateLightmapTexture(LMTexWidth, LMTexHeight, lMap);
/*	delete [] lMap;
	lMap = NULL;*/
	delete [] samples;
	samples = NULL;

	return lMap;
}



uint32* COpenGLWorldSystemManager::updateSceneObjectLightmapTexture( float &fProgress, ISGPObject* pSceneObj, uint32 nLMTexWidth, uint32 nLMTexHeight, Random* pRandom )
{
	if( !m_pWorldMap || !m_pTerrain || !pSceneObj )
		return NULL;


	// Create Sample direction
	Vector3D *samples = new Vector3D [CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count];
	Vector3D *samples2 = new Vector3D [CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count];
	
	for(uint32 x = 0; x < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; x++)
	{
		Vector3D v;
		do {
				v.x = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
				v.y = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
				v.z = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
		} while (v * v > 1.0f);
		samples[x] = v;
	}

	// Create lightmap Image in memory
	uint32 *lMap = new uint32 [nLMTexWidth * nLMTexHeight];
	memset(lMap, 0, nLMTexWidth * nLMTexHeight * sizeof(uint32));

	float du = 1.0f / (nLMTexWidth - 1);
	float dv = 1.0f / (nLMTexHeight - 1);

	float fLightColorRGB[3] = {0.0f};
	float fLightDistensy = 0.0f;
	Vector3D samplePos;
	Vector3D sampleNormal;

	Vector3D vertexPos;
	Vector3D lightPos;
	Vector3D lightVec;
	Vector3D lightVecNor;

	CStaticMeshInstance *pInstance = m_SceneIDToInstanceMap[pSceneObj->getSceneObjectID()];
	Matrix4x4 modelMatrix = pInstance->getModelMatrix();

	CMF1FileResource *pMF1Res = m_pRenderDevice->GetModelManager()->getModelByID(pInstance->getMF1ModelResourceID());
	CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;
	jassert( pMF1Model );

	Vector4D SunColor = m_pRenderDevice->getOpenGLSkydomeRenderer()->getSunColorAndIntensity();

	Array<ISGPLightObject*> LightObjectArray;
	getAllIlluminatedLight(LightObjectArray, pSceneObj);

	for(uint32 t = 0; t < nLMTexHeight; t++)
	{
		for(uint32 s = 0; s < nLMTexWidth; s++)
		{
			if( pMF1Model->GetMeshPointFromSecondTexCoord( samplePos, sampleNormal, Vector2D(s * du, t * dv), modelMatrix ) )
			{
				// Pass 1, illuminate using the main light
				fLightColorRGB[0] = fLightColorRGB[1] = fLightColorRGB[2] = 0.0f;

				ISGPLightObject** pEnd = LightObjectArray.end();
				for( ISGPLightObject** pBegin = LightObjectArray.begin(); pBegin < pEnd; pBegin++ )
				{
					fLightDistensy = 0.0f;

					vertexPos = samplePos;
					lightPos.Set( (*pBegin)->m_fPosition[0], (*pBegin)->m_fPosition[1], (*pBegin)->m_fPosition[2] );
				
					lightVec = lightPos - vertexPos;
					float fdistance = lightVec.GetLength();
					if( fdistance >= (*pBegin)->m_fRange )
						continue;

					float atten = 1.0f / ( (*pBegin)->m_fAttenuation0 +
						(*pBegin)->m_fAttenuation1 * fdistance +
						(*pBegin)->m_fAttenuation2 * fdistance * fdistance );


					lightVecNor = lightVec;
					lightVecNor.Normalize();
					float diffuse = lightVecNor * sampleNormal;
					if( diffuse > 0 )
					{
						//m_CollisionTree.pushSphere(vertexPos, CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset);
						vertexPos += sampleNormal * CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset;
						for(uint32 k = 0; k < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; k++)
						{
							if( !m_CollisionTree.intersect( lightPos + samples[k] * (*pBegin)->m_fLightSize, vertexPos ) )
								fLightDistensy += 1.0f / CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count;
						}

						fLightColorRGB[0] += (*pBegin)->m_fDiffuseColor[0] * atten * fLightDistensy * diffuse;
						fLightColorRGB[1] += (*pBegin)->m_fDiffuseColor[1] * atten * fLightDistensy * diffuse;
						fLightColorRGB[2] += (*pBegin)->m_fDiffuseColor[2] * atten * fLightDistensy * diffuse;
					}
				}

				// Global Sun Direction Light
				lightVecNor = m_pRenderDevice->GetWorldSystemManager()->getWorldSun()->getNormalizedSunDirection();
				float diffuse = lightVecNor * sampleNormal;
				if( diffuse > 0 )
				{
					fLightColorRGB[0] += SunColor.x * diffuse;
					fLightColorRGB[1] += SunColor.y * diffuse;
					fLightColorRGB[2] += SunColor.z * diffuse;
				}
				fLightColorRGB[0] = jlimit(0.0f, 1.0f, fLightColorRGB[0]);
				fLightColorRGB[1] = jlimit(0.0f, 1.0f, fLightColorRGB[1]);
				fLightColorRGB[2] = jlimit(0.0f, 1.0f, fLightColorRGB[2]);

				lMap[t * nLMTexWidth + s] =	0xFF000000 +
											((uint32)(255.0f * fLightColorRGB[0]) << 16) +
											((uint32)(255.0f * fLightColorRGB[1]) << 8)  +
											(uint32)(255.0f * fLightColorRGB[2]);
				

				// Pass 2, indirect lumination (AO)
				vertexPos = samplePos;
				//m_CollisionTree.pushSphere(vertexPos, CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset);
				vertexPos += sampleNormal * CSGPLightMapGenConfig::getInstance()->m_fLightMap_Collision_Offset;

				for(uint32 x = 0; x < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; x++)
				{
					Vector3D v;
					do {
						v.x = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
						v.y = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
						v.z = pRandom->nextInt(65536)/65535.0f * 2.0f - 1.0f;
					} while ((v * v > 1.0f) || (sampleNormal * v < 0));
					samples2[x] = v * CSGPLightMapGenConfig::getInstance()->m_fLightMap_AO_Distance;
				}

				float AO = 0.0f;
				for(uint32 k = 0; k < CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count; k++)
				{
					if( !m_CollisionTree.intersect(vertexPos, vertexPos + samples2[k]) )
						AO += 1.0f / CSGPLightMapGenConfig::getInstance()->m_iLightMap_Sample_Count;
				}
				lMap[t * nLMTexWidth + s] = (lMap[t * nLMTexWidth + s] & 0x00FFFFFF) + ((uint32)(AO * 255.0f) << 24);
			}
			fProgress = float(t * nLMTexWidth + s) / float(nLMTexHeight * nLMTexWidth);
		}
	//	m_pLogger->writeToLog(String("illuminate Object ")+String(pSceneObj->getSceneObjectName())+String(fProgress), ELL_INFORMATION);
	}




//	pInstance->updateLightmapTexture(nLMTexWidth, nLMTexHeight, lMap);

/*	delete [] lMap;
	lMap = NULL;*/
	delete [] samples;
	samples = NULL;	
	delete [] samples2;
	samples2 = NULL;

	return lMap;
}

void COpenGLWorldSystemManager::getAllIlluminatedLight(Array<ISGPLightObject*>& LightObjectArray, ISGPObject* pSceneObj)
{
	jassert(pSceneObj);

	ISGPLightObject** pEnd = m_LightObjectArray.end();
	for( ISGPLightObject** pBegin = m_LightObjectArray.begin(); pBegin < pEnd; pBegin++ )
	{
		const OBBox& boundingbox = pSceneObj->getBoundingBox();
		AABBox lightBox(	Vector3D( (*pBegin)->m_fPosition[0] - (*pBegin)->m_fRange, (*pBegin)->m_fPosition[1] - (*pBegin)->m_fRange, (*pBegin)->m_fPosition[2] - (*pBegin)->m_fRange ),
							Vector3D( (*pBegin)->m_fPosition[0] + (*pBegin)->m_fRange, (*pBegin)->m_fPosition[1] + (*pBegin)->m_fRange, (*pBegin)->m_fPosition[2] + (*pBegin)->m_fRange )	);
		if( OBBox( &lightBox ).Intersects(boundingbox) )
			LightObjectArray.add( *pBegin );
	}
}

//==============================================================================
//==============================================================================






void COpenGLWorldSystemManager::initializeTerrainRenderer(bool bLoadFromMap)
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
	if( !bLoadFromMap )
	{
		uint32* pMinimapData = getWorldMap()->m_WorldChunkColorMiniMapTextureData;
		for( uint32 row = 0; row < m_pTerrain->GetTerrainChunkSize(); row++ )
			for( uint32 col = 0; col < m_pTerrain->GetTerrainChunkSize(); col++ )
				m_pRenderDevice->getOpenGLTerrainRenderer()->generateChunkColorMinimapData(pMinimapData, m_pTerrain->GetTerrainChunkSize(), row, col);
	}
	m_pRenderDevice->getOpenGLTerrainRenderer()->registerColorMinimapTexture(m_pWorldMap, String(L"ColorMinimap-")+getWorldName(), m_pTerrain->GetTerrainChunkSize());

	// Register terrain lightmap texture
	m_pRenderDevice->getOpenGLTerrainRenderer()->registerLightmapTexture(getWorldName(), String(L"TerrainLightmap.dds"));
}

void COpenGLWorldSystemManager::releaseTerrainRenderer()
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

void COpenGLWorldSystemManager::getVisibleSceneObjectArray(const Frustum& ViewFrustum, const Array<CSGPTerrainChunk*>& VisibleChunkArray, Array<ISGPObject*>& VisibleSceneObjectArray)
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

