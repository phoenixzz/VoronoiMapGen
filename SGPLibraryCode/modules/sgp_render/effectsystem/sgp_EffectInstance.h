#ifndef __SGP_EFFECTINSTANCE_HEADER__
#define __SGP_EFFECTINSTANCE_HEADER__

class SGP_API CEffectInstance
{
public:
	CEffectInstance(ISGPRenderDevice *pdevice);
	~CEffectInstance();

	// Create and Destroy
	void		changeEffect( const String& MF1ModelFileName, uint32 ConfigIndex = 0 );
	void		destroyEffect( void );

	// Update and Render
	bool		update( float deltaTimeinSeconds );
	void		render();

	// Setting Interface
	void		setPosition(float x, float y, float z) { m_vPosition.Set(x, y, z); }
	Vector3D	getPosition() { return m_vPosition; }
	void		setRotationXYZ(float x, float y, float z) { m_vRotationXYZ.Set(x, y, z); }
	void		setRotationX(float x) { m_vRotationXYZ.x = x; }
	void		setRotationY(float y) { m_vRotationXYZ.y = y; }
	void		setRotationZ(float z) { m_vRotationXYZ.z = z; }
	Vector3D	getRotationXYZ() { return m_vRotationXYZ; }

	void		setScale(float scale) { m_fEffectScale = scale; }
	float		getScale() { return m_fEffectScale; }
	void		setAlpha(float alpha) { m_fEffectAlpha = alpha; }
	float		getAlpha() { return m_fEffectAlpha; }


	bool		getVisible() { return m_bVisible; }
	void		setVisible(bool bvisible) { m_bVisible = bvisible; }


	// Animate
	void		playAnim(float fSpeed, bool bLoop);
	float		getAnimPlayingSpeed() { return m_fEffectPlayingSpeed; }
	float		getAnimPlayedTime();


private:
	ISGPRenderDevice*		m_pRenderDevice;

	CSkeletonMeshInstance*	m_pSkeletonMeshInstance;
	CStaticMeshInstance*	m_pStaticMeshInstance;

	// Translation
	Vector3D				m_vPosition;
	Vector3D				m_vRotationXYZ;


	float					m_fEffectAlpha;				// Effect Render alpha
	float					m_fEffectScale;				// Effect instance scale
	float					m_fEffectPlayingSpeed;		// Effect anim playing speed
	bool					m_fEffectPlayingLoop;		// Effect anim playing loop

	bool					m_bVisible;					// is this effect instance visible
	

	float					m_fOldEffectPlayingSpeed;	// backup playing speed

	SGP_DECLARE_NON_COPYABLE (CEffectInstance)
};

#endif		// __SGP_EFFECTINSTANCE_HEADER__