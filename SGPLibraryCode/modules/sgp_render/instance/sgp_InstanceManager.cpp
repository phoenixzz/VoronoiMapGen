

uint8 ISGPInstanceManager::DefaultSkeletonFPS = 30;
float ISGPInstanceManager::DefaultSecondsPerFrame = 1.0f / ISGPInstanceManager::DefaultSkeletonFPS;
float ISGPInstanceManager::DefaultSecondsActionBlend = 0.3f;


void ISGPInstanceManager::updateAllSkeletonInstance( float deltaTimeinSeconds )
{
	for( int i=0; i<m_SkeletonInstance.size(); i++ )
	{
		m_SkeletonInstance[i]->update(deltaTimeinSeconds);
	}
}

void ISGPInstanceManager::updateAllStaticInstance( float deltaTimeinSeconds )
{
	for( int i=0; i<m_StaticInstance.size(); i++ )
	{
		m_StaticInstance[i]->update(deltaTimeinSeconds);
	}
}