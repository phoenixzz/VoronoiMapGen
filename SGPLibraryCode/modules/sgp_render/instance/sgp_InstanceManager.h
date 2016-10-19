#ifndef __SGP_INSTANCEMANAGER_HEADER__
#define __SGP_INSTANCEMANAGER_HEADER__

class CSkeletonMeshInstance;
class CStaticMeshInstance;

class SGP_API ISGPInstanceManager
{
public:
	ISGPInstanceManager(ISGPRenderDevice *pdevice) : m_pRenderDevice(pdevice)
	{}
	~ISGPInstanceManager() 
	{}

	void updateAllSkeletonInstance( float deltaTimeinSeconds );
	void updateAllStaticInstance( float deltaTimeinSeconds );

public:
	static uint8 DefaultSkeletonFPS;
	static float DefaultSecondsPerFrame;		// Default Animation delta seconds per frame
	static float DefaultSecondsActionBlend;		// Default Animation blend time when Action Blending


private:
	ISGPRenderDevice*			m_pRenderDevice;

	// Created Skeleton Instance Array
	OwnedArray<CSkeletonMeshInstance> m_SkeletonInstance;
	OwnedArray<CStaticMeshInstance> m_StaticInstance;
};


#endif		// __SGP_INSTANCEMANAGER_HEADER__