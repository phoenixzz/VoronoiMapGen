#ifndef __SGP_MESHCOMPONENT_HEADER__
#define __SGP_MESHCOMPONENT_HEADER__

class CSkeletonMeshInstance;

class SGP_API CMeshComponent
{
public:
	CMeshComponent(ISGPRenderDevice *pdevice, CSkeletonMeshInstance *pParent, SGP_ATTACHMENT_DEFINE part);
	~CMeshComponent();

	// Create and Destroy
	void		changeModel( const String& MF1ModelFileName, uint32 ConfigIndex = 0 );
	void		destroyModel( void );


	// Update and Render
	bool		update( float deltaTimeinSeconds );
	void		render();

	// Animate
	void		playAnim(float fSpeed, bool bLoop);
	float		getAnimPlayedTime() { return m_fAnimPlayedTime; }

	// Init model Component data
	void		resetModel();

	// ConfigSetting Interface
	uint32		getConfigSettingIndex() { return m_MF1ConfigIndex; }
	// if pConfigSetting is NULL, unregister replaced textures
	void		setConfigSetting(SGPMF1ConfigSetting* pConfigSetting);

private:
	void		buildOBB();
	void		updateOBB(const Matrix4x4& Matrix);

	Vector3D	calculateCurrentTranslation(const SGPMF1Bone *pBone, float fTime);
	Quaternion  calculateCurrentRotation(const SGPMF1Bone *pBone, float fTime);
	float		calculateCurrentScale(const SGPMF1Bone *pBone, float fTime);
	bool		calculateCurrentVisible(const SGPMF1Bone *pBone, float fTime);

	bool		isMeshVisible(int meshIndex);
	bool		isParticleVisible(int partIndex);
	bool		isRibbonVisible(int ) { return false; }
	bool		isBoneVisible(uint16 nBoneID);

	Matrix4x4   getSkeletonBBRDMeshMatrix(int meshIndex);
	Matrix4x4   getStaticBBRDMeshMatrix(int meshIndex);
	uint32		getBoneNum();

private:
	ISGPRenderDevice*		m_pRenderDevice;

	CSkeletonMeshInstance*	m_pParentInstance;

	Matrix4x4				m_AttachInitMatrix;
	Matrix4x4				m_ComponentMatrix;			// Component Translate Matrix

	// Attached Parent's BoneID
	int32					m_nAttachedBoneID;			// attached parent Bone ID
	uint32					m_nAttachedPart;			// attached part SGP_ATTACHMENT_DEFINE Enum

	// Animation
	float					m_nStartFrameTime;
	float					m_nEndFrameTime;
	float					m_fAnimPlayedTime;
	float					m_fPlaySpeed;				// Animation playing speed
	bool					m_bLoopPlaying;				// Animation Loop
	bool					m_bStatic;					// no Animation

	// Resource
	uint32					m_MF1ModelResourceID;		// MF1 File Resource ID in SGPModelManager Models Array
	uint32					m_TBOID;					// index of TBO Array in VertexCacheManager
	
	// Render
	RenderBatchConfig		m_InstanceBatchConfig;		// used for Instance render batch setting

	// updated Bone Matrix Data
	// These data are transformed Bone Matrix, Multiplied by Frame0Inv matrix
	float*					m_BoneMatrixBuffer;		

	// Config Setting
	uint32					m_MF1ConfigIndex;			// Config Index in MF1 (Default is 0)
	SGPMF1ConfigSetting*	m_pCurrentConfig;			// A pointer to outer ConfigSetting Data, so it will NOT be deleted when destory

	// Bounding-Box
	OBBox					m_ComponentOBBox;

	// Model file name
	String					m_ModelFileName;


	SGP_DECLARE_NON_COPYABLE (CMeshComponent)
};

#endif		// __SGP_MESHCOMPONENT_HEADER__