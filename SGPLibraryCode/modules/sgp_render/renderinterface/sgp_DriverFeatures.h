#ifndef __SGP_DRIVERFEATURES_HEADER__
#define __SGP_DRIVERFEATURES_HEADER__

	//! enumeration for querying features of the video driver.
	enum SGP_DRIVER_FEATURE
	{
		//! Is driver able to render to a surface?
		SGPVDF_RENDER_TO_TARGET = 0,

		//! Is hardeware transform and lighting supported?
		SGPVDF_HARDWARE_TL,

		//! Are multiple textures per material possible?
		SGPVDF_MULTITEXTURE,

		//! Is driver able to render with a bilinear filter applied?
		SGPVDF_BILINEAR_FILTER,

		//! Can the driver handle mip maps?
		SGPVDF_MIP_MAP,

		//! Can the driver update mip maps auto generated?
		SGPVDF_MIP_MAP_GEN_HW,

		//! Are stencilbuffers switched on and does the device support stencil buffers?
		SGPVDF_STENCIL_BUFFER,

		//! Is Vertex Shader 1.1 supported?
		SGPVDF_VERTEX_SHADER_1_1,

		//! Is Vertex Shader 2.0 supported?
		SGPVDF_VERTEX_SHADER_2_0,

		//! Is Vertex Shader 3.0 supported?
		SGPVDF_VERTEX_SHADER_3_0,

		//! Is Pixel Shader 1.1 supported?
		SGPVDF_PIXEL_SHADER_1_1,

		//! Is Pixel Shader 1.2 supported?
		SGPVDF_PIXEL_SHADER_1_2,

		//! Is Pixel Shader 1.3 supported?
		SGPVDF_PIXEL_SHADER_1_3,

		//! Is Pixel Shader 1.4 supported?
		SGPVDF_PIXEL_SHADER_1_4,

		//! Is Pixel Shader 2.0 supported?
		SGPVDF_PIXEL_SHADER_2_0,

		//! Is Pixel Shader 3.0 supported?
		SGPVDF_PIXEL_SHADER_3_0,

		//! Is Pixel Shader 4.0 supported?
		SGPVDF_PIXEL_SHADER_4_0,

		//! Are ARB vertex programs v1.0 supported?
		SGPVDF_ARB_VERTEX_PROGRAM_1,

		//! Are ARB fragment programs v1.0 supported?
		SGPVDF_ARB_FRAGMENT_PROGRAM_1,

		//! Is GLSL supported?
		SGPVDF_ARB_GLSL,

		//! Is HLSL supported?
		SGPVDF_HLSL,

		//! Are non-square textures supported?
		SGPVDF_TEXTURE_NSQUARE,

		//! Are non-power-of-two textures supported?
		SGPVDF_TEXTURE_NPOT,

		//! Are framebuffer objects supported?
		SGPVDF_FRAMEBUFFER_OBJECT,

		//! Are vertex buffer objects supported?
		SGPVDF_VERTEX_BUFFER_OBJECT,

		//! Supports Alpha To Coverage
		SGPVDF_ALPHA_TO_COVERAGE,

		//! Supports Color masks (disabling color planes in output)
		SGPVDF_COLOR_MASK,

		//! Supports multiple render targets at once
		SGPVDF_MULTIPLE_RENDER_TARGETS,

		//! Supports separate blend settings for multiple render targets
		SGPVDF_MRT_BLEND,

		//! Supports separate color masks for multiple render targets
		SGPVDF_MRT_COLOR_MASK,

		//! Supports separate blend functions for multiple render targets
		SGPVDF_MRT_BLEND_FUNC,

		//! Supports geometry shaders
		SGPVDF_GEOMETRY_SHADER,

		//! Supports occlusion queries
		SGPVDF_OCCLUSION_QUERY,

		//! Supports polygon offset/depth bias for avoiding z-fighting
		SGPVDF_POLYGON_OFFSET,

		//! Support for different blend functions. Without, only ADD is available
		SGPVDF_BLEND_OPERATIONS,

		//! Support for texture coord transformation via texture matrix
		SGPVDF_TEXTURE_MATRIX,

		//! Only used for counting the elements of this enum
		SGPVDF_COUNT
	};
#endif	// __SGP_DRIVERFEATURES_HEADER__