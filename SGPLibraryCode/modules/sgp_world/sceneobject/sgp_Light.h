#ifndef __SGP_LIGHT_HEADER__
#define __SGP_LIGHT_HEADER__

//! An enum for all types of lights in SGP Engine supports.
enum SGP_LIGHT_TYPE
{
	SGPLT_Point = 0,
	SGPLT_Spot,
	SGPLT_Direction,
};

#pragma pack(push, packing)
#pragma pack(1)

struct ISGPLightObject
{
public:
	ISGPLightObject() : m_iLightID(0), m_fLightSize(0.5f), m_iLightType(SGPLT_Point)
	{
		m_SceneObjectName[0] = 0;

		m_fPosition[0] = m_fPosition[1] = m_fPosition[2] = 0;
		m_fDirection[0] = 0;
		m_fDirection[1] = -1;
		m_fDirection[2] = 0;

		m_fDiffuseColor[0] = m_fDiffuseColor[1] = m_fDiffuseColor[2] = 1;
		m_fSpecularColor[0] = m_fSpecularColor[1] = m_fSpecularColor[2] = 1;
		m_fAmbientColor[0] = m_fAmbientColor[1] = m_fAmbientColor[2] = 0;

		m_fAttenuation0 = 1.0f;
		m_fAttenuation1 = 0.5f;
		m_fAttenuation2 = 0;

		m_fRange = 5.0f;
		m_fFalloff = 1.0f;
		m_fTheta = 30.0f * pi_over_180;
		m_fPhi = 45.0f * pi_over_180;
	}
	inline void Clone(const ISGPLightObject* pLightObjSrc)
	{
		memcpy(this, pLightObjSrc, sizeof(ISGPLightObject));
	}
public:
	char			m_SceneObjectName[128];		// Scene Light Object Name

	uint32			m_iLightID;					// Scene ID for SGPLight

	float			m_fPosition[3];				// light position
	float			m_fDirection[3];			// light direction vector

	float			m_fLightSize;				// light size(seems like a sphere)


	uint32			m_iLightType;				// light type

	float			m_fDiffuseColor[3];			// Light color
	float			m_fSpecularColor[3];
	float			m_fAmbientColor[3];

	float			m_fRange;					// Distance beyond which the light has no effect
	float			m_fFalloff;					// Decrease in illumination between a spotlight's inner cone and the outer edge of the outer cone
	float			m_fAttenuation0;			// how the light intensity changes over distance
	float			m_fAttenuation1;			// Atten = 1/( att0i + att1i * d + att2i * d*d)
	float			m_fAttenuation2;			// Atten = 1, if the light is a directional light. 
												// Atten = 0, the vertex exceeds the light's range
	float			m_fTheta;					// spotlight's inner cone angle, in radians
	float			m_fPhi;						// spotlight's outer cone angle, in radians
};

#pragma pack(pop, packing)
#endif