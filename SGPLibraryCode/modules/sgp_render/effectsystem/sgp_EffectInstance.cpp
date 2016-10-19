

CEffectInstance::CEffectInstance(ISGPRenderDevice *pdevice) : m_pRenderDevice(pdevice),
	m_pSkeletonMeshInstance(NULL), m_pStaticMeshInstance(NULL),
	m_fEffectPlayingSpeed(0), m_fOldEffectPlayingSpeed(0),
	m_fEffectPlayingLoop(true),
	m_fEffectAlpha(1.0f), m_fEffectScale(1.0f),
	m_bVisible(true)
{
}

CEffectInstance::~CEffectInstance()
{
	if( m_pSkeletonMeshInstance )
	{
		m_pSkeletonMeshInstance->destroyModel();
		delete m_pSkeletonMeshInstance;
		m_pSkeletonMeshInstance = NULL;
	}
	if( m_pStaticMeshInstance )
	{
		m_pStaticMeshInstance->destroyModel();
		delete m_pStaticMeshInstance;
		m_pStaticMeshInstance = NULL;
	}
}


void CEffectInstance::changeEffect( const String& MF1ModelFileName, uint32 ConfigIndex )
{
	if( m_pSkeletonMeshInstance )
	{
		m_pSkeletonMeshInstance->destroyModel();
		delete m_pSkeletonMeshInstance;
		m_pSkeletonMeshInstance = NULL;
	}
	if( m_pStaticMeshInstance )
	{
		m_pStaticMeshInstance->destroyModel();
		delete m_pStaticMeshInstance;
		m_pStaticMeshInstance = NULL;
	}

	String BoneAnimFileName = MF1ModelFileName.dropLastCharacters(3) + String( "bf1" );
	String AbsolutePath(BoneAnimFileName);
	// Identify by their absolute filenames if possible.
	if( !File::isAbsolutePath(AbsolutePath) )
	{
		AbsolutePath = m_pRenderDevice->GetEffectInstanceManager()->getWorkingDirection() + File::separatorString + String(BoneAnimFileName);
	}

	if( File(AbsolutePath).existsAsFile() )
	{
		// Skeleton Instance
		m_pSkeletonMeshInstance = new CSkeletonMeshInstance(m_pRenderDevice);
		m_pSkeletonMeshInstance->changeModel( MF1ModelFileName, ConfigIndex );
	}
	else
	{
		// Static Instance
		m_pStaticMeshInstance = new CStaticMeshInstance(m_pRenderDevice);
		m_pStaticMeshInstance->changeModel( MF1ModelFileName, ConfigIndex );
	}
}

void CEffectInstance::destroyEffect()
{
	if( m_pSkeletonMeshInstance )
	{
		m_pSkeletonMeshInstance->destroyModel();
		delete m_pSkeletonMeshInstance;
		m_pSkeletonMeshInstance = NULL;
	}
	if( m_pStaticMeshInstance )
	{
		m_pStaticMeshInstance->destroyModel();
		delete m_pStaticMeshInstance;
		m_pStaticMeshInstance = NULL;
	}
}

bool CEffectInstance::update( float deltaTimeinSeconds )
{
	if( !m_bVisible || (m_fEffectAlpha < 0.01f) )
		return false;

	bool bResult = false;

	if( m_pSkeletonMeshInstance )
	{
		m_pSkeletonMeshInstance->setVisible(true);

		m_pSkeletonMeshInstance->setInstanceScale( m_fEffectScale );
		m_pSkeletonMeshInstance->setInstanceAlpha( m_fEffectAlpha );
		m_pSkeletonMeshInstance->setPosition( m_vPosition.x, m_vPosition.y, m_vPosition.z );
		m_pSkeletonMeshInstance->setRotationXYZ( m_vRotationXYZ.x, m_vRotationXYZ.y, m_vRotationXYZ.z );


		if( m_pSkeletonMeshInstance->getMF1ModelResourceID() != 0xFFFFFFFF )
		{
			// Playing speed
			if( m_fOldEffectPlayingSpeed != m_fEffectPlayingSpeed )
			{
				playAnim(m_fEffectPlayingSpeed, m_fEffectPlayingLoop);
				m_fOldEffectPlayingSpeed = m_fEffectPlayingSpeed;
			}
		}

		bResult = m_pSkeletonMeshInstance->update( deltaTimeinSeconds );
	}
	else if( m_pStaticMeshInstance )
	{
		m_pStaticMeshInstance->setVisible(true);

		m_pStaticMeshInstance->setInstanceScale( m_fEffectScale );
		m_pStaticMeshInstance->setInstanceAlpha( m_fEffectAlpha );
		m_pStaticMeshInstance->setPosition( m_vPosition.x, m_vPosition.y, m_vPosition.z );
		m_pStaticMeshInstance->setRotationXYZ( m_vRotationXYZ.x, m_vRotationXYZ.y, m_vRotationXYZ.z );
		
		bResult = m_pStaticMeshInstance->update( deltaTimeinSeconds );
	}

	return bResult;
}

void CEffectInstance::render()
{
	if( m_pSkeletonMeshInstance )
	{
		m_pSkeletonMeshInstance->render();
	}
	else if( m_pStaticMeshInstance )
	{
		m_pStaticMeshInstance->render();
	}
}

void CEffectInstance::playAnim(float fSpeed, bool bLoop)
{
	if( m_pSkeletonMeshInstance )
	{
		m_fEffectPlayingSpeed = fSpeed;
		m_fEffectPlayingLoop = bLoop;
		uint32 startFrameID = 0;
		uint32 endFrameID = 0;
		m_pSkeletonMeshInstance->getActionFrameIDFromName( "all", startFrameID, endFrameID );
		m_pSkeletonMeshInstance->playAnim(fSpeed, startFrameID, endFrameID, bLoop, true);
	}
}

float CEffectInstance::getAnimPlayedTime()
{
	if( m_pSkeletonMeshInstance )
	{
		return m_pSkeletonMeshInstance->getAnimPlayedTime();
	}

	return 0;
}