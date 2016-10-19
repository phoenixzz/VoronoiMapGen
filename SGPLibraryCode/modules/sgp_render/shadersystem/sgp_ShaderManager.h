#ifndef __SGP_SHADERMANAGER_HEADER__
#define __SGP_SHADERMANAGER_HEADER__

class ISGPShaderManager
{
public:
	virtual ~ISGPShaderManager() {}

	virtual void onDeviceLost(void) = 0;
	virtual void onDeviceReset(void) = 0;

	virtual void preCacheShaders(void) = 0;

	virtual void loadAllShaders(void) = 0;
	virtual void unloadAllShaders(void) = 0;
};


#endif		// __SGP_SHADERMANAGER_HEADER__