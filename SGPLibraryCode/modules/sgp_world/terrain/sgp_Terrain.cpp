
void CSGPTerrain::InitializeCreateHeightmap(SGP_TERRAIN_SIZE ChunkSize, bool bPerlinNoise, uint16 MaxHeight)
{
	m_terrainChunkSize = ChunkSize;

	SetTerrainMaxHeight(MaxHeight);

	if( !m_heightMap )
	{
		m_heightMap = new uint16 [ GetVertexCount() ];
	}	

	if( bPerlinNoise )
	{
		GeneratePerlinNoiseToHeightmap( 0.01f, 5, 0.6f );
	}
	else
	{
		memset( m_heightMap, 0, sizeof(uint16) * GetVertexCount() );
	}

	// create trunks
	GenTerrainChunks();

	// create normal data
	CreateNormalTable();
}

void CSGPTerrain::LoadCreateHeightmap(SGP_TERRAIN_SIZE ChunkSize, const uint16* pHeightData, uint16 MaxHeight)
{
	m_terrainChunkSize = ChunkSize;
	SetTerrainMaxHeight(MaxHeight);

	if( !m_heightMap )
	{
		m_heightMap = new uint16 [ GetVertexCount() ];
	}

	memcpy( m_heightMap, pHeightData, sizeof(uint16) * GetVertexCount() );

	// create trunks
	GenTerrainChunks();
}

void CSGPTerrain::GenTerrainChunks()
{
	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			CSGPTerrainChunk* pChunk = new CSGPTerrainChunk( i, j, (uint16)(j*m_terrainChunkSize+i), this );
			m_TerrainChunks.add( pChunk );
		}
	}	
}

void CSGPTerrain::CreateNormalTable()
{
	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			m_TerrainChunks[j*m_terrainChunkSize+i]->CreateChunkNormalTable();
		}
	}

	// Average chunk Normal
	for( int32 j=0; j<m_terrainChunkSize; j++ )
	{
		for( int32 i=0; i<m_terrainChunkSize-1; i++ )
		{
			AverageChunkBorderNormal_Horizontal(j*m_terrainChunkSize+i, j*m_terrainChunkSize+i+1);
		}
	}

	for( int32 i=0; i<m_terrainChunkSize; i++ )
	{
		for( int32 j=0; j<m_terrainChunkSize-1; j++ )
		{
			AverageChunkBorderNormal_Vertical(j*m_terrainChunkSize+i, (j+1)*m_terrainChunkSize+i);
		}
	}

	NormalizeAllChunkNormal();
}

void CSGPTerrain::LoadCreateNormalTable(const float* pNormalData, const float* pTangentData, const float* pBinormalData)
{
	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			m_TerrainChunks[j*m_terrainChunkSize+i]->SetChunkNormalTable(pNormalData, pTangentData, pBinormalData);
		}
	}
}

void CSGPTerrain::SaveNormalTable(float* pNormalData, float* pTangentData, float* pBinormalData)
{
	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			m_TerrainChunks[j*m_terrainChunkSize+i]->GetChunkNormalTable(pNormalData, pTangentData, pBinormalData);
		}
	}
}

uint32 CSGPTerrain::GetChunkIndex(float Pos_x, float Pos_z)
{
	int chunkIdx_x = int( Pos_x / (SGPTT_TILENUM*SGPTT_TILE_METER) );
	int chunkIdx_z = m_terrainChunkSize - 1 - int( Pos_z / (SGPTT_TILENUM*SGPTT_TILE_METER) );

	// If the position is ouside of terrain, return 0xFFFFFFFF
	if( chunkIdx_x < 0 || chunkIdx_x > m_terrainChunkSize ||
		chunkIdx_z < 0 || chunkIdx_z > m_terrainChunkSize )
		return 0xFFFFFFFF;
		

	chunkIdx_x = jlimit( 0, m_terrainChunkSize-1, chunkIdx_x );
	chunkIdx_z = jlimit( 0, m_terrainChunkSize-1, chunkIdx_z );

	return chunkIdx_z * m_terrainChunkSize + chunkIdx_x;
}

