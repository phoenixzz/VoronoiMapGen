

CSGPQuadTree::CSGPQuadTree() : m_parentNode(NULL)
{
}

CSGPQuadTree::~CSGPQuadTree()
{
	Shutdown();
}

void CSGPQuadTree::InitializeFromTerrain(CSGPTerrain* pTerrain)
{
	// Calculate the center x,z and the width of the whole terrain.
	Vector2D terrain_center = pTerrain->GetTerrainCenter();
	float width = pTerrain->GetTerrainWidth();

	// Create the parent node for the quad tree.
	m_parentNode = new NodeType;

	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(pTerrain, m_parentNode, terrain_center.x, terrain_center.y, width);

}

void CSGPQuadTree::Shutdown()
{
	// Recursively release the quad tree data.
	if(m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = NULL;
	}
}

void CSGPQuadTree::GetVisibleTerrainChunk(NodeType* pNode, const Frustum& ViewFrustum, Array<CSGPTerrainChunk*>& VisibleChunkArray)
{
	if( pNode )
	{
		if( pNode->NodeBoundingBox.Intersects(ViewFrustum) )
		{
			GetVisibleTerrainChunk( pNode->nodes[0], ViewFrustum, VisibleChunkArray );
			GetVisibleTerrainChunk( pNode->nodes[1], ViewFrustum, VisibleChunkArray );
			GetVisibleTerrainChunk( pNode->nodes[2], ViewFrustum, VisibleChunkArray );
			GetVisibleTerrainChunk( pNode->nodes[3], ViewFrustum, VisibleChunkArray );

			for( int i=0; i<pNode->terrainchunks.size(); i++ )
			{
				if( pNode->terrainchunks[i]->m_BoundingBox.Intersects(ViewFrustum) )
					VisibleChunkArray.add( pNode->terrainchunks[i] );
			}
		}
	}
}







void CSGPQuadTree::CreateTreeNode(CSGPTerrain* pTerrain, NodeType* node, float positionX, float positionZ, float width)
{
	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// Initialize the triangle count to zero for the node.
	node->triangleCount = 0;

	// Initialize the children nodes of this node to null.
	node->nodes[0] = NULL;
	node->nodes[1] = NULL;
	node->nodes[2] = NULL;
	node->nodes[3] = NULL;

	// Initialize the terrain chunks of this node to null.
	node->terrainchunks.ensureStorageAllocated(4);

	// Count the number of triangles that are inside this node.
	uint32 numTriangles = CountTriangles(pTerrain, positionX, positionZ, width);

	// Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
	if(numTriangles == 0)
	{
		return;
	}

	// Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
	// If QuadNode smaller than one chunk, Do not split!
	float offsetX, offsetZ;
	if( (numTriangles > MAX_QUAD_TRIANGLES) && (width > SGPTT_TILE_METER * SGPTT_TILENUM) )
	{
		for(int i=0; i<4; i++)
		{
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// See if there are any triangles in the new node.
			uint32 count = CountTriangles(pTerrain, (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if(count > 0)
			{
				// If there are triangles inside where this new node would be then create the child node.
				node->nodes[i] = new NodeType;

				// Extend the tree starting from this new child node now.
				CreateTreeNode(pTerrain, node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
				
				node->NodeBoundingBox += node->nodes[i]->NodeBoundingBox;
			}
		}

		return;
	}

	// Case 3: If this node is not empty and the triangle count for it is less than the max then 
	// this node is at the bottom of the tree so the list of terrain chunks to store in it.
	node->triangleCount = numTriangles;



	// Go through all the terrain chunks in this zone
	uint32 center_x = uint32(positionX / SGPTT_TILE_METER / SGPTT_TILENUM);
	uint32 center_z = uint32(positionZ / SGPTT_TILE_METER / SGPTT_TILENUM);
	int32 center_width = int32(width / SGPTT_TILE_METER / SGPTT_TILENUM);
	
	uint32 chunk_index = 0;
	if( center_width == 1 )
	{
		chunk_index = (pTerrain->GetTerrainChunkSize() - 1 - center_z) * pTerrain->GetTerrainChunkSize() + center_x;
		node->terrainchunks.add( pTerrain->m_TerrainChunks[chunk_index] );
		node->NodeBoundingBox += pTerrain->m_TerrainChunks[chunk_index]->m_BoundingBox;
	}
	else
	{
		for( int32 j = -center_width/2; j<center_width/2; j++ )
		{
			for( int32 i = -center_width/2; i<center_width/2; i++ )
			{
				chunk_index = (pTerrain->GetTerrainChunkSize() - center_z + j) * pTerrain->GetTerrainChunkSize() +
					(center_x + i);
				node->terrainchunks.add( pTerrain->m_TerrainChunks[chunk_index] );
				node->NodeBoundingBox += pTerrain->m_TerrainChunks[chunk_index]->m_BoundingBox;
			}
		}
	}
	return;
}




void CSGPQuadTree::ReleaseNode(NodeType* node)
{
	// Recursively go down the tree and release the bottom nodes first.
	for(int i=0; i<4; i++)
	{
		if(node->nodes[i])
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// Release the four child nodes.
	for(int i=0; i<4; i++)
	{
		if(node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = NULL;
			node->terrainchunks.clear();
		}
	}

}



uint32 CSGPQuadTree::CountTriangles(CSGPTerrain* pTerrain, float positionX, float positionZ, float width)
{
	// Initialize the count to zero.
	uint32 count = 0;

	// Go through all the terrain chunks in this zone and check the triangle count
	uint32 center_x = uint32(positionX / SGPTT_TILE_METER / SGPTT_TILENUM);
	uint32 center_z = uint32(positionZ / SGPTT_TILE_METER / SGPTT_TILENUM);
	int32 center_width = int32(width / SGPTT_TILE_METER / SGPTT_TILENUM);

	uint32 chunk_index = 0;

	if( center_width == 1 )
	{
		chunk_index = (pTerrain->GetTerrainChunkSize() - 1 - center_z) * pTerrain->GetTerrainChunkSize() + center_x;
		count += pTerrain->m_TerrainChunks[chunk_index]->GetTriangleCount();
	}
	else
	{
		for( int32 j = -center_width/2; j<center_width/2; j++ )
		{
			for( int32 i = -center_width/2; i<center_width/2; i++ )
			{
				chunk_index = (pTerrain->GetTerrainChunkSize() - center_z + j) * pTerrain->GetTerrainChunkSize() +
					(center_x + i);

				count += pTerrain->m_TerrainChunks[chunk_index]->GetTriangleCount();
			}
		}
	}
	return count;
}