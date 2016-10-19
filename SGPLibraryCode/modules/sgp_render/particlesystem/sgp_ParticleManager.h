#ifndef __SGP_PARTICLEMANAGER_HEADER__
#define __SGP_PARTICLEMANAGER_HEADER__

class ISGPParticleSystem;


class SGP_API ISGPParticleManager
{
public:
	ISGPParticleManager(ISGPRenderDevice *pdevice, Logger* logger);
	~ISGPParticleManager();

	// Inits Particle Engine and update mode etc.
	// systemTime : current engine real time
	void initParticleSystemEngine(double systemTime);




	// Creates one new ISGPParticleSystem
	// WorldMatrix : this system world matrix
	// worldTransformed : true to emit particles in world, false to emit them localy
	// return the uint32 ID of the system, one index of m_ParticleSystemArray
	uint32 createParticleSystem(const Matrix4x4& WorldMatrix, bool worldTransformed = true);

	// Recreate one new ISGPParticleSystem at given index
	// WorldMatrix : this system world matrix
	// worldTransformed : true to emit particles in world, false to emit them localy
	// If old ParticleSystem exist at this index, it will be deleted at first
	void recreateParticleSystemByID( uint32 id, const Matrix4x4& WorldMatrix, bool worldTransformed = true );
	
	//! Removes one particle system from the Particle manager and deletes it.
	void clearParticleSystemByID( uint32 id );

	//! Get one particle system from the Particle manager by uint32 ID
	// if id is invalid, return NULL
	ISGPParticleSystem* getParticleSystemByID( uint32 id );

	//! Render all particle system in the Particle manager
	void renderAllParticleSystem();

	//! Update all particle system in the Particle manager
	void updateAllParticleSystem( float deltaTimeinSeconds );

	//! Removes all particle system from the Particle manager and deletes them.
	void clearAllParticleSystem();


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
	// Init m_ParticleSystemArray Array size 
	// (so that the array won't have to keep dynamically resizing itself as the elements are added, 
	// and it'll therefore be more efficient. )
	static const int				INIT_PARTICLEARRAYSIZE = 1024;

	ISGPRenderDevice*				m_pRenderDevice;
	Logger*							m_pLogger;
	String							m_WorkingDir;

	OwnedArray<ISGPParticleSystem>	m_ParticleSystemArray;
};

#endif		// __SGP_PARTICLEMANAGER_HEADER__