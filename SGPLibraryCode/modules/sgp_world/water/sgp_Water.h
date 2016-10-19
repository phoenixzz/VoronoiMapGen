#ifndef __SGP_WATER_HEADER__
#define __SGP_WATER_HEADER__

class SGP_API CSGPWater
{
public:
	CSGPWater( float fwaterheight ) : m_fWaterHeight(fwaterheight) 
	{
		m_TerrainWaterChunks.ensureStorageAllocated(SGPTS_LARGE*SGPTS_LARGE);

		m_vWaterSurfaceColor.Set(0.0f, 0.15f, 0.115f);
		m_fRefractionBumpScale = 0.01f;
		m_fReflectionBumpScale = 0.03f;
		m_fFresnelBias = 0.2f;
		m_fFresnelPow = 5.0f;
		m_fWaterDepthScale = 1.5f;
		m_fWaterBlendScale = 1.0f;
		m_vWaveDir.Set(1.0f, 0.0f);
		m_fWaveSpeed = 0.01f;
		m_fWaveRate = 0.02f;
		m_fWaterSunSpecularPower = 64.0f;
	}
	~CSGPWater() {}

public:
	float						m_fWaterHeight;
	Array<CSGPTerrainChunk*>	m_TerrainWaterChunks;


	Vector3D					m_vWaterSurfaceColor;
	float						m_fRefractionBumpScale;
	float						m_fReflectionBumpScale;
	float						m_fFresnelBias;
	float						m_fFresnelPow;
	float						m_fWaterDepthScale;
	float						m_fWaterBlendScale;
	Vector2D					m_vWaveDir;
	float						m_fWaveSpeed;
	float						m_fWaveRate;
	float						m_fWaterSunSpecularPower;

	String						m_WaterWaveTextureName;
private:
	SGP_DECLARE_NON_COPYABLE (CSGPWater)
};


#endif		// __SGP_WATER_HEADER__