bool CSGPTerrain::GetChunkIndex(uint8& index_x, uint8& index_z, float Pos_x, float Pos_z)
{
	int chunkIdx_x = int( Pos_x / (SGPTT_TILENUM*SGPTT_TILE_METER) );
	int chunkIdx_z = m_terrainChunkSize - 1 - int( Pos_z / (SGPTT_TILENUM*SGPTT_TILE_METER) );

	// If the position is ouside of terrain, return false
	if( chunkIdx_x < 0 || chunkIdx_x > m_terrainChunkSize ||
		chunkIdx_z < 0 || chunkIdx_z > m_terrainChunkSize )
		return false;

	index_x = uint8( jlimit( 0, m_terrainChunkSize-1, chunkIdx_x ) );
	index_z = uint8( jlimit( 0, m_terrainChunkSize-1, chunkIdx_z ) );

	return true;
}

float CSGPTerrain::GetTerrainHeight(float Pos_x, float Pos_z)
{
	int chunkIdx_x = int( Pos_x / (SGPTT_TILENUM*SGPTT_TILE_METER) );
	int chunkIdx_z = m_terrainChunkSize - 1 - int( Pos_z / (SGPTT_TILENUM*SGPTT_TILE_METER) );

	// If the position is ouside of terrain, return 0
	if( chunkIdx_x < 0 || chunkIdx_x > m_terrainChunkSize ||
		chunkIdx_z < 0 || chunkIdx_z > m_terrainChunkSize )
		return 0;

	chunkIdx_x = jlimit( 0, (m_terrainChunkSize-1), chunkIdx_x );
	chunkIdx_z = jlimit( 0, (m_terrainChunkSize-1), chunkIdx_z );

	float chunk_offsetx = Pos_x - chunkIdx_x * SGPTT_TILENUM * SGPTT_TILE_METER;
	float chunk_offsetz = m_terrainChunkSize * SGPTT_TILENUM * SGPTT_TILE_METER - Pos_z - chunkIdx_z * SGPTT_TILENUM * SGPTT_TILE_METER;

	return m_TerrainChunks[chunkIdx_z*m_terrainChunkSize + chunkIdx_x]->GetTerrainHeight(chunk_offsetx, chunk_offsetz);
}

float CSGPTerrain::GetRealTerrainHeight(float Pos_x, float Pos_z)
{
	int chunkIdx_x = int( Pos_x / (SGPTT_TILENUM*SGPTT_TILE_METER) );
	int chunkIdx_z = m_terrainChunkSize - 1 - int( Pos_z / (SGPTT_TILENUM*SGPTT_TILE_METER) );

	// If the position is ouside of terrain, return 0
	if( chunkIdx_x < 0 || chunkIdx_x > m_terrainChunkSize ||
		chunkIdx_z < 0 || chunkIdx_z > m_terrainChunkSize )
		return 0;

	chunkIdx_x = jlimit( 0, (m_terrainChunkSize-1), chunkIdx_x );
	chunkIdx_z = jlimit( 0, (m_terrainChunkSize-1), chunkIdx_z );

	float chunk_offsetx = Pos_x - chunkIdx_x * SGPTT_TILENUM * SGPTT_TILE_METER;
	float chunk_offsetz = m_terrainChunkSize * SGPTT_TILENUM * SGPTT_TILE_METER - Pos_z - chunkIdx_z * SGPTT_TILENUM * SGPTT_TILE_METER;

	return m_TerrainChunks[chunkIdx_z*m_terrainChunkSize + chunkIdx_x]->GetRealTerrainHeight(Pos_x, Pos_z, chunk_offsetx, chunk_offsetz);
}

