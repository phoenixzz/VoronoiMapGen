#ifndef __SGP_ATMOSPHERICSCATTERING_HEADER__
#define __SGP_ATMOSPHERICSCATTERING_HEADER__

class SGP_API CSGPHoffmanPreethemScatter
{
public:
	CSGPHoffmanPreethemScatter()
	{
        m_fHGgFunction = 0.9f;
        m_fInscatteringMultiplier = 1.79f;
        m_fBetaRayMultiplier = 2.8f;
        m_fBetaMieMultiplier = 0.00003f;
        m_fSunIntensity = 0.1f;
        m_fTurbitity = 0.8f;

        calculateScatteringConstants();
	}

	~CSGPHoffmanPreethemScatter() {}

	inline void calculateScatteringConstants()
	{
        const float n = 1.0003f;
        const float N = 2.545e25f;
        const float pn = 0.035f;

		float fLambda[3] = {0};
		float fLambda2[3] = {0};
		float fLambda4[3] = {0};

        fLambda[0]  = 1.0f / 650e-9f;   // red
        fLambda[1]  = 1.0f / 570e-9f;   // green
        fLambda[2]  = 1.0f / 475e-9f;   // blue

        for( int i = 0; i < 3; ++i )
        {
            fLambda2[i] = fLambda[i] * fLambda[i];
            fLambda4[i] = fLambda2[i] * fLambda2[i];
        }

        Vector3D vLambda2( fLambda2[0], fLambda2[1], fLambda2[2] ); 
        Vector3D vLambda4( fLambda4[0], fLambda4[1], fLambda4[2] ); 

        // Rayleigh scattering constants

        float fTemp = float_Pi * float_Pi * (n * n - 1.0f) * (n * n - 1.0f) * (6.0f + 3.0f * pn) / (6.0f - 7.0f * pn) / N;
        float fBeta = 8.0f * fTemp * float_Pi / 3.0f;
         
        m_BetaRay = vLambda4 * fBeta;

        float fBetaDash = fTemp / 2.0f;

        m_BetaDashRay = vLambda4 * fBetaDash;

        // Mie scattering constants

        float T       = 2.0f;
        float c       = (6.544f * T - 6.51f) * 1e-17f;
        float fTemp2  = 0.434f * c * (2.0f * float_Pi) * (2.0f * float_Pi) * 0.5f;

        m_BetaDashMie =  vLambda2 * fTemp2;

        float K[3]    = {0.685f, 0.679f, 0.670f};
        float fTemp3  = 0.434f * c * float_Pi * (2.0f * float_Pi) * (2.0f * float_Pi);

        Vector3D vBetaMieTemp(K[0] * fLambda2[0], K[1] * fLambda2[1], K[2] * fLambda2[2]);

        m_BetaMie = vBetaMieTemp * fTemp3;
	}

	inline void computeAttenuation(float a_theta) 
    {
        float fBeta = 0.04608365822050f * m_fTurbitity - 0.04586025928522f;
        float fTauR, fTauA;
        float fTau[3];
        float m = 1.0f / (std::cos( a_theta ) + 0.15f * std::pow( 93.885f - a_theta / float_Pi * 180.0f, -1.253f ));  // Relative Optical Mass
        float fLambda[3] = {0.65f, 0.57f, 0.475f }; 

        for( int i = 0; i < 3; ++i )
        {
            // Rayleigh Scattering
            // lambda in um.

            fTauR = std::exp( -m * 0.008735f * std::pow( fLambda[i], (-4.08f) ) );

            // Aerosal (water + dust) attenuation
            // beta - amount of aerosols present 
            // alpha - ratio of small to large particle sizes. (0:4,usually 1.3)

            const float fAlpha = 1.3f;
            fTauA = std::exp(-m * fBeta * std::pow(fLambda[i], -fAlpha));  // lambda should be in um


            fTau[i] = fTauR * fTauA; 

        }

        m_SunColorAndIntensity.Set( fTau[0], fTau[1], fTau[2], m_fSunIntensity );
    }



public:
    // Atmosphere data
    float       m_fHGgFunction;                   // g value in Henyey Greenstein approximation function
    float       m_fInscatteringMultiplier;        // Multiply inscattering term with this factor
    float       m_fBetaRayMultiplier;             // Multiply Rayleigh scattering coefficient with this factor
    float       m_fBetaMieMultiplier;             // Multiply Mie scattering coefficient with this factor
    float       m_fSunIntensity;                  // Current sun ambient intensity
    float       m_fTurbitity;                     // Current turbidity

    Vector3D    m_BetaRay;                        // Rayleigh total scattering coefficient
    Vector3D    m_BetaDashRay;                    // Rayleigh angular scattering coefficient without phase term
    Vector3D    m_BetaMie;                        // Mie total scattering coefficient
    Vector3D    m_BetaDashMie;                    // Mie angular scattering coefficient without phase term
    Vector4D    m_SunColorAndIntensity;           // Sun color and intensity packed for the shader

private:
	SGP_DECLARE_NON_COPYABLE (CSGPHoffmanPreethemScatter)
};


#endif		// __SGP_ATMOSPHERICSCATTERING_HEADER__