#ifndef __SGP_OPENGLCONFIG_HEADER__
#define __SGP_OPENGLCONFIG_HEADER__


class COpenGLConfig
{
public:
	COpenGLConfig() : MaxTextureUnits(1),
		MaxAnisotropy(1), MaxUserClipPlanes(0), 
		MaxColorAttachementPoints(1),
		MaxAuxBuffers(0), MaxMultipleRenderTargets(1), FullScreenAntiAlias(0),
		MaxTextureSize(1), MaxTextureLODBias(0.0f), 
		OpenGLVersion(0), GLSLVersion(0), HandleSRGB(false),
		Force_Disable_MIPMAPPING(false),
		m_DefaultAmbientColor(0,0,0,1.0f)
	{		
		for( int i=0; i<SGPVDF_COUNT; i++ )
			m_bFeatureEnabled[i] = true;		
	}
	~COpenGLConfig() 
	{
		clearSingletonInstance();
	}

	bool		m_bFeatureEnabled[SGPVDF_COUNT];
	Colour		m_DefaultAmbientColor;

	
	uint8		MaxTextureUnits;
	uint8		MaxAnisotropy;				// Max Texture Anisotropy
	uint8		MaxUserClipPlanes;
	uint8		MaxAuxBuffers;
	uint8		MaxMultipleRenderTargets;
	uint8		MaxColorAttachementPoints;	// Max Color Attachement Points for FBO
	uint32		FullScreenAntiAlias;

	uint32		MaxTextureSize;
	float		MaxTextureLODBias;

	uint16		OpenGLVersion;
	uint16		GLSLVersion;

	bool		HandleSRGB;
	bool		Force_Disable_MIPMAPPING;		// Force NO MIPMAPPING for all texture

	sgp_DeclareSingleton_SingleThreaded (COpenGLConfig)
};


#endif		// __SGP_OPENGLCONFIG_HEADER__