Vector3D CSGPTerrain::GetTerrainNormal(float Pos_x, float Pos_z) const
{
	int chunkIdx_x = int( Pos_x / (SGPTT_TILENUM*SGPTT_TILE_METER) );
	int chunkIdx_z = m_terrainChunkSize - 1 - int( Pos_z / (SGPTT_TILENUM*SGPTT_TILE_METER) );

	// If the position is ouside of terrain, return 0
	if( chunkIdx_x < 0 || chunkIdx_x > m_terrainChunkSize ||
		chunkIdx_z < 0 || chunkIdx_z > m_terrainChunkSize )
		return Vector3D(0,0,0);

	chunkIdx_x = jlimit( 0, m_terrainChunkSize-1, chunkIdx_x );
	chunkIdx_z = jlimit( 0, m_terrainChunkSize-1, chunkIdx_z );

	float chunk_offsetx = Pos_x - chunkIdx_x * SGPTT_TILENUM * SGPTT_TILE_METER;
	float chunk_offsetz = m_terrainChunkSize * SGPTT_TILENUM * SGPTT_TILE_METER - Pos_z - chunkIdx_z * SGPTT_TILENUM * SGPTT_TILE_METER;

	return m_TerrainChunks[chunkIdx_z*m_terrainChunkSize + chunkIdx_x]->GetTerrainNormal(chunk_offsetx, chunk_offsetz);
}

void CSGPTerrain::SetTerrainGrassLayerData(float Pos_x, float Pos_z, const SGPGrassCluster& GrassData)
{
	int chunkIdx_x = int( Pos_x / (SGPTT_TILENUM*SGPTT_TILE_METER) );
	int chunkIdx_z = m_terrainChunkSize - 1 - int( Pos_z / (SGPTT_TILENUM*SGPTT_TILE_METER) );

	// If the position is ouside of terrain, return 0
	if( chunkIdx_x < 0 || chunkIdx_x > m_terrainChunkSize ||
		chunkIdx_z < 0 || chunkIdx_z > m_terrainChunkSize )
		return;

	chunkIdx_x = jlimit( 0, m_terrainChunkSize-1, chunkIdx_x );
	chunkIdx_z = jlimit( 0, m_terrainChunkSize-1, chunkIdx_z );

	float chunk_offsetx = Pos_x - chunkIdx_x * SGPTT_TILENUM * SGPTT_TILE_METER;
	float chunk_offsetz = m_terrainChunkSize * SGPTT_TILENUM * SGPTT_TILE_METER - Pos_z - chunkIdx_z * SGPTT_TILENUM * SGPTT_TILE_METER;
	
	m_TerrainChunks[chunkIdx_z*m_terrainChunkSize + chunkIdx_x]->SetChunkGrassClusterInPosition(chunk_offsetx, chunk_offsetz, GrassData);
}

void CSGPTerrain::CreateLODHeights()
{
	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			m_TerrainChunks[j*m_terrainChunkSize+i]->CreateLODHeights();
		}
	}
}

void CSGPTerrain::UpdateBoundingBox()
{
	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			m_TerrainChunks[j*m_terrainChunkSize+i]->UpdateAABB();
		}
	}	
}

void CSGPTerrain::AddSceneObject( const ISGPObject* pObj, uint32 chunkIndex )
{
	m_TerrainChunks[chunkIndex]->AddSceneObject( pObj );
}

void CSGPTerrain::RemoveSceneObject( const ISGPObject* pObj, uint32 chunkIndex )
{
	m_TerrainChunks[chunkIndex]->RemoveSceneObject( pObj );
}





void CSGPTerrain::GeneratePerlinNoiseToHeightmap(float scale, int32 octaves, float falloff)
{
	CPerlinNoise perlin(Time::currentTimeMillis());
	uint32 index = 0;

	for( int height=0; height <= (int)m_terrainChunkSize*SGPTT_TILENUM; height++ )
	{
		for( int width=0; width <= (int)m_terrainChunkSize*SGPTT_TILENUM; width++ )
		{
			float accum = 0;
			float frequency = scale;
			float amplitude = 1.0f;

			for(int32 i=0; i<octaves; ++i)
			{
				accum += perlin.noise(width, height, frequency) * amplitude;
				amplitude *= falloff;
				frequency *= 2.0f;
			}

			accum = jlimit( -1.0f, 1.0f, accum );
			accum *= 0.5f;
			accum += 0.5f;

			m_heightMap[index++] = (uint16)(accum * m_TerrainMaxHeight);

		}
	}
}

