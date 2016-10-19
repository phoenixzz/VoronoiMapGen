#ifndef __SGP_SKELETONMESHINSTANCE_HEADER__
#define __SGP_SKELETONMESHINSTANCE_HEADER__

class SGP_API CSkeletonMeshInstance
{
public:
	CSkeletonMeshInstance(ISGPRenderDevice *pdevice);
	~CSkeletonMeshInstance();

	// Create and Destroy
	void		changeModel( const String& MF1ModelFileName, uint32 ConfigIndex = 0 );
	void		destroyModel( void );


	// Update and Render
	bool		update( float deltaTimeinSeconds );
	void		render();


	// Setting Interface
	void		setPosition(float x, float y, float z) { m_vPosition.Set(x, y, z); }
	Vector3D	getPosition() { return m_vPosition; };
	void		setRotationXYZ(float x, float y, float z) { m_vRotationXYZ.Set(x, y, z); }
	void		setRotationX(float x) { m_vRotationXYZ.x = x; }
	void		setRotationY(float y) { m_vRotationXYZ.y = y; }
	void		setRotationZ(float z) { m_vRotationXYZ.z = z; }
	Vector3D	getRotationXYZ() { return m_vRotationXYZ; }	
	void		setScale(float scale) { m_fScale = scale; }
	float		getScale() { return m_fScale; }


	bool		getVisible() { return m_bVisible; }
	void		setVisible(bool bvisible) { m_bVisible = bvisible; }

	float		getInstanceAlpha() { return m_fInstanceRenderAlpha; }
	void		setInstanceAlpha(float alpha) { m_fInstanceRenderAlpha = alpha; }
	float		getInstanceScale() { return m_fInstanceRenderScale; }
	void		setInstanceScale(float scale) { setScale(scale); m_fInstanceRenderScale = scale; }


	// MF1 Info Interface
	// Find Bone Array index from name, if not found, return -1 
	int32		getBoneIDByName( const char* BoneName );
	// Get Bone position in final world space, according to Bone Array index
	Vector3D	getBoneWorldPositionByID( int32 iBoneID );
	// Get Bone number in MF1 file
	uint32		getBoneNum();
	

	OBBox			getStaticMeshOBBox();
	Matrix4x4&		getModelMatrix() { return m_matModel; }
	float*			getBonesMatrix() { return m_BoneMatrixBuffer; }
	const OBBox&	getInstanceOBBox() const { return m_InstanceOBBox; }
	uint32			getMF1ModelResourceID() { return m_MF1ModelResourceID; }

	// ConfigSetting Interface
	uint32		getConfigSettingIndex() { return m_MF1ConfigIndex; }
	// if pConfigSetting is NULL, unregister replaced textures
	void		setConfigSetting(SGPMF1ConfigSetting* pConfigSetting);

	// Init model instance data
	void		resetModel();

	// Animate
	void		playAnim(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, bool bNewAnim = true);
	void		playAnimCrossFade(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, float fBlendAbsoluteTime = ISGPInstanceManager::DefaultSecondsActionBlend);
	void		playUpperBodyAnim(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, bool bNewAnim = true);
	void		playUpperBodyAnimCrossFade(float fSpeed, uint32 StartFrameID, uint32 EndFrameID, bool bLoop, float fBlendAbsoluteTime = ISGPInstanceManager::DefaultSecondsActionBlend);

	float		getAnimPlayedTime()				{ return m_fAnimPlayedTime; }
	float		getUpperBodyAnimPlayedTime()	{ return m_fUpperAnimPlayedTime; }
	void		getActionFrameIDFromName( const char* ActionName, uint32& startFrameID, uint32& endFrameID );
	
	void		setEnableUpperBodyAnim(bool bEnable) { m_bEnableUpperBodyAnim = bEnable; }
	//void		setUpperBodyRotationY(float y) { m_matUpperBodyRotYFromLowerBody.RotationY(y-m_vRotationXYZ.y); }


