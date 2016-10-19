

CSGPTerrainChunk::CSGPTerrainChunk(uint8 x, uint8 z, uint16 index, CSGPTerrain* pTerrain) 
	: m_ChunkIndex_x(x), m_ChunkIndex_z(z), 
	  m_TerrainChunkIndex(index), m_ObjectTriangleCount(0), 
	  m_pGrassLayerData(NULL), m_nGrassLayerDataNum(0),
	  m_pTerrain(pTerrain)
{
	m_TerrainTriangleCount = SGPTL_LOD0_TRIANGLESINCHUNK;

	// Fill Height data and Fill Vertex data
	uint32 offsetindex = (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM+1) * m_ChunkIndex_z * SGPTT_TILENUM +
		m_ChunkIndex_x * SGPTT_TILENUM;

	float uv_offset = 1.0f / (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM);

	for( int j=0; j<=SGPTT_TILENUM; j++ )
	{
		for( int i=0; i<=SGPTT_TILENUM; i++ )
		{
			float y = m_pTerrain->GetHeightMap()[offsetindex + i];
			float x = (float)(m_ChunkIndex_x * SGPTT_TILENUM * SGPTT_TILE_METER + i * SGPTT_TILE_METER);
			float z = (float)
				((m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM*SGPTT_TILE_METER) - 
				(m_ChunkIndex_z * SGPTT_TILENUM * SGPTT_TILE_METER + j * SGPTT_TILE_METER));

			m_ChunkTerrainHeight[j*(SGPTT_TILENUM+1)+i].fHeight = m_pTerrain->GetHeightMap()[offsetindex + i];

			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].x = x;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].y = y;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].z = z;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].w = y;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[0] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[1] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[2] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].tu0 = 1.0f/SGPTT_TILENUM * i;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].tv0 = 1.0f/SGPTT_TILENUM * j;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].tu1 = (float)((offsetindex + i) % (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM + 1)) * uv_offset;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].tv1 = (float)(offsetindex + i) / (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM + 1) * uv_offset;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[0] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[1] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[2] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[0] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[1] = 0;
			m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[2] = 0;
		}

		offsetindex += m_pTerrain->GetTerrainChunkSize() * SGPTT_TILENUM + 1;
	}
}

void CSGPTerrainChunk::Shutdown() 
{ 
	m_ChunkObjects.clear();
	m_pTerrain = NULL; 
}

void CSGPTerrainChunk::CreateChunkNormalTable()
{
	for( uint16 index=0; index < SGPTL_LOD0_TRIANGLESINCHUNK*3; index += 3 )
	{
		Vector4D v0(	m_ChunkTerrainVertex[base_index_tile[index]].x, 
						m_ChunkTerrainVertex[base_index_tile[index]].y,
						m_ChunkTerrainVertex[base_index_tile[index]].z );
		Vector4D v1(	m_ChunkTerrainVertex[base_index_tile[index+1]].x,
						m_ChunkTerrainVertex[base_index_tile[index+1]].y,
						m_ChunkTerrainVertex[base_index_tile[index+1]].z );
		Vector4D v2(	m_ChunkTerrainVertex[base_index_tile[index+2]].x,
						m_ChunkTerrainVertex[base_index_tile[index+2]].y,
						m_ChunkTerrainVertex[base_index_tile[index+2]].z );

		Vector4D Normal;
		Normal.Cross(v2-v0, v1-v0);

		m_ChunkTerrainVertex[base_index_tile[index]].fNormal[0] += Normal.x;
		m_ChunkTerrainVertex[base_index_tile[index]].fNormal[1] += Normal.y;
		m_ChunkTerrainVertex[base_index_tile[index]].fNormal[2] += Normal.z;

		m_ChunkTerrainVertex[base_index_tile[index+1]].fNormal[0] += Normal.x;
		m_ChunkTerrainVertex[base_index_tile[index+1]].fNormal[1] += Normal.y;
		m_ChunkTerrainVertex[base_index_tile[index+1]].fNormal[2] += Normal.z;

		m_ChunkTerrainVertex[base_index_tile[index+2]].fNormal[0] += Normal.x;
		m_ChunkTerrainVertex[base_index_tile[index+2]].fNormal[1] += Normal.y;
		m_ChunkTerrainVertex[base_index_tile[index+2]].fNormal[2] += Normal.z;
	}

	CalculateTerrainVectors();
}

