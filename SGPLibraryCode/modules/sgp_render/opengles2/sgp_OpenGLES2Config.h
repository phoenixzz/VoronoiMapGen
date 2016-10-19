#ifndef __SGP_OPENGLES2CONFIG_HEADER__
#define __SGP_OPENGLES2CONFIG_HEADER__

class COpenGLES2Config
{
public:
	COpenGLES2Config() : bIsPVRTCSupported(false), bIsPVRTC2Supported(false),
		bIsFloat16Supported(false), bIsFloat32Supported(false),
		bIsBGRA8888Supported(false),
		MaxTextureUnits(1), MaxTextureSize(1), FullScreenAntiAlias(0), 
		HandleSRGB(false),
		Force_Disable_MIPMAPPING(false)
	{

#ifndef TARGET_OS_IPHONE
	bIsETCSupported = false;
#endif
		DimAliasedLine[0] = 1.0f;
		DimAliasedLine[1] = 1.0f;
	}

	~COpenGLES2Config() 
	{
		clearSingletonInstance();
	}


	void InitConfig()
	{
		GLint Intparams;

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Intparams);
		MaxTextureUnits = jmin(static_cast<uint8>(Intparams), static_cast<uint8>(SGP_MATERIAL_MAX_TEXTURES));

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &Intparams);
		MaxTextureSize = static_cast<uint32>(Intparams);


		// Check supported OPENGL ES 2.0 extension
		bIsPVRTCSupported = CPVRTgles2Ext::IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
		bIsPVRTC2Supported = CPVRTgles2Ext::IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc2");

#ifndef TARGET_OS_IPHONE
		bIsBGRA8888Supported  = CPVRTgles2Ext::IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");
#else
		bIsBGRA8888Supported  = CPVRTgles2Ext::IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
#endif

		bIsFloat16Supported = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_texture_half_float");
		bIsFloat32Supported = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_texture_float");

#ifndef TARGET_OS_IPHONE
		bIsETCSupported = CPVRTgles2Ext::IsGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
#endif
		glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
	}

public:


	bool		bIsPVRTCSupported;
	bool		bIsPVRTC2Supported;

	bool		bIsBGRA8888Supported;

	bool		bIsFloat16Supported; 
	bool		bIsFloat32Supported;

#ifndef TARGET_OS_IPHONE
	bool		bIsETCSupported;
#endif

	Colour		m_DefaultAmbientColor;

	// the maximum supported texture image units that can be used to access texture maps from the vertex shader and the fragment processor combined.
	uint8		MaxTextureUnits;
	// a rough estimate of the largest texture that the GLES2 can handle
	uint32		MaxTextureSize;


	uint32		FullScreenAntiAlias;

	//! Minimal and maximal supported thickness for lines without smoothing
	GLfloat		DimAliasedLine[2];

	bool		HandleSRGB;
	bool		Force_Disable_MIPMAPPING;

	sgp_DeclareSingleton_SingleThreaded (COpenGLES2Config)
};

#endif		// __SGP_OPENGLES2CONFIG_HEADER__