#ifndef __SGP_OPENGLES2WATERRENDER_HEADER__
#define __SGP_OPENGLES2WATERRENDER_HEADER__



class COpenGLES2WaterRenderer
{
public:
	COpenGLES2WaterRenderer(COpenGLES2RenderDevice *pRenderDevice);
	~COpenGLES2WaterRenderer();

	void render(CSGPWater* pWater);

	void update(float fDeltaTimeInSecond, CSGPWater* pWater);

	// Frame Buffer Interface
	void createWaterReflectionFBO(uint32 Width, uint32 Height);
	void createWaterRefractionFBO(uint32 Width, uint32 Height);
	void createSceneBufferFBO(uint32 Width, uint32 Height);
	void releaseWaterFBO();

	// Switch Render Target Interface
	bool discardFramebuffer(bool bDiscardDepth, bool bDiscardStencil);

	void switchToReflectionRenderTarget(bool bClearColor=true, bool bClearDepth=true, bool bClearStencil=true);
	void switchToRefractionRenderTarget(bool bClearColor=true, bool bClearDepth=true, bool bClearStencil=true);
	void switchToSceneBufferRenderTarget(bool bClearColor=true, bool bClearDepth=true, bool bClearStencil=true);
	void switchSceneBufferToBackBuffer(bool bClearColor=true, bool bClearDepth=true, bool bClearStencil=true);

	// Render Interface
	void renderSceneBufferFullScreenQuad();


	void DoDrawWaterReflectionMap();
	void DoDrawWaterRefractionMap();
	void DoDrawWaterSimulation(uint32 ReflectionMapID, uint32 RefractionMapID);

	void createWaterWaveTexture(const String& WaveTextureName);
	void releaseWaterWaveTexture();

	inline bool needRenderWater() { return m_VisibleWaterChunks.size() > 0; }

	inline uint32 getReflectionMapID() { return m_pReflectionFBO ? m_pReflectionFBO->getFBORenderToTextureID() : 0; }
	inline uint32 getRefractionMapID() { return m_pRefractionFBO ? m_pRefractionFBO->getFBORenderToTextureID() : 0; }
	inline uint32 getSceneBufferMapID(){ return m_pSceneBufferFBO ? m_pSceneBufferFBO->getFBORenderToTextureID() : 0; }

private:
	void createReflectionMatrix( float fWaterHeight );

public:
	Matrix4x4						m_ObliqueNearPlaneReflectionProjMatrix;
	Matrix4x4						m_MirrorViewMatrix;			// water surface mirrored view matrix

private:
	COpenGLES2RenderDevice*			m_pRenderDevice;
	COpenGLES2FrameBufferObject*	m_pReflectionFBO;			// water Reflection Render Buffer Target
	COpenGLES2FrameBufferObject*	m_pRefractionFBO;			// water Refraction Render Buffer Target
	COpenGLES2FrameBufferObject*	m_pSceneBufferFBO;			// main scene Render Buffer Target

	uint32							m_BumpWaveTextureID;		// water surface bump texture ID

	Array<CSGPTerrainChunk*>		m_VisibleWaterChunks;		// visible water terrain chunks array

	// For Render
	Vector4D						m_vWaveParams;
	Vector2D						m_vWaveDir;
	Vector4D						m_vWaterBumpFresnel;
	Vector4D						m_vWaterColor;		
	Vector2D						m_vWaterDepthBlend;	

};

#endif		// __SGP_OPENGLES2WATERRENDER_HEADER__