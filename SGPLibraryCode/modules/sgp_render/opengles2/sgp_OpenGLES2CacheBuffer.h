#ifndef __SGP_OPENGLES2CACHEBUFFER_HEADER__
#define __SGP_OPENGLES2CACHEBUFFER_HEADER__


class COpenGLES2VertexCacheManager;

class COpenGLES2StaticBuffer
{
public:
	COpenGLES2StaticBuffer() : nSBID(0), nStride(0), nNumVerts(0), nNumIndis(0), nNumTris(0), pVBO(0) {}
	~COpenGLES2StaticBuffer();


	uint32  nSBID;
	uint32	nStride;

	uint32	nNumVerts;
	uint32	nNumIndis;
	uint32	nNumTris;


	//ISGPMaterialSystem::SGPMaterialInfo		MaterialModifier;
	COpenGLES2VertexBufferObject*			pVBO;	
	AABBox									BoundingBox;
	SGPSkin									MaterialSkin;
};


/////////////////////////////////////////////////////////////////////////////////////////
/**
 * Class for batching together polygons to be rendered in dynamic buffers.
 * Will flush content if requested or if max number of verts/indis in
 * cache is reached.
 */
class COpenGLES2DynamicBuffer
{
public:
	COpenGLES2DynamicBuffer(uint32 nVertsMax, uint32 nIndisMax, uint32 nStride, 
							SGP_VERTEX_TYPE vertextype,
							COpenGLES2VertexCacheManager *pVCManager );
	~COpenGLES2DynamicBuffer();

	

    void Add(uint32 nVertexNum, uint32 nIndexNum, const void *pVerts, const uint16 *pIndis);

	inline bool	IsCapacityEnough(uint32 nVertexNum, uint32 nIndexNum)
	{
		if( nIndexNum == 0 )
			return (m_nNumVerts+nVertexNum <= m_nNumVertsMax) &&
				 (m_nNumIndis+nVertexNum <= m_nNumIndisMax);
		else
			return (m_nNumVerts+nVertexNum <= m_nNumVertsMax) &&
				 (m_nNumIndis+nIndexNum <= m_nNumIndisMax);
	}


	void SetMaterialSkin(const SGPSkin& skin);
	bool IsUseSameSkin(const SGPSkin& skin);

	void Commit();
	void Clear(void);



    inline bool IsNotEmpty(void) { return (m_nNumVerts > 0); }
    inline uint32  GetVertexNum(void) { return m_nNumVerts; }	
	inline uint32  GetIndexNum(void) { return m_nNumIndis; }

public:
    COpenGLES2VertexCacheManager			*m_pVCManager;
	COpenGLES2VertexBufferObject			*m_pVBO;
	//ISGPMaterialSystem::SGPMaterialInfo	m_MaterialModifier;

	AABBox			m_BoundingBox;	  // AABB Bounding box
	SGPSkin			m_MaterialSkin;	  // Material Skin

    uint32			m_nNumVertsMax;   // maximum verts in buffer
    uint32			m_nNumIndisMax;   // maximum indices in buffer
    uint32			m_nNumVerts;      // actual number in buffer
    uint32			m_nNumIndis;      // actual number in buffer
    uint32			m_nStride;        // stride of one vertex element

	bool			m_bCommited;	  // be commited to GPU?
};



#endif		// __SGP_OPENGLES2CACHEBUFFER_HEADER__