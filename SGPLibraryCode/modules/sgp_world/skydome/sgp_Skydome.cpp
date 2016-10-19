
CSGPSkyDome::CSGPSkyDome() : m_SkydomeMF1ModelResourceID(0xFFFFFFFF), m_scale(1.0f), m_bUpdateModel(false)
{
	m_worldMatrix.Identity();
	m_nSkydomeTextureID.ensureStorageAllocated(2);

	m_coludMoveSpeed_x = 0.002f;
	m_coludMoveSpeed_z = 0.002f;
	m_coludNoiseScale = 0.3f;
	m_coludBrightness = 0.4f;

}

void CSGPSkyDome::UpdateWorldMatrix(const Vector4D& campos)
{
	// Update skydome Model Transform Matrix
	m_worldMatrix.Identity();
	m_worldMatrix._11 = m_worldMatrix._22 = m_worldMatrix._33 = GetScale();

	m_worldMatrix.Translate( campos.x, campos.y, campos.z );
}