#ifndef __SGP_OPENGLWATERRENDER_HEADER__
#define __SGP_OPENGLWATERRENDER_HEADER__



class COpenGLWaterRenderer
{
public:
	COpenGLWaterRenderer(COpenGLRenderDevice *pRenderDevice);
	~COpenGLWaterRenderer();

	void render(CSGPWater* pWater);

	void update(float fDeltaTimeInSecond, CSGPWater* pWater);

	void DoDrawWaterReflectionMap();
	void DoDrawWaterRefractionMap();
	void DoDrawWaterSimulation(uint32 ReflectionMapID, uint32 RefractionMapID);

	void createWaterWaveTexture(const String& WaveTextureName);
	void releaseWaterWaveTexture();

	inline bool needRenderWater() { return m_VisibleWaterChunks.size() > 0; }

private:
	void createReflectionMatrix( float fWaterHeight );

public:
	Matrix4x4					m_ObliqueNearPlaneReflectionProjMatrix;
	Matrix4x4					m_MirrorViewMatrix;			// water surface mirrored view matrix

private:
	COpenGLRenderDevice*		m_pRenderDevice;

	uint32						m_BumpWaveTextureID;		// water surface bump texture ID

	Array<CSGPTerrainChunk*>	m_VisibleWaterChunks;		// visible water terrain chunks array

	// For Render
	Vector4D					m_vWaveParams;
	Vector2D					m_vWaveDir;
	Vector4D					m_vWaterBumpFresnel;
	Vector4D					m_vWaterColor;		
	Vector2D					m_vWaterDepthBlend;	

};

#endif		// __SGP_OPENGLWATERRENDER_HEADER__