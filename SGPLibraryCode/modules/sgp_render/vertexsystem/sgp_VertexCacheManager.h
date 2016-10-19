#ifndef __SGP_VERTEXCACHEMANAGER_HEADER__
#define __SGP_VERTEXCACHEMANAGER_HEADER__

/**
 * Class to manage static and dynamic vertex bunches, optionally
 * using indices during rendering process.
 */
class ISGPVertexCacheManager
{
public:
    ISGPVertexCacheManager(void) {};
	virtual ~ISGPVertexCacheManager(void) {};

    virtual uint32		CreateStaticBuffer(
											SGP_VERTEX_TYPE VertexType,
											const SGPSkin& skin,
											const AABBox& boundingbox,
											uint32  nVertexNum, 
											uint32  nIndexNum, 
											const void   *pVerts,
											const uint16 *pIndis ) = 0;

	virtual uint32		CreateMF1MeshStaticBuffer(
											const SGPMF1Skin& MeshSkin,
											const SGPMF1Mesh& MF1Mesh,
											const SGPMF1BoneGroup* pBoneGroup,
											uint32 NumBoneGroup ) = 0;

	virtual void*		GetStaticBufferByID(uint32 nSBufferID) = 0;

	virtual uint32		CreateTextureBufferObjectByID(uint32 ModelResourceID) = 0;
	virtual void		UpdateTextureBufferObjectByID(float* pBoneMatrixBuffer, uint32 nBoneCount, uint32 TBOID) = 0;
	virtual bool		BindTextureBufferObjectByID(uint32 TBOID, int iTextureUnit) = 0;

	virtual void		ClearAllTextureBufferObject() = 0;
	virtual void		ClearTextureBufferObject( uint32 nTBOID ) = 0;

	virtual void		ClearAllStaticBuffer() = 0;
	virtual void		ClearStaticBuffer( uint32 nSBufferID ) = 0;

	virtual void		RenderStaticBuffer( uint32 nSBufferID, const Matrix4x4& matWorld, const RenderBatchConfig& config) = 0;	

	virtual void		RenderSkeletonMesh( uint32 nSBufferID, const Matrix4x4& matWorld, uint32 nTBOID, const RenderBatchConfig& config ) = 0;

	virtual void		RenderSkeletonMesh( uint32 nSBufferID, const Matrix4x4& matWorld, float* pBoneMatrixBuffer, uint32 nBoneNum, const RenderBatchConfig& config ) = 0;
      
    virtual void		RenderDynamicBuffer(SGP_VERTEX_TYPE VertexType,
											const SGPSkin& skin,
											uint32 nVertexNum, 
											uint32 nIndexNum, 
											const void   *pVerts,
											const uint16 *pIndis ) = 0;

	virtual void		ForcedCommitAll(void) = 0;
	virtual void		ForcedClearAll(void) = 0;


	// Below rendering functions which used for Debug Line or Debug Info
	//
	//				y^    z
	//				 |   /
	//				 |  /
	//				 | /
	//			     |/----------->x
	//				O

    virtual void		RenderPoints(	uint32							 nVertexNum,
										const SGPVertex_UPOS_VERTEXCOLOR *pVerts ) = 0;

    virtual void		RenderLines(	uint32							 nVertexNum,
										const SGPVertex_UPOS_VERTEXCOLOR *pVerts,
										bool							 bStrip ) = 0;

    virtual void		RenderLine(		const float  *fStart,
										const float  *fEnd, 
										const Colour *pColor ) = 0;

	virtual void		RenderTriangles(	uint32          nVertexNum,
											uint32			nIndexNum,
											const void		*pVerts,
											const uint16	*pIndis,
											bool			bStrip ) = 0;

	virtual void		RenderBox( const AABBox& aabbox, const Colour& vColor ) = 0;
	virtual void		RenderBox( const OBBox& obbox, const Colour& vColor ) = 0;
	virtual void		RenderBox( const Vector3D& boxmin, const Vector3D& boxmax, const Colour& vColor ) = 0;
	virtual void		FillBox( const AABBox& aabbox, const Colour& vColor ) = 0;
	virtual void		FillBox( const OBBox& obbox, const Colour& vColor ) = 0;
	virtual void		FillBox( const Vector3D& boxmin, const Vector3D& boxmax, const Colour& vColor ) = 0;
	virtual void		RenderCircle( const Vector3D& center, float radius, int axis, const Colour& vColor ) = 0;
	virtual void		RenderCircle( const Vector3D& center, float radius, const Vector3D& axis, const Colour& vColor ) = 0;
	virtual void		RenderEllipse( const Vector3D& center, const Vector3D& radius, int axis, const Colour& vColor ) = 0;
	virtual void		RenderEllipse( const Vector3D& center, const Vector3D& xaxis, float rx, const Vector3D& yaxis, float ry, const Colour& vColor ) = 0;
	
	virtual void		RenderSphere( const Vector3D& center, float radius, const Colour& vColor ) = 0;
	virtual void		RenderSphere( const Matrix4x4& matrix, float radius, const Colour& vColor ) = 0;
	virtual void		FillSphere( const Vector3D& center, float radius, const Colour& vColor ) = 0;

	virtual void		RenderDetailSphere( const Vector3D& center, float radius, int rings, int segments, const Colour& vColor ) = 0;

	virtual void		RenderCylinder( const Vector3D& base, float radius, float height, int segments, const Colour& vColor ) = 0;
	virtual void		RenderCylinder( const Matrix4x4& matrix, float radius, float height, int segments, const Colour& vColor ) = 0;

	virtual void		RenderCone( const Vector3D& base, float radius, float height, int segments, const Colour& vColor ) = 0;
	virtual void		RenderCone( const Matrix4x4& matrix, float radius, float height, int segments, const Colour& vColor ) = 0;

	virtual void		RenderEllipsoid( const Vector3D& center, const Vector3D& radius, const Colour& vColor ) = 0;
	virtual void		RenderEllipsoid( const Matrix4x4& matrix, const Vector3D& radius, const Colour& vColor ) = 0;

	virtual void		RenderCapsule( const Vector3D& p0, const Vector3D& p1, float radius, const Colour& vColor ) = 0;
	virtual void		RenderFrustum( const Frustum& f, const Colour& vColor ) = 0;

	virtual void		FillTriangles(	SGP_VERTEX_TYPE	VertexType,
										uint32          nVertexNum,
										uint32			nIndexNum,
										const void		*pVerts,
										const uint16	*pIndis,
										const SGPSkin&	skin ) = 0;


	virtual void		RenderFullScreenQuad() = 0;

	virtual void		RenderFullScreenQuadWithoutMaterial() = 0;

};

#endif		// __SGP_VERTEXCACHEMANAGER_HEADER__