	// Attachment interface
	void			addAttachment(SGP_ATTACHMENT_DEFINE part, const String& MF1ModelFileName, uint32 ConfigIndex = 0);
	CMeshComponent* getAttachment(SGP_ATTACHMENT_DEFINE part);
	void			detachAttachment(SGP_ATTACHMENT_DEFINE part);
	void			playAttachmentAnim(float fSpeed, bool bLoop);


	// Particle system interface
	void		resetParticleSystemByID(uint32 partID, const SGPMF1ParticleTag& newSetting, bool* pGroupVisibleArray = NULL);

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

	Matrix4x4   getBBRDMeshMatrix(int meshIndex);

private:
	ISGPRenderDevice*	m_pRenderDevice;

	// Translation
	Vector3D			m_vPosition;
	Vector3D			m_vRotationXYZ;
	float				m_fScale;
	Matrix4x4			m_matModel;

	// Animation (whole body animation)
	float				m_fLastTime;
	float				m_nStartFrameTime;
	float				m_nEndFrameTime;
	float				m_fAnimPlayedTime;
	float				m_fPlaySpeed;
	bool				m_bLoopPlaying;

	// Upperbody Animation
	float				m_fUpperLastTime;
	float				m_nUpperStartFrameTime;
	float				m_nUpperEndFrameTime;
	float				m_fUpperAnimPlayedTime;
	float				m_fUpperPlaySpeed;
	bool				m_bUpperLoopPlaying;

	bool				m_bPlayingUpperBodyAnim;		// Instance currently playing with UpperBodyAnim?
	bool				m_bEnableUpperBodyAnim;			// UpperBodyAnim control flag by user

	// Upper Body Action respect to the lower body Action rotation matrix
	//Matrix4x4			m_matUpperBodyRotYFromLowerBody;

	// when one Anim action blending with following Anim action, backup Last frame Bone Anim Matrix
	Matrix4x4*			m_pBlendFrameMatrix;
	float				m_fActionBlendAbsoluteTime;
	float				m_fActionBlendPassageTime;
	Matrix4x4*			m_pUpperBodyBlendFrameMatrix;
	float				m_fUpperBodyActionBlendAbsoluteTime;
	float				m_fUpperBodyActionBlendPassageTime;

	// updated Bone Matrix Data
	// These data are transformed Bone Matrix, Multiplied by Frame0Inv matrix
	float*				m_BoneMatrixBuffer;			

	// Render
	uint32				m_RenderFlagEx;				// render flag
	float				m_fInstanceRenderAlpha;		// Instance Alpha
	float				m_fOldInstanceRenderAlpha;	// backup Instance Alpha
	float				m_fInstanceRenderScale;		// Instance scale
	float				m_fOldInstanceRenderScale;	// backup Instance scale
	bool				m_bVisible;					// is this instance visible

	RenderBatchConfig	m_InstanceBatchConfig;		// used for Instance render batch setting

	// Resource
	uint32				m_MF1ModelResourceID;		// MF1 File Resource ID in SGPModelManager Models Array
	uint32				m_TBOID;					// index of TBO Array in VertexCacheManager

	// Particle scale and alpha
	float				m_fEffectChangedScale;

	// Config Setting
	uint32				m_MF1ConfigIndex;			// Config Index in MF1 (Default is 0)
	SGPMF1ConfigSetting* m_pCurrentConfig;			// A pointer to outer ConfigSetting Data, so it will NOT be deleted when destory

	// Bounding-Box
	OBBox				m_InstanceOBBox;

	// Model file name
	String				m_ModelFileName;

	// Attachment Data
	CMeshComponent*		m_pAttachedComponents[SGPATTDEF_MAXATTACHMENT];


	SGP_DECLARE_NON_COPYABLE (CSkeletonMeshInstance)
};

#endif		// __SGP_SKELETONMESHINSTANCE_HEADER__