void CSGPTerrainChunk::SetChunkNormalTable(const float* pNormalData, const float* pTangentData, const float* pBinormalData)
{
	if( pNormalData || pTangentData || pBinormalData )
	{
		uint32 offsetindex = (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM+1) * m_ChunkIndex_z * SGPTT_TILENUM +
			m_ChunkIndex_x * SGPTT_TILENUM;

		for( int j=0; j<=SGPTT_TILENUM; j++ )
		{
			for( int i=0; i<=SGPTT_TILENUM; i++ )
			{
				if( pNormalData )
				{
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[0] = pNormalData[(offsetindex + i)*3+0];
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[1] = pNormalData[(offsetindex + i)*3+1];
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[2] = pNormalData[(offsetindex + i)*3+2];
				}
				if( pTangentData )
				{
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[0] = pTangentData[(offsetindex + i)*3+0];
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[1] = pTangentData[(offsetindex + i)*3+1];
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[2] = pTangentData[(offsetindex + i)*3+2];
				}
				if( pBinormalData )
				{
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[0] = pBinormalData[(offsetindex + i)*3+0];
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[1] = pBinormalData[(offsetindex + i)*3+1];
					m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[2] = pBinormalData[(offsetindex + i)*3+2];
				}
			}
			offsetindex += m_pTerrain->GetTerrainChunkSize() * SGPTT_TILENUM + 1;
		}
	}
}

void CSGPTerrainChunk::GetChunkNormalTable(float* pNormalData, float* pTangentData, float* pBinormalData)
{
	if( pNormalData || pTangentData || pBinormalData )
	{
		uint32 offsetindex = (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM+1) * m_ChunkIndex_z * SGPTT_TILENUM +
			m_ChunkIndex_x * SGPTT_TILENUM;

		for( int j=0; j<=SGPTT_TILENUM; j++ )
		{
			for( int i=0; i<=SGPTT_TILENUM; i++ )
			{
				if( pNormalData )
				{
					pNormalData[(offsetindex + i)*3+0] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[0];
					pNormalData[(offsetindex + i)*3+1] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[1];
					pNormalData[(offsetindex + i)*3+2] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fNormal[2];
				}
				if( pTangentData )
				{
					pTangentData[(offsetindex + i)*3+0] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[0];
					pTangentData[(offsetindex + i)*3+1] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[1];
					pTangentData[(offsetindex + i)*3+2] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fTangent[2];
				}
				if( pBinormalData )
				{
					pBinormalData[(offsetindex + i)*3+0] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[0];
					pBinormalData[(offsetindex + i)*3+1] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[1];
					pBinormalData[(offsetindex + i)*3+2] = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].fBinormal[2];
				}
			}
			offsetindex += m_pTerrain->GetTerrainChunkSize() * SGPTT_TILENUM + 1;
		}
	}
}

float CSGPTerrainChunk::GetTerrainHeight(float offsetx, float offsetz)
{
	int iMapX0 = int(offsetx / SGPTT_TILE_METER);
	int iMapZ0 = int(offsetz / SGPTT_TILE_METER);

	float fMapX = offsetx / SGPTT_TILE_METER - iMapX0;
	float fMapZ = offsetz / SGPTT_TILE_METER - iMapZ0;


	iMapX0 = jlimit( 0, SGPTT_TILENUM-1, iMapX0 );
	iMapZ0 = jlimit( 0, SGPTT_TILENUM-1, iMapZ0 );

	int iMapX1 = iMapX0 + 1;
	int iMapZ1 = iMapZ0 + 1;

	// read 4 map values
	float h0 = m_ChunkTerrainHeight[iMapX0 + iMapZ0 * (SGPTT_TILENUM+1)].fHeight;
	float h1 = m_ChunkTerrainHeight[iMapX1 + iMapZ0 * (SGPTT_TILENUM+1)].fHeight;
	float h2 = m_ChunkTerrainHeight[iMapX0 + iMapZ1 * (SGPTT_TILENUM+1)].fHeight;
	float h3 = m_ChunkTerrainHeight[iMapX1 + iMapZ1 * (SGPTT_TILENUM+1)].fHeight;

	float avgLo = (h1 * fMapX) + (h0 * (1.0f-fMapX));
	float avgHi = (h3 * fMapX) + (h2 * (1.0f-fMapX));

	return (avgHi * fMapZ) + (avgLo * (1.0f-fMapZ));
}

