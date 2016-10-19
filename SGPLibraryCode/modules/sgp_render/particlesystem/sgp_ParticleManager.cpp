

ISGPParticleManager::ISGPParticleManager(ISGPRenderDevice *pdevice, Logger* logger)
	: m_pRenderDevice(pdevice), m_pLogger(logger)
{
	m_ParticleSystemArray.ensureStorageAllocated(INIT_PARTICLEARRAYSIZE);
}

ISGPParticleManager::~ISGPParticleManager()
{
	for( int i=0; i<m_ParticleSystemArray.size(); ++i )
	{
		ISGPParticleSystem* pParticleSystem = m_ParticleSystemArray.getUnchecked(i);
		if( pParticleSystem )
			m_pLogger->writeToLog(String("Memory Leak: Particle System is not deleted! "), ELL_ERROR);
	}

	SPARK::SPKFactory::getInstance().destroyAll();
	SPARK::SPKFactory::destroyInstance();

	m_ParticleSystemArray.clear(true);
}

uint32 ISGPParticleManager::createParticleSystem(const Matrix4x4& WorldMatrix, bool worldTransformed)
{
	int pid = m_ParticleSystemArray.indexOf(NULL);

	if( pid == -1 )
	{
		uint32 ID = m_ParticleSystemArray.size();
		m_ParticleSystemArray.add( ISGPParticleSystem::create(m_pRenderDevice, WorldMatrix, worldTransformed) );
		return ID;
	}

	m_ParticleSystemArray.set( pid, ISGPParticleSystem::create(m_pRenderDevice, WorldMatrix, worldTransformed), false );

	return uint32(pid);
}

void ISGPParticleManager::recreateParticleSystemByID( uint32 id, const Matrix4x4& WorldMatrix, bool worldTransformed )
{
	m_ParticleSystemArray.set( id, ISGPParticleSystem::create(m_pRenderDevice, WorldMatrix, worldTransformed), true );
}

void ISGPParticleManager::clearParticleSystemByID( uint32 id )
{
	m_ParticleSystemArray.set(id, NULL, true);
}

ISGPParticleSystem* ISGPParticleManager::getParticleSystemByID( uint32 id )
{
	if( id != 0xFFFFFFFF )
		return m_ParticleSystemArray[id];
	else
		return NULL;
}


void ISGPParticleManager::clearAllParticleSystem()
{
	SPARK::SPKFactory::getInstance().destroyAll();

	m_ParticleSystemArray.clear(true);
}



void ISGPParticleManager::renderAllParticleSystem()
{
	ISGPParticleSystem** pEnd = m_ParticleSystemArray.end();
	for( ISGPParticleSystem** pbegin = m_ParticleSystemArray.begin(); pbegin < pEnd; pbegin++ )
	{
		if( (*pbegin) )
			(*pbegin)->render();
	}
}


void ISGPParticleManager::updateAllParticleSystem( float deltaTimeinSeconds )
{
	ISGPParticleSystem** pEnd = m_ParticleSystemArray.end();
	for( ISGPParticleSystem** pbegin = m_ParticleSystemArray.begin(); pbegin < pEnd; pbegin++ )
	{
		if( (*pbegin) )
			(*pbegin)->update(deltaTimeinSeconds);
	}
}


void ISGPParticleManager::initParticleSystemEngine(double systemTime)
{
	// Inits Particle Engine
	SPARK::randomSeed = uint32(systemTime);
	// Sets the update step
	SPARK::System::useRealStep();
}
