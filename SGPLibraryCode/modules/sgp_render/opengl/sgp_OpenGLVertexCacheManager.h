#ifndef __SGP_OPENGLVERTEXCACHEMANAGER_HEADER__
#define __SGP_OPENGLVERTEXCACHEMANAGER_HEADER__


#define CIRCLE_SLIDES_MAX 24

class COpenGLVertexCacheManager : public ISGPVertexCacheManager
{
public:
    COpenGLVertexCacheManager(COpenGLRenderDevice* pRenderDevice);
	virtual ~COpenGLVertexCacheManager();

    virtual uint32		CreateStaticBuffer(
											SGP_VERTEX_TYPE VertexType,
											const SGPSkin& skin,
											const AABBox& boundingbox,
											uint32  nVertexNum, 
											uint32  nIndexNum, 
											const void   *pVerts,
											const uint16 *pIndis );

	virtual uint32		CreateMF1MeshStaticBuffer(
											const SGPMF1Skin& MeshSkin,
											const SGPMF1Mesh& MF1Mesh,
											const SGPMF1BoneGroup* pBoneGroup,
											uint32 NumBoneGroup );

	virtual void*		GetStaticBufferByID(uint32 nSBufferID);

	virtual uint32		CreateTextureBufferObjectByID(uint32 ModelResourceID);
	virtual void		UpdateTextureBufferObjectByID(float* pBoneMatrixBuffer, uint32 nBoneCount, uint32 TBOID);
	virtual bool		BindTextureBufferObjectByID(uint32 TBOID, int iTextureUnit);

	virtual void		ClearAllTextureBufferObject();
	virtual void		ClearTextureBufferObject( uint32 nTBOID );

	virtual void		ClearAllStaticBuffer();
	virtual void		ClearStaticBuffer( uint32 nSBufferID );

	virtual void		RenderStaticBuffer( uint32 nSBufferID, const Matrix4x4& matWorld, const RenderBatchConfig& config );

	virtual void		RenderSkeletonMesh( uint32 nSBufferID, const Matrix4x4& matWorld, uint32 nTBOID, const RenderBatchConfig& config );
	virtual void		RenderSkeletonMesh( uint32 /*nSBufferID*/, const Matrix4x4& /*matWorld*/, float* /*pBoneMatrixBuffer*/, uint32 /*nBoneNum*/, const RenderBatchConfig& /*config*/ ) {}
      
    virtual void		RenderDynamicBuffer(SGP_VERTEX_TYPE VertexType,
											const SGPSkin& skin,
											uint32 nVertexNum, 
											uint32 nIndexNum, 
											const void   *pVerts,
											const uint16 *pIndis );

    virtual void		ForcedCommitAll(void);
	virtual void		ForcedClearAll(void);





    virtual void		RenderPoints(	uint32							 nVertexNum,
										const SGPVertex_UPOS_VERTEXCOLOR *pVerts );

    virtual void		RenderLines(	uint32							 nVertexNum,
										const SGPVertex_UPOS_VERTEXCOLOR *pVerts,
										bool							 bStrip );

    virtual void		RenderLine(		const float  *fStart,
										const float  *fEnd, 
										const Colour *pColor );

	virtual void		RenderTriangles(	uint32          nVertexNum,
											uint32			nIndexNum,
											const void		*pVerts,
											const uint16	*pIndis,
											bool			bStrip );

	virtual void		RenderBox( const AABBox& aabbox, const Colour& vColor );
	virtual void		RenderBox( const OBBox& obbox, const Colour& vColor );
	virtual void		RenderBox( const Vector3D& boxmin, const Vector3D& boxmax, const Colour& vColor );
	virtual void		FillBox( const AABBox& aabbox, const Colour& vColor );
	virtual void		FillBox( const OBBox& obbox, const Colour& vColor );
	virtual void		FillBox( const Vector3D& boxmin, const Vector3D& boxmax, const Colour& vColor );
	virtual void		RenderCircle( const Vector3D& center, float radius, int axis, const Colour& vColor );
	virtual void		RenderCircle( const Vector3D& center, float radius, const Vector3D& axis, const Colour& vColor );
	virtual void		RenderEllipse( const Vector3D& center, const Vector3D& radius, int axis, const Colour& vColor );
	virtual void		RenderEllipse( const Vector3D& center, const Vector3D& xaxis, float rx, const Vector3D& yaxis, float ry, const Colour& vColor );

	virtual void		RenderSphere( const Vector3D& center, float radius, const Colour& vColor );
	virtual void		RenderSphere( const Matrix4x4& matrix, float radius, const Colour& vColor );
	virtual void		FillSphere( const Vector3D& center, float radius, const Colour& vColor );
	virtual void		RenderDetailSphere( const Vector3D& center, float radius, int rings, int segments, const Colour& vColor );

	virtual void		RenderCylinder( const Vector3D& base, float radius, float height, int segments, const Colour& vColor );
	virtual void		RenderCylinder( const Matrix4x4& matrix, float radius, float height, int segments, const Colour& vColor );
	virtual void		RenderCone( const Vector3D& base, float radius, float height, int segments, const Colour& vColor );
	virtual void		RenderCone( const Matrix4x4& matrix, float radius, float height, int segments, const Colour& vColor );

	virtual void		RenderEllipsoid( const Vector3D& center, const Vector3D& radius, const Colour& vColor );
	virtual void		RenderEllipsoid( const Matrix4x4& matrix, const Vector3D& radius, const Colour& vColor );

	virtual void		RenderCapsule( const Vector3D& p0, const Vector3D& p1, float radius, const Colour& vColor );
	virtual void		RenderFrustum( const Frustum& f, const Colour& vColor );

	virtual void		FillTriangles(	SGP_VERTEX_TYPE	VertexType,
										uint32          nVertexNum,
										uint32			nIndexNum,
										const void		*pVerts,
										const uint16	*pIndis,
										const SGPSkin&  skin );

	virtual void		RenderFullScreenQuad();

	virtual void		RenderFullScreenQuadWithoutMaterial();

public:
	inline COpenGLRenderDevice*	GetDevice() { return m_pRenderDevice; }

private:
	COpenGLRenderDevice*			m_pRenderDevice;

	Array<COpenGLStaticBuffer*>			m_pSB;					// Static Mesh
	Array<COpenGLTextureBufferObject*>	m_pTBO;					// Texture Buffer object
	
	Array<COpenGLDynamicBuffer*>	m_UPOSVCCache;				// untransformed position + vertex color only
	Array<COpenGLDynamicBuffer*>	m_UPOSTEXCache;				// untransformed position + texcoord 
	Array<COpenGLDynamicBuffer*>	m_UPOSTEXVCCache;			// untransformed position + texcooed with color vertex


	COpenGLVertexBufferObject*		m_pFullScreenQuadVAO;		// full screen quad VAO

	static const int INIT_SB_RESOURCENUM = 1024;
	static const int INIT_DB_MAXSIZE = 8196;

	static uint32 StaticBuffer_ID;

	static float m_fSin[CIRCLE_SLIDES_MAX];
	static float m_fCos[CIRCLE_SLIDES_MAX];
};



#endif		// __SGP_OPENGLVERTEXCACHEMANAGER_HEADER__