float CSGPTerrainChunk::GetRealTerrainHeight(float Pos_x, float Pos_z, float offsetx, float offsetz)
{
	int iMapX0 = int(offsetx / SGPTT_TILE_METER);
	int iMapZ0 = int(offsetz / SGPTT_TILE_METER);
	iMapX0 = jlimit( 0, SGPTT_TILENUM-1, iMapX0 );
	iMapZ0 = jlimit( 0, SGPTT_TILENUM-1, iMapZ0 );

	int triangleindex = (iMapX0 + iMapZ0 * SGPTT_TILENUM) * 2 * 3;
	Vector3D v0( m_ChunkTerrainVertex[base_index_tile[triangleindex + 0]].x,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 0]].y,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 0]].z );
	Vector3D v1( m_ChunkTerrainVertex[base_index_tile[triangleindex + 1]].x,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 1]].y,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 1]].z );
	Vector3D v2( m_ChunkTerrainVertex[base_index_tile[triangleindex + 2]].x,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 2]].y,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 2]].z );
	Vector3D v3( m_ChunkTerrainVertex[base_index_tile[triangleindex + 3]].x,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 3]].y,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 3]].z );
	Vector3D v4( m_ChunkTerrainVertex[base_index_tile[triangleindex + 4]].x,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 4]].y,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 4]].z );
	Vector3D v5( m_ChunkTerrainVertex[base_index_tile[triangleindex + 5]].x,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 5]].y,
				 m_ChunkTerrainVertex[base_index_tile[triangleindex + 5]].z );

	Ray testRay;
	testRay.Set( Vector3D(Pos_x, 0, Pos_z), Vector3D(0, 1, 0) );

	float dis = 0;
	if( testRay.Intersects(v0, v1, v2, false, &dis) )
		return dis;
	else if( testRay.Intersects(v3, v4, v5, false, &dis) )
		return dis;
	return dis;
}

Vector3D CSGPTerrainChunk::GetTerrainNormal(float offsetx, float offsetz) const
{
	int iMapX0 = uint16(offsetx / SGPTT_TILE_METER);
	int iMapZ0 = uint16(offsetz / SGPTT_TILE_METER);

	float fMapX = offsetx / SGPTT_TILE_METER - iMapX0;
	float fMapZ = offsetz / SGPTT_TILE_METER - iMapZ0;

	iMapX0 = jlimit( 0, SGPTT_TILENUM-1, iMapX0 );
	iMapZ0 = jlimit( 0, SGPTT_TILENUM-1, iMapZ0 );

	int iMapX1 = iMapX0 + 1;
	int iMapZ1 = iMapZ0 + 1;

	// read 4 map values
	Vector3D n0( m_ChunkTerrainVertex[iMapX0 + iMapZ0 * (SGPTT_TILENUM+1)].fNormal[0],
				m_ChunkTerrainVertex[iMapX0 + iMapZ0 * (SGPTT_TILENUM+1)].fNormal[1],
				m_ChunkTerrainVertex[iMapX0 + iMapZ0 * (SGPTT_TILENUM+1)].fNormal[2] );

	Vector3D n1( m_ChunkTerrainVertex[iMapX1 + iMapZ0 * (SGPTT_TILENUM+1)].fNormal[0],
				m_ChunkTerrainVertex[iMapX1 + iMapZ0 * (SGPTT_TILENUM+1)].fNormal[1],
				m_ChunkTerrainVertex[iMapX1 + iMapZ0 * (SGPTT_TILENUM+1)].fNormal[2] );

	Vector3D n2( m_ChunkTerrainVertex[iMapX0 + iMapZ1 * (SGPTT_TILENUM+1)].fNormal[0],
				m_ChunkTerrainVertex[iMapX0 + iMapZ1 * (SGPTT_TILENUM+1)].fNormal[1],
				m_ChunkTerrainVertex[iMapX0 + iMapZ1 * (SGPTT_TILENUM+1)].fNormal[2] );

	Vector3D n3( m_ChunkTerrainVertex[iMapX1 + iMapZ1 * (SGPTT_TILENUM+1)].fNormal[0],
				m_ChunkTerrainVertex[iMapX1 + iMapZ1 * (SGPTT_TILENUM+1)].fNormal[1],
				m_ChunkTerrainVertex[iMapX1 + iMapZ1 * (SGPTT_TILENUM+1)].fNormal[2] );

	Vector3D avgLo = (n1 * fMapX) + (n0 * (1.0f-fMapX));
	Vector3D avgHi = (n3 * fMapX) + (n2 * (1.0f-fMapX));

	Vector3D normal = (avgHi * fMapZ) + (avgLo * (1.0f-fMapZ));
	normal.Normalize();

	return normal;
}

