#ifndef __SGP_OPENGLSKYDOMERENDER_HEADER__
#define __SGP_OPENGLSKYDOMERENDER_HEADER__

class COpenGLSkydomeRenderer
{
public:
	COpenGLSkydomeRenderer(COpenGLRenderDevice *pRenderDevice);
	~COpenGLSkydomeRenderer(); 

	void updateSkydomeMaterialSkin(const CSGPSkyDome* pSkyDome);

	void render(CSGPSkyDome* pSkyDome);

	void update(float fDeltaTimeInSecond, CSGPSkyDome* pSkyDome, const Vector4D& campos);

	void DoDrawSkydomeRenderBatch();
	void DoDrawReflectionSkydomeRenderBatch();

	inline const Vector4D& getSunColorAndIntensity() const { return m_SunColorAndIntensity; }

	inline void resetSkydomeStaticBuffer() { m_SkydomeStaticBuffer = NULL; }

private:
	COpenGLRenderDevice*			m_pRenderDevice;

	COpenGLStaticBuffer*			m_SkydomeStaticBuffer;

	SGPSkin							m_SkydomeMaterialSkin;


	// Skydome Rendering Param
	Matrix4x4						m_SkydomeWorldMatrix;
	Vector4D						m_SkydomeCloudParam;



	// Skydome Atmospheric Scattering
	Vector3D 						m_HGg;					
	Vector3D 						m_BetaRPlusBetaM;		
	Vector4D 						m_Multipliers;			
	Vector3D 						m_BetaDashR;			
	Vector3D 						m_BetaDashM;			
	Vector3D 						m_OneOverBetaRPlusBetaM;
	Vector3D						m_SunDir;
	Vector4D 						m_SunColorAndIntensity;	
};

#endif		// __SGP_OPENGLSKYDOMERENDER_HEADER__