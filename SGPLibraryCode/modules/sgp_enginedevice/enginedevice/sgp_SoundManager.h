#ifndef __SPG_ENGINESOUNDMANAGER_HEADER__
#define __SPG_ENGINESOUNDMANAGER_HEADER__

class ISGPSoundManager
{
public:
	virtual ~ISGPSoundManager() {}

	// Sound Linstener Interface
	virtual void addListener(const String& name) = 0;
	virtual sgp::CMovementController* getListener() = 0;
	virtual	void setListenerPosition(sgp::Vector4D position) = 0;
	virtual void setListenerOrientation(sgp::Vector4D orientation, sgp::Vector4D upVector) = 0;


	// Scene Sound Source Interface
	virtual void addSceneSoundSource(const sgp::String& name, const sgp::String& soundFileName, float referenceDistance, float maxDistance, bool directionalSound = false, bool autoPlay = true, bool bLoop = false) = 0;
	virtual void* getSceneSoundSourceByID(int32 sourceid) = 0;
	virtual void* getSceneSoundSourceByName(const sgp::String& name) = 0;
	virtual void deleteSceneSoundSourceByName(const String& name) = 0;
	virtual void deleteSceneSoundSourceByID(int32 id) = 0;

		

	/**
	* Sets the global sound volume.
	*/ 
	virtual void setGlobalVolume(float globalVolume) = 0;

	virtual void Update(float fElapsedTime) = 0;
	virtual void UpdateInner() = 0;

	// Called from main thread to sync info
	virtual void syncSoundResource() = 0;
};


#endif		// __SPG_ENGINESOUNDMANAGER_HEADER__