void CSGPTerrainChunk::CreateLODHeights()
{
	for( uint32 i=0; i<(SGPTT_TILENUM>>1)*(SGPTT_TILENUM>>1)*2*3; i+=3 )
	{
		m_ChunkTerrainVertex[ (lod1_index_tile[i+0] + lod1_index_tile[i+1]) / 2 ].w = 
			0.5f * (m_ChunkTerrainVertex[ lod1_index_tile[i+0] ].y + m_ChunkTerrainVertex[ lod1_index_tile[i+1] ].y);
		m_ChunkTerrainVertex[ (lod1_index_tile[i+1] + lod1_index_tile[i+2]) / 2 ].w = 
			0.5f * (m_ChunkTerrainVertex[ lod1_index_tile[i+1] ].y + m_ChunkTerrainVertex[ lod1_index_tile[i+2] ].y);
		m_ChunkTerrainVertex[ (lod1_index_tile[i+0] + lod1_index_tile[i+2]) / 2 ].w = 
			0.5f * (m_ChunkTerrainVertex[ lod1_index_tile[i+0] ].y + m_ChunkTerrainVertex[ lod1_index_tile[i+2] ].y);
	}
}

// Calculate the normal, tangent, and binormal vectors for the chunk
void CSGPTerrainChunk::CalculateTerrainVectors()
{
	SGPTerrainVertex vertex1, vertex2, vertex3;
	Vector3D tangent, binormal;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for( uint32 i=0; i<SGPTL_LOD0_TRIANGLESINCHUNK*3; i+=3 )
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.x  = m_ChunkTerrainVertex[base_index_tile[i]].x;
		vertex1.y  = m_ChunkTerrainVertex[base_index_tile[i]].y;
		vertex1.z  = m_ChunkTerrainVertex[base_index_tile[i]].z;
		vertex1.tu0 = m_ChunkTerrainVertex[base_index_tile[i]].tu0;
		vertex1.tv0 = m_ChunkTerrainVertex[base_index_tile[i]].tv0;

		vertex2.x  = m_ChunkTerrainVertex[base_index_tile[i+1]].x;
		vertex2.y  = m_ChunkTerrainVertex[base_index_tile[i+1]].y;
		vertex2.z  = m_ChunkTerrainVertex[base_index_tile[i+1]].z;
		vertex2.tu0 = m_ChunkTerrainVertex[base_index_tile[i+1]].tu0;
		vertex2.tv0 = m_ChunkTerrainVertex[base_index_tile[i+1]].tv0;


		vertex3.x  = m_ChunkTerrainVertex[base_index_tile[i+2]].x;
		vertex3.y  = m_ChunkTerrainVertex[base_index_tile[i+2]].y;
		vertex3.z  = m_ChunkTerrainVertex[base_index_tile[i+2]].z;
		vertex3.tu0 = m_ChunkTerrainVertex[base_index_tile[i+2]].tu0;
		vertex3.tv0 = m_ChunkTerrainVertex[base_index_tile[i+2]].tv0;


		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		m_ChunkTerrainVertex[base_index_tile[i+2]].fTangent[0] = tangent.x;
		m_ChunkTerrainVertex[base_index_tile[i+2]].fTangent[1] = tangent.y;
		m_ChunkTerrainVertex[base_index_tile[i+2]].fTangent[2] = tangent.z;
		m_ChunkTerrainVertex[base_index_tile[i+2]].fBinormal[0] = binormal.x;
		m_ChunkTerrainVertex[base_index_tile[i+2]].fBinormal[1] = binormal.y;
		m_ChunkTerrainVertex[base_index_tile[i+2]].fBinormal[2] = binormal.z;

		m_ChunkTerrainVertex[base_index_tile[i+1]].fTangent[0] = tangent.x;
		m_ChunkTerrainVertex[base_index_tile[i+1]].fTangent[1] = tangent.y;
		m_ChunkTerrainVertex[base_index_tile[i+1]].fTangent[2] = tangent.z;
		m_ChunkTerrainVertex[base_index_tile[i+1]].fBinormal[0] = binormal.x;
		m_ChunkTerrainVertex[base_index_tile[i+1]].fBinormal[1] = binormal.y;
		m_ChunkTerrainVertex[base_index_tile[i+1]].fBinormal[2] = binormal.z;

		m_ChunkTerrainVertex[base_index_tile[i]].fTangent[0] = tangent.x;
		m_ChunkTerrainVertex[base_index_tile[i]].fTangent[1] = tangent.y;
		m_ChunkTerrainVertex[base_index_tile[i]].fTangent[2] = tangent.z;
		m_ChunkTerrainVertex[base_index_tile[i]].fBinormal[0] = binormal.x;
		m_ChunkTerrainVertex[base_index_tile[i]].fBinormal[1] = binormal.y;
		m_ChunkTerrainVertex[base_index_tile[i]].fBinormal[2] = binormal.z;
	}

}


