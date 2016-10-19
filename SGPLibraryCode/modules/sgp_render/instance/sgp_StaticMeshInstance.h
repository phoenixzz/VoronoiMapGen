#ifndef __SGP_STATICMESHINSTANCE_HEADER__
#define __SGP_STATICMESHINSTANCE_HEADER__


class SGP_API CStaticMeshInstance
{
public:
	CStaticMeshInstance(ISGPRenderDevice *pdevice);
	~CStaticMeshInstance();

	// Create and Destroy
	void		changeModel( const String& MF1ModelFileName, uint32 ConfigIndex = 0 );
	void		destroyModel( void );

	// Update and Render
	bool		update( float deltaTimeinSeconds );
	void		render();

	// Setting Interface
	void		setPosition(float x, float y, float z) { m_vPosition.Set(x, y, z); m_bNeedUpdate = true; }
	Vector3D	getPosition() { return m_vPosition; };
	void		setRotationXYZ(float x, float y, float z) { m_vRotationXYZ.Set(x, y, z); m_bNeedUpdate = true; }
	void		setRotationX(float x) { m_vRotationXYZ.x = x; m_bNeedUpdate = true; }
	void		setRotationY(float y) { m_vRotationXYZ.y = y; m_bNeedUpdate = true; }
	void		setRotationZ(float z) { m_vRotationXYZ.z = z; m_bNeedUpdate = true; }
	Vector3D	getRotationXYZ() { return m_vRotationXYZ; }
	void		setScale(float scale) { m_fScale = scale; m_bNeedUpdate = true; }
	float		getScale() { return m_fScale; }

	bool		getVisible() { return m_bVisible; }
	void		setVisible(bool bvisible) { m_bVisible = bvisible; }

	float		getInstanceAlpha() { return m_fInstanceRenderAlpha; }
	void		setInstanceAlpha(float alpha) { m_fInstanceRenderAlpha = alpha; }
	float		getInstanceScale() { return m_fInstanceRenderScale; }
	void		setInstanceScale(float scale) { setScale(scale); m_fInstanceRenderScale = scale; }


	OBBox			getStaticMeshOBBox();
	Matrix4x4&		getModelMatrix() { return m_matModel; }
	const OBBox&	getInstanceOBBox() { return m_InstanceOBBox; }
	uint32			getMF1ModelResourceID() { return m_MF1ModelResourceID; }
	const uint32	getMeshTriangleCount();
	const uint32	getMeshVertexCount();

	// ConfigSetting Interface
	uint32		getConfigSettingIndex() { return m_MF1ConfigIndex; }
	// if pConfigSetting is NULL, unregister replaced textures
	void		setConfigSetting(SGPMF1ConfigSetting* pConfigSetting);

	// Init model instance data
	void		resetModel();

	// Particle system interface
	void		resetParticleSystemByID(uint32 partID, const SGPMF1ParticleTag& newSetting, bool* pGroupVisibleArray = NULL);

	// Lightmap interface
	void		registerLightmapTexture(const String& WorldMapName, const String& LightmapTexName);
	void		unregisterLightmapTexture();
	void		updateLightmapTexture(uint32 TexWidth, uint32 TexHeight, uint32 *pLightMapData);


private:
	void		buildOBB();
	void		updateOBB(const Matrix4x4& Matrix);

	bool		isMeshVisible(int meshIndex);
	bool		isParticleVisible(int partIndex);
	bool		isRibbonVisible(int ) { return false; }

	Matrix4x4   getBBRDMeshMatrix(int meshIndex);

private:
	ISGPRenderDevice*	m_pRenderDevice;

	// Translation
	Vector3D			m_vPosition;
	Vector3D			m_vRotationXYZ;
	float				m_fScale;
	Matrix4x4			m_matModel;

	bool				m_bNeedUpdate;				// is this instance need update
	bool				m_bVisible;					// is this instance visible

	// Render
	uint32				m_RenderFlagEx;				// render flag
	float				m_fInstanceRenderAlpha;		// Instance Alpha
	float				m_fOldInstanceRenderAlpha;	// backup Instance Alpha
	float				m_fInstanceRenderScale;		// Instance scale
	float				m_fOldInstanceRenderScale;	// backup Instance scale

	RenderBatchConfig	m_InstanceBatchConfig;		// used for Instance render batch setting

	// Resource
	uint32				m_MF1ModelResourceID;		// MF1 File Resource ID in SGPModelManager Models Array
	uint32				m_LightMapTextureID;		// lightmap texture ID for this static instance

	// Config Setting
	uint32				m_MF1ConfigIndex;			// Config Index in MF1 (Default is 0)
	SGPMF1ConfigSetting* m_pCurrentConfig;			// A pointer to outer ConfigSetting Data, so it will NOT be deleted when destory

	// Bounding-Box
	OBBox				m_InstanceOBBox;

	// Model file name
	String				m_ModelFileName;


	SGP_DECLARE_NON_COPYABLE (CStaticMeshInstance)

};

#endif		// __SGP_STATICMESHINSTANCE_HEADER__