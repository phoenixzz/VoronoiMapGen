#ifndef __SGP_QUADTREE_HEADER__
#define __SGP_QUADTREE_HEADER__

class CSGPQuadTree
{
private:	
	struct NodeType
	{
        float positionX, positionZ, width;			// Node X-Z center and Node width
		uint32 triangleCount;						// all triangle count in this node
		AABBox NodeBoundingBox;						// the axis aligned bounding box of this node
        NodeType* nodes[4];							// 4 children subnodes
		Array< CSGPTerrainChunk* > terrainchunks;	// terrain chunks array in this node
	};



public:
	CSGPQuadTree();
	~CSGPQuadTree();

	//! creates the Quad tree from terrain
	void InitializeFromTerrain(CSGPTerrain* pTerrain);
	void Shutdown();
	void GetVisibleTerrainChunk(NodeType* pNode, const Frustum& ViewFrustum, Array<CSGPTerrainChunk*>& VisibleChunkArray);



	inline NodeType* GetRootNode() { return m_parentNode; }

private:
	void CreateTreeNode(CSGPTerrain* pTerrain, NodeType* node, float positionX, float positionZ, float width);
	uint32 CountTriangles(CSGPTerrain* pTerrain, float positionX, float positionZ, float width);

	void ReleaseNode(NodeType* node);

private:	
	NodeType* m_parentNode;

	// Using 1,024 triangles per quad as the criteria for splitting nodes in the quad tree.
	// Note that making this number too low will cause the tree to be incredibly more complex and
	// hence will exponentially increase the time it takes to construct it. 
	static const int MAX_QUAD_TRIANGLES = 1024;
};



#endif		// __SGP_QUADTREE_HEADER__