void CSGPTerrainChunk::CalculateTangentBinormal(const SGPTerrainVertex& vertex1,
	const SGPTerrainVertex& vertex2, const SGPTerrainVertex& vertex3,
	Vector3D& tangent, Vector3D& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float flength;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu0 - vertex1.tu0;
	tvVector[0] = vertex2.tv0 - vertex1.tv0;

	tuVector[1] = vertex3.tu0 - vertex1.tu0;
	tvVector[1] = vertex3.tv0 - vertex1.tv0;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	flength = std::sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
			
	// Normalize the normal and then store it
	tangent.x = tangent.x / flength;
	tangent.y = tangent.y / flength;
	tangent.z = tangent.z / flength;

	// Calculate the length of this normal.
	flength = std::sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
			
	// Normalize the normal and then store it
	binormal.x = binormal.x / flength;
	binormal.y = binormal.y / flength;
	binormal.z = binormal.z / flength;

}



void CSGPTerrainChunk::RemoveSceneObject(const ISGPObject* pObj)
{
	m_ObjectTriangleCount -= pObj->getTriangleCount();

	m_ChunkObjects.removeAllInstancesOf( (ISGPObject*)pObj );

	UpdateAABB();
}

void CSGPTerrainChunk::AddSceneObject(const ISGPObject* pObj)
{
	AABBox ObjectAABB;
	ObjectAABB.Construct( &(pObj->getBoundingBox()) );
	m_BoundingBox += ObjectAABB;

	m_ObjectTriangleCount += pObj->getTriangleCount();

	m_ChunkObjects.add( (ISGPObject*)pObj );
}

