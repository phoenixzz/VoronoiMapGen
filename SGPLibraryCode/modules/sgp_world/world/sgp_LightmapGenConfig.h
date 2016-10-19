#ifndef __SGP_LIGHTMAPGENCONFIG_HEADER__
#define __SGP_LIGHTMAPGENCONFIG_HEADER__

class CSGPLightMapGenConfig
{
public:
	CSGPLightMapGenConfig()
	{
		m_iLightMap_Sample_Count = 400;
		m_fLightMap_Collision_Offset = 0.05f;
		m_fLightMap_AO_Distance = 5.0f;
	}
	~CSGPLightMapGenConfig()
	{
		clearSingletonInstance();
	}

public:
	uint32		m_iLightMap_Sample_Count;
	float		m_fLightMap_Collision_Offset;
	float		m_fLightMap_AO_Distance;

	sgp_DeclareSingleton_SingleThreaded (CSGPLightMapGenConfig)
};
#endif		// __SGP_LIGHTMAPGENCONFIG_HEADER__