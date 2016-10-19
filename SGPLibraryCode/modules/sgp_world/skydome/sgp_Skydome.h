#ifndef __SGP_SKYDOME_HEADER__
#define __SGP_SKYDOME_HEADER__

/*	CSGPSkyDome
-----------------------------------------------------------------
    
    A Sky Dome is a simple dome oriented around the origin. It is
	intended to be used with a dome texture to render
	distant scenery around the camera
    
-----------------------------------------------------------------
*/
class SGP_API CSGPSkyDome
{
public:
	CSGPSkyDome();
	~CSGPSkyDome() {}
	

	inline float GetScale() const { return m_scale; }
	inline void SetScale(float fScale) { m_scale = fScale; }

	inline CSGPHoffmanPreethemScatter& GetScatter() { return m_Scatter; }

	void UpdateWorldMatrix(const Vector4D& campos);
	const Matrix4x4& GetWorldMatrix() const { return m_worldMatrix; }

public:
	uint32 m_SkydomeMF1ModelResourceID;		// MF1 File Resource ID in SGPModelManager Models Array
	Array<uint32> m_nSkydomeTextureID;		// Skydome Textures ID Array


	String m_MF1FileName;					// skydome MF1 file name
	String m_cloudTextureFileName;			// cloud texture file name
	String m_noiseTextureFileName;			// noise texture file name


	float  m_coludMoveSpeed_x;				// cloud layer move speed x direction
	float  m_coludMoveSpeed_z;				// cloud layer move speed z direction
	float  m_coludNoiseScale;				// cloud layer texture Noise Scale
	float  m_coludBrightness;				// cloud layer texture brightness

	bool m_bUpdateModel;					// skydome model need be updated?


private:
	Matrix4x4 m_worldMatrix;				// skydome world matrix

	float m_scale;							// skydome model scale

	CSGPHoffmanPreethemScatter m_Scatter;	// Atmospheric Scattering

	SGP_DECLARE_NON_COPYABLE (CSGPSkyDome)
};

#endif		// __SGP_SKYDOME_HEADER__