void CSGPTerrainChunk::UpdateAABB()
{
	m_BoundingBox.vcMin.Set(0,0,0);
	m_BoundingBox.vcMax.Set(0,0,0);
	m_BoundingBox.vcCenter.Set(0,0,0);

	for( int i=0; i<(SGPTT_TILENUM+1)*(SGPTT_TILENUM+1); i++)
	{
		m_BoundingBox += Vector3D(m_ChunkTerrainVertex[i].x, m_ChunkTerrainVertex[i].y, m_ChunkTerrainVertex[i].z);
	}
	for( int i=0; i<m_ChunkObjects.size(); i++ )
	{
		AABBox ObjectAABB;
		ObjectAABB.Construct( &(m_ChunkObjects[i]->getBoundingBox()) );
		m_BoundingBox += ObjectAABB;
	}
}


// Load Chunk Grass data from memory
void CSGPTerrainChunk::SetChunkGrassCluster(SGPGrassCluster* pRawGrassData, uint32 nDataNum)
{
	m_pGrassLayerData = pRawGrassData;
	m_nGrassLayerDataNum = nDataNum;
}


// Set grass data from this terrain chunk in position (usually used for Editor)
void CSGPTerrainChunk::SetChunkGrassClusterInPosition(float offsetx, float offsetz, const SGPGrassCluster& RawGrassData)
{
	int iMapX0 = uint16(offsetx / SGPTT_TILE_METER * SGPTGD_GRASS_DIMISION);
	int iMapZ0 = uint16(offsetz / SGPTT_TILE_METER * SGPTGD_GRASS_DIMISION);

	iMapX0 = jlimit( 0, SGPTT_TILENUM*SGPTGD_GRASS_DIMISION-1, iMapX0 );
	iMapZ0 = jlimit( 0, SGPTT_TILENUM*SGPTGD_GRASS_DIMISION-1, iMapZ0 );

	m_pGrassLayerData[ iMapZ0 * SGPTT_TILENUM*SGPTGD_GRASS_DIMISION + iMapX0 ] = RawGrassData;
}


bool CSGPTerrainChunk::FlushTerrainChunkHeight()
{
	bool bChunkHeightChanged = false;

	uint32 offsetindex = (m_pTerrain->GetTerrainChunkSize()*SGPTT_TILENUM+1) * m_ChunkIndex_z * SGPTT_TILENUM +
		m_ChunkIndex_x * SGPTT_TILENUM;

	for( int j=0; j<=SGPTT_TILENUM; j++ )
	{
		for( int i=0; i<=SGPTT_TILENUM; i++ )
		{
			if( m_ChunkTerrainHeight[j*(SGPTT_TILENUM+1)+i].fHeight != m_pTerrain->GetHeightMap()[offsetindex + i] )
			{	
				bChunkHeightChanged = true;
				
				m_ChunkTerrainHeight[j*(SGPTT_TILENUM+1)+i].fHeight = m_pTerrain->GetHeightMap()[offsetindex + i];
				m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].y = m_pTerrain->GetHeightMap()[offsetindex + i];
				m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].w = m_ChunkTerrainVertex[j*(SGPTT_TILENUM+1)+i].y;
			}
		}

		offsetindex += m_pTerrain->GetTerrainChunkSize() * SGPTT_TILENUM + 1;
	}

	if( !bChunkHeightChanged )
		return false;

	CreateLODHeights();
	UpdateAABB();

	return true;
}

void CSGPTerrainChunk::AddTriangleCollisionSet(CollisionSet& coll)
{
	for( uint16 index=0; index < SGPTL_LOD0_TRIANGLESINCHUNK*3; index += 3 )
	{
		Vector3D v0(	m_ChunkTerrainVertex[base_index_tile[index]].x, 
						m_ChunkTerrainVertex[base_index_tile[index]].y,
						m_ChunkTerrainVertex[base_index_tile[index]].z );
		Vector3D v1(	m_ChunkTerrainVertex[base_index_tile[index+1]].x,
						m_ChunkTerrainVertex[base_index_tile[index+1]].y,
						m_ChunkTerrainVertex[base_index_tile[index+1]].z );
		Vector3D v2(	m_ChunkTerrainVertex[base_index_tile[index+2]].x,
						m_ChunkTerrainVertex[base_index_tile[index+2]].y,
						m_ChunkTerrainVertex[base_index_tile[index+2]].z );
		
		coll.addTriangle(v0, v1, v2, NULL);
	}
}