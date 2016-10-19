#ifndef __SGP_WORLDCONFIG_HEADER__
#define __SGP_WORLDCONFIG_HEADER__

class CSGPWorldConfig
{
public:
	CSGPWorldConfig() 
		: m_bUsingQuadTree(true), m_bVisibleCull(true), m_bUsingTerrainLOD(true),
		  m_bHavingWaterInWorld(false), /*m_bHavingPostProcess(false),*/
		  m_bPostFog(false), m_bDOF(false),
		  m_bShowSkyDome(true), m_bShowWater(true), m_bShowTerrain(true), 		
		  m_bShowLight(true), m_bShowStaticObject(true), m_bShowSkeleton(true), 
		  m_bShowParticle(true), m_bShowGrass(true), m_bShowPostProcess(true), 
		  m_bShowDebugLine(true)
	{
		m_fGrassFarFadingStart = 100.0f;
		m_fGrassFarFadingEnd = 160.0f;
	}

	~CSGPWorldConfig() 
	{
		clearSingletonInstance();
	}



public:
	bool		m_bUsingQuadTree;				// Whether to use the Quad tree
	bool		m_bVisibleCull;					// Whether to use visibility culling
	bool		m_bUsingTerrainLOD;				// Whether to use terrain LOD

	bool		m_bHavingWaterInWorld;			// Whether there is water in this world
	//bool		m_bHavingPostProcess;			// Whether there is PostProcess effect for this world

	bool		m_bPostFog;						// Whether there is PostProcess fog for this world
	bool		m_bDOF;							// Whether there is PostProcess DOF for this world


	float		m_fGrassFarFadingStart;			// how far to fade out the grass
	float		m_fGrassFarFadingEnd;			// the farthest distance the grass will be drawn

	bool		m_bShowSkyDome;					// Whether to render skydome
	bool		m_bShowWater;					// Whether to render water
	bool		m_bShowTerrain;					// Whether to render terrain 
	bool		m_bShowLight;					// Whether to render light
	bool		m_bShowStaticObject;			// Whether to render static mesh objs
	bool		m_bShowSkeleton;				// Whether to render Skin Anim steleton mesh
	bool		m_bShowParticle;				// Whether to render particle
	bool		m_bShowGrass;					// Whether to render grass
	bool		m_bShowPostProcess;				// Whether to render full screen PostProcess
	bool		m_bShowDebugLine;				// Whether to render debug line

	sgp_DeclareSingleton_SingleThreaded (CSGPWorldConfig)
};

#endif		// __SGP_WORLDCONFIG_HEADER__