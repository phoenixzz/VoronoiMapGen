CMovementController::CMovementController(void)
{
	Init();
}

CMovementController::~CMovementController(void)
{
}

void CMovementController::Init(void)
{
	m_vcPos.Set(0.0f, 0.0f, 0.0f);
	m_vcRight.Set(1.0f, 0.0f, 0.0f);
	m_vcUp.Set(0.0f, 1.0f, 0.0f);
	m_vcDir.Set(0.0f, 0.0f, 1.0f);
	m_vcVelocity.Set(0.0f, 0.0f, 0.0f);
	m_fRotX = m_fRotY = m_fRotZ = 0.0f;
	m_fThrust = 0.0f;
	m_fRollSpeed = m_fPitchSpeed = m_fYawSpeed = 0.0f;
	m_Quaternion.x = m_Quaternion.y = m_Quaternion.z = 0.0f;
	m_Quaternion.w = 1.0f;
}

/**
 * Recalculate the orientation of the local axes according to the
 * current rotation values around the axes.
 * according to m_fRotX, m_fRotY, m_fRotZ
 */
void CMovementController::RecalcAxes(void)
{
	Quaternion		qFrame;
	Matrix4x4		mat;
	Quaternion		quaternion(m_Quaternion.x, m_Quaternion.y, m_Quaternion.z, m_Quaternion.w);

	// keep in range of 360 degree
	if(m_fRotX > float_2pi)
		m_fRotX -= float_2pi;
	else if(m_fRotX < -float_2pi)
		m_fRotX += float_2pi;
   
	if(m_fRotY > float_2pi)
		m_fRotY -= float_2pi;
	else if(m_fRotY < -float_2pi)
		m_fRotY += float_2pi;
   
	if(m_fRotZ > float_2pi)
		m_fRotZ -= float_2pi;
	else if(m_fRotZ < -float_2pi)
		m_fRotZ += float_2pi;

	// build new quaternion for this rotation
	qFrame.MakeFromEuler(m_fRotX, m_fRotY, m_fRotZ);

	// add to existing rotation
	quaternion *= qFrame;

	// extract the local axes
	quaternion.GetMatrix(&mat);

	m_vcRight.x = mat._11;
	m_vcRight.y = mat._21;
	m_vcRight.z = mat._31;

	m_vcUp.x    = mat._12;
	m_vcUp.y    = mat._22;
	m_vcUp.z    = mat._32;

	m_vcDir.x   = mat._13;
	m_vcDir.y   = mat._23;
	m_vcDir.z   = mat._33;
}