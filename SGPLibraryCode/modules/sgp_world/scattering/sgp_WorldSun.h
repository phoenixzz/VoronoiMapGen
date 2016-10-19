#ifndef __SGP_WORLDSUN_HEADER__
#define __SGP_WORLDSUN_HEADER__


class SGP_API CSGPWorldSun
{
public:
	CSGPWorldSun() : m_fSunPosition(0.0f) 
	{
		updateSunDirection();
	}

	~CSGPWorldSun() {}

	inline const Vector3D& getSunDirection() const { return m_vSunDirection; }
	inline const Vector3D& getNormalizedSunDirection() const { return m_vNormalizedSunDir; }
	inline void updateSunDirection()
	{
		// the sun Direction is vector from 0,0,0 to sun position
		m_vSunDirection.Set( std::cos(90.0f * pi_over_180),
                             std::cos(m_fSunPosition * pi_over_180),
                             std::sin(m_fSunPosition * pi_over_180) );
		m_vNormalizedSunDir = m_vSunDirection;
		m_vNormalizedSunDir.Normalize();
	}

public:
	float		m_fSunPosition;
	Vector3D	m_vSunDirection;
	Vector3D	m_vNormalizedSunDir;
};


#endif			// __SGP_WORLDSUN_HEADER__