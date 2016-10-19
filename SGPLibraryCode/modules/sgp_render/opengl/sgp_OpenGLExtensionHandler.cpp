

COpenGLExtensionHandler::COpenGLExtensionHandler() :
		StencilBuffer(false), MultiTextureExtension(false),
		TextureCompressionExtension(false),
		OcclusionQuerySupport(false),
		MaxSupportedTextures(1), MaxTextureUnits(1), MaxLights(1),
		MaxAnisotropy(1), MaxUserClipPlanes(0), MaxAuxBuffers(0),
		MaxColorAttachementPoints(1),
		MaxMultipleRenderTargets(1), MaxIndices(65535),
		MaxTextureSize(1), MaxGeometryVerticesOut(0),
		MaxTextureLODBias(0.0f), Version(0), ShaderLanguageVersion(0)
{
	DimAliasedLine[0] = 1.0f;
	DimAliasedLine[1] = 1.0f;
	DimAliasedPoint[0] = 1.0f;
	DimAliasedPoint[1] = 1.0f;
	DimSmoothedLine[0] = 1.0f;
	DimSmoothedLine[1] = 1.0f;
	DimSmoothedPoint[0] = 1.0f;
	DimSmoothedPoint[1] = 1.0f;
}

void COpenGLExtensionHandler::initExtensions(bool stencilBuffer)
{
	Version = _GLEE_VERSION_4_1 ? 410 : _GLEE_VERSION_4_0 ? 400 : _GLEE_VERSION_3_3 ? 330 :
		_GLEE_VERSION_3_2 ? 320 : _GLEE_VERSION_3_1 ? 310 : _GLEE_VERSION_3_0 ? 300 :
		_GLEE_VERSION_2_1 ? 210 : _GLEE_VERSION_2_0 ? 200 : _GLEE_VERSION_1_5 ? 150 :
		_GLEE_VERSION_1_4 ? 140 : _GLEE_VERSION_1_3 ? 130 : _GLEE_VERSION_1_2 ? 120 : 0;

	//if ( Version >= 200)
	//	os::Printer::log("OpenGL driver version is 2.0 or better.", ELL_INFORMATION);
	//else
	//	os::Printer::log("OpenGL driver version is not 2.0 or better.", ELL_WARNING);


	
	MultiTextureExtension = GLEE_ARB_multitexture ? true : false;
	TextureCompressionExtension = GLEE_ARB_texture_compression ? true : false;
	StencilBuffer = stencilBuffer;


	GLint num = 0;
	// set some properties
#if defined(GL_ARB_multitexture) || defined(GL_VERSION_1_3)
	if( Version>120 || GLEE_ARB_multitexture )
	{
#if defined(GL_MAX_TEXTURE_IMAGE_UNITS)
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
#elif defined(GL_MAX_TEXTURE_UNITS_ARB)
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &num);
#endif
		MaxSupportedTextures = static_cast<uint8>(num);
	}
#endif
#if defined(GL_ARB_vertex_shader) || defined(GL_VERSION_2_0)
	if( Version>=200 || GLEE_ARB_vertex_shader )
	{
		num = 0;
#if defined(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &num);
#elif defined(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB)
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &num);
#endif
		MaxSupportedTextures = jmax( MaxSupportedTextures,static_cast<uint8>(num) );
	}
#endif
	glGetIntegerv(GL_MAX_LIGHTS, &num);
	MaxLights = static_cast<uint8>(num);

#ifdef GL_EXT_texture_filter_anisotropic
	if( GLEE_EXT_texture_filter_anisotropic )
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &num);
		MaxAnisotropy = static_cast<uint8>(num);
	}
#endif

#ifdef GL_VERSION_1_2
	if( Version > 110 )
	{
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &num);
		MaxIndices = num;
	}
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &num);
	MaxTextureSize = static_cast<uint32>(num);

	if( queryDriverFeature(SGPVDF_GEOMETRY_SHADER) )
	{
#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_shader4)
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &num);
		MaxGeometryVerticesOut = static_cast<uint32>(num);
#elif defined(GL_NV_geometry_program4)
		extGlGetProgramiv(GEOMETRY_PROGRAM_NV, GL_MAX_PROGRAM_OUTPUT_VERTICES_NV, &num);
		MaxGeometryVerticesOut = static_cast<uint32>(num);
#endif
	}

#ifdef GL_EXT_texture_lod_bias
	if( GLEE_EXT_texture_lod_bias )
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS_EXT, &MaxTextureLODBias);
#endif

	glGetIntegerv(GL_MAX_CLIP_PLANES, &num);
	MaxUserClipPlanes = static_cast<uint8>(num);
	glGetIntegerv(GL_AUX_BUFFERS, &num);
	MaxAuxBuffers = static_cast<uint8>(num);

#ifdef GL_ARB_draw_buffers
	if( GLEE_ARB_draw_buffers )
	{
		glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &num);
		MaxMultipleRenderTargets = static_cast<uint8>(num);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &num);
		MaxColorAttachementPoints = static_cast<uint8>(num);
	}
#endif
#if defined(GL_ATI_draw_buffers)
#ifdef GL_ARB_draw_buffers
	else
#endif
	if( GLEE_ATI_draw_buffers )
	{
		glGetIntegerv(GL_MAX_DRAW_BUFFERS_ATI, &num);
		MaxMultipleRenderTargets = static_cast<uint8>(num);
	}
#endif
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, DimSmoothedLine);
	glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, DimSmoothedPoint);
#if defined(GL_ARB_shading_language_100) || defined (GL_VERSION_2_0)
	if (GLEE_ARB_shading_language_100 || Version>=200)
	{
		glGetError(); // clean error buffer
#ifdef GL_SHADING_LANGUAGE_VERSION
		const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
#else
		const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
#endif
		if (glGetError() == GL_INVALID_ENUM)
			ShaderLanguageVersion = 100;
		else
		{
			const double sl_ver = String(reinterpret_cast<const char*>(shaderVersion)).getDoubleValue();
			ShaderLanguageVersion = static_cast<uint16>(sl_ver*100.0f);
		}
	}
#endif

	MaxTextureUnits = jmin(MaxSupportedTextures, static_cast<uint8>(SGP_MATERIAL_MAX_TEXTURES));
	if (MaxTextureUnits < 2)
	{
		MultiTextureExtension = false;
		//os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
	}

#ifdef GL_ARB_occlusion_query
	if( GLEE_ARB_occlusion_query )
	{
		extGlGetQueryiv(GL_SAMPLES_PASSED_ARB,GL_QUERY_COUNTER_BITS_ARB, &num);
		OcclusionQuerySupport = (num>0);
	}
	else
#endif
#ifdef GL_NV_occlusion_query
	if (GLEE_NV_occlusion_query)
	{
		glGetIntegerv(GL_PIXEL_COUNTER_BITS_NV, &num);
		OcclusionQuerySupport = (num>0);
	}
	else
#endif
		OcclusionQuerySupport = false;

}

bool COpenGLExtensionHandler::queryDriverFeature(SGP_DRIVER_FEATURE feature) const
{
	switch (feature)
	{
	case SGPVDF_RENDER_TO_TARGET:
		return true;
	case SGPVDF_HARDWARE_TL:
		return true; // we cannot tell other things
	case SGPVDF_MULTITEXTURE:
		return MultiTextureExtension;
	case SGPVDF_BILINEAR_FILTER:
		return true;
	case SGPVDF_MIP_MAP:
		return true;
	case SGPVDF_MIP_MAP_GEN_HW:
		return (Version >= 300) || GLEE_EXT_framebuffer_object || GLEE_ARB_framebuffer_object;
	case SGPVDF_STENCIL_BUFFER:
		return StencilBuffer;
	case SGPVDF_VERTEX_SHADER_1_1:
	case SGPVDF_ARB_VERTEX_PROGRAM_1:
		return GLEE_ARB_vertex_program || GLEE_NV_vertex_program1_1;
	case SGPVDF_PIXEL_SHADER_1_1:
	case SGPVDF_PIXEL_SHADER_1_2:
	case SGPVDF_ARB_FRAGMENT_PROGRAM_1:
		return GLEE_ARB_fragment_program || GLEE_NV_fragment_program;
	case SGPVDF_PIXEL_SHADER_2_0:
	case SGPVDF_VERTEX_SHADER_2_0:
	case SGPVDF_ARB_GLSL:
		return GLEE_ARB_shading_language_100 || (Version >= 200);
	case SGPVDF_TEXTURE_NSQUARE:
		return true; // non-square is always supported
	case SGPVDF_TEXTURE_NPOT:
		return GLEE_ARB_texture_non_power_of_two ? true : false;
	case SGPVDF_FRAMEBUFFER_OBJECT:
		return GLEE_EXT_framebuffer_object || GLEE_ARB_framebuffer_object;
	case SGPVDF_VERTEX_BUFFER_OBJECT:
		return GLEE_ARB_vertex_buffer_object ? true : false;
	case SGPVDF_COLOR_MASK:
		return true;
	case SGPVDF_ALPHA_TO_COVERAGE:
		return GLEE_ARB_multisample ? true : false;
	case SGPVDF_GEOMETRY_SHADER:
		return GLEE_ARB_geometry_shader4 || GLEE_EXT_geometry_shader4 || GLEE_NV_geometry_shader4;
	case SGPVDF_MULTIPLE_RENDER_TARGETS:
		return GLEE_ARB_draw_buffers || GLEE_ATI_draw_buffers;
	case SGPVDF_MRT_BLEND:
	case SGPVDF_MRT_COLOR_MASK:
		return GLEE_EXT_draw_buffers2 ? true : false;
	case SGPVDF_MRT_BLEND_FUNC:
		return GLEE_ARB_draw_buffers_blend || GLEE_AMD_draw_buffers_blend;
	case SGPVDF_OCCLUSION_QUERY:
		return GLEE_ARB_occlusion_query && OcclusionQuerySupport;
	case SGPVDF_POLYGON_OFFSET:
		// both features supported with OpenGL 1.1
		return Version >= 110;
	case SGPVDF_BLEND_OPERATIONS:
		return (Version>=120) || 
			GLEE_EXT_blend_minmax || GLEE_EXT_blend_subtract || GLEE_EXT_blend_logic_op;
	case SGPVDF_TEXTURE_MATRIX:
		return true;
	default:
		return false;
	};
}