void CSGPTerrain::AverageChunkBorderNormal_Horizontal(uint32 chunkIdx0, uint32 chunkIdx1)
{
	float v0_Normal[3];
	float v1_Normal[3];

	for( int i=0; i<=SGPTT_TILENUM; i++ )
	{
		v0_Normal[0] = m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_horizontal_right[i]].fNormal[0];
		v0_Normal[1] = m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_horizontal_right[i]].fNormal[1];
		v0_Normal[2] = m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_horizontal_right[i]].fNormal[2];

		v1_Normal[0] = m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_horizontal_left[i]].fNormal[0];
		v1_Normal[1] = m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_horizontal_left[i]].fNormal[1];
		v1_Normal[2] = m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_horizontal_left[i]].fNormal[2];

		m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_horizontal_left[i]].fNormal[0] += v0_Normal[0];
		m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_horizontal_left[i]].fNormal[1] += v0_Normal[1];
		m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_horizontal_left[i]].fNormal[2] += v0_Normal[2];

		m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_horizontal_right[i]].fNormal[0] += v1_Normal[0];
		m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_horizontal_right[i]].fNormal[1] += v1_Normal[1];
		m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_horizontal_right[i]].fNormal[2] += v1_Normal[2];
	}
}

void CSGPTerrain::AverageChunkBorderNormal_Vertical(uint32 chunkIdx0, uint32 chunkIdx1)
{
	float v0_Normal[3];
	float v1_Normal[3];

	for( int i=0; i<=SGPTT_TILENUM; i++ )
	{
		v0_Normal[0] = m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_vertical_down[i]].fNormal[0];
		v0_Normal[1] = m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_vertical_down[i]].fNormal[1];
		v0_Normal[2] = m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_vertical_down[i]].fNormal[2];

		v1_Normal[0] = m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_vertical_up[i]].fNormal[0];
		v1_Normal[1] = m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_vertical_up[i]].fNormal[1];
		v1_Normal[2] = m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_vertical_up[i]].fNormal[2];

		m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_vertical_up[i]].fNormal[0] += v0_Normal[0];
		m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_vertical_up[i]].fNormal[1] += v0_Normal[1];
		m_TerrainChunks[chunkIdx1]->m_ChunkTerrainVertex[border_index_tile_vertical_up[i]].fNormal[2] += v0_Normal[2];

		m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_vertical_down[i]].fNormal[0] += v1_Normal[0];
		m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_vertical_down[i]].fNormal[1] += v1_Normal[1];
		m_TerrainChunks[chunkIdx0]->m_ChunkTerrainVertex[border_index_tile_vertical_down[i]].fNormal[2] += v1_Normal[2];
	}
}

void CSGPTerrain::NormalizeAllChunkNormal()
{
	Vector3D vNormal;

	for( uint8 j=0; j<m_terrainChunkSize; j++ )
	{
		for( uint8 i=0; i<m_terrainChunkSize; i++ )
		{
			for( uint16 idx=0; idx<(SGPTT_TILENUM+1)*(SGPTT_TILENUM+1); idx++ )
			{
				vNormal.Set(
					m_TerrainChunks[j*m_terrainChunkSize+i]->m_ChunkTerrainVertex[idx].fNormal[0],
					m_TerrainChunks[j*m_terrainChunkSize+i]->m_ChunkTerrainVertex[idx].fNormal[1],
					m_TerrainChunks[j*m_terrainChunkSize+i]->m_ChunkTerrainVertex[idx].fNormal[2] );
				vNormal.Normalize();

				m_TerrainChunks[j*m_terrainChunkSize+i]->m_ChunkTerrainVertex[idx].fNormal[0] = vNormal.x;
				m_TerrainChunks[j*m_terrainChunkSize+i]->m_ChunkTerrainVertex[idx].fNormal[1] = vNormal.y;
				m_TerrainChunks[j*m_terrainChunkSize+i]->m_ChunkTerrainVertex[idx].fNormal[2] = vNormal.z;
			}
		}
	}
}


