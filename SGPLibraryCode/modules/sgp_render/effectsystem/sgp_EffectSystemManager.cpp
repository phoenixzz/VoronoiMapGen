
ISGPEffectSystemManager::ISGPEffectSystemManager(ISGPRenderDevice *pdevice, Logger* logger)
	: m_pRenderDevice(pdevice), m_pLogger(logger)
{
	m_EffectInstanceArray.ensureStorageAllocated(INIT_EFFECTINSTARRAYSIZE);
}

ISGPEffectSystemManager::~ISGPEffectSystemManager()
{
	for( int i=0; i<m_EffectInstanceArray.size(); ++i )
	{
		CEffectInstance* pEffectInst = m_EffectInstanceArray.getUnchecked(i);
		if( pEffectInst )
			m_pLogger->writeToLog(String("Memory Leak: Effect Instance is not deleted! "), ELL_ERROR);
	}

	m_EffectInstanceArray.clear(true);
}

uint32 ISGPEffectSystemManager::createEffectInstance(const String& MF1ModelFileName, uint32 ConfigIndex)
{
	CEffectInstance* pEffectInst = new CEffectInstance(m_pRenderDevice);
	pEffectInst->changeEffect(MF1ModelFileName, ConfigIndex);


	int pid = m_EffectInstanceArray.indexOf(NULL);

	if( pid == -1 )
	{
		uint32 ID = m_EffectInstanceArray.size();
		m_EffectInstanceArray.add( pEffectInst );
		return ID;
	}

	m_EffectInstanceArray.set( pid, pEffectInst, false );

	return uint32(pid);
}

void ISGPEffectSystemManager::clearEffectInstanceByID( uint32 id )
{
	m_EffectInstanceArray.set(id, NULL, true);
}

CEffectInstance* ISGPEffectSystemManager::getEffectInstanceByID( uint32 id )
{
	if( id != 0xFFFFFFFF )
		return m_EffectInstanceArray[id];
	else
		return NULL;
}

void ISGPEffectSystemManager::renderAllEffectInstance()
{
	CEffectInstance** pEnd = m_EffectInstanceArray.end();
	for( CEffectInstance** pbegin = m_EffectInstanceArray.begin(); pbegin < pEnd; pbegin++ )
	{
		if( (*pbegin) )
			(*pbegin)->render();
	}
}



void ISGPEffectSystemManager::updateAllEffectInstance( float deltaTimeinSeconds )
{
	CEffectInstance** pEnd = m_EffectInstanceArray.end();
	for( CEffectInstance** pbegin = m_EffectInstanceArray.begin(); pbegin < pEnd; pbegin++ )
	{
		if( (*pbegin) )
			(*pbegin)->update(deltaTimeinSeconds);
	}
}

void ISGPEffectSystemManager::clearAllEffectInstance()
{
	m_EffectInstanceArray.clear(true);
}