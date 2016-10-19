#ifndef __SGP_EFFECTSYSTEMMANAGER_HEADER__
#define __SGP_EFFECTSYSTEMMANAGER_HEADER__


class ISGPEffectSystemManager
{
public:
	ISGPEffectSystemManager(ISGPRenderDevice *pdevice, Logger* logger);
	virtual ~ISGPEffectSystemManager();

	// Creates one new CEffectInstance
	// MF1ModelFileName - Effect Instance MF1 file name
	// ConfigIndex - Effect Instance MF1 config index (default 0)
	// return the uint32 ID of the instance, one index of m_EffectInstanceArray
	uint32 createEffectInstance(const String& MF1ModelFileName, uint32 ConfigIndex = 0);

	//! Removes one effect instance from the manager and deletes it.
	void clearEffectInstanceByID( uint32 id );

	//! Get one effect instance from the manager by uint32 ID
	// if id is invalid, return NULL
	CEffectInstance* getEffectInstanceByID( uint32 id );

	//! Render all effect instance in the manager
	void renderAllEffectInstance();

	//! Update all effect instance in the manager
	void updateAllEffectInstance( float deltaTimeinSeconds );

	//! Removes all effect instance from the manager and deletes them.
	void clearAllEffectInstance();


	//! Setting Working Direction
	inline void setWorkingDirection(const String& workingDir)
	{ 
		m_WorkingDir = workingDir;
	}

	inline String& getWorkingDirection() 
	{
		return m_WorkingDir;
	}

private:
	// Init m_EffectInstanceArray Array size 
	// (so that the array won't have to keep dynamically resizing itself as the elements are added, 
	// and it'll therefore be more efficient. )
	static const int				INIT_EFFECTINSTARRAYSIZE = 1024;

	ISGPRenderDevice*				m_pRenderDevice;
	Logger*							m_pLogger;
	String							m_WorkingDir;

	OwnedArray< CEffectInstance >	m_EffectInstanceArray;	// EffectInstance array
};

#endif		// __SGP_EFFECTSYSTEMMANAGER_HEADER__