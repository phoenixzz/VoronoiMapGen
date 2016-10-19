#ifndef __SGP_SHADERTYPES_HEADER__
#define __SGP_SHADERTYPES_HEADER__

	//! Abstracted and easy to use programmabled pipeline shader modes.
	enum SGP_SHADER_TYPE
	{
		//! Standard solid material.
		/** No texture used, vertex color as diffuse material*/
		SGPST_VERTEXCOLOR = 0,

		/** Only first texture is used, which is supposed to be the	diffuse material. */
		SGPST_TEXTURE,

#if !defined(BUILD_OGLES2)
		/** Only first texture is used, which is supposed to be the	diffuse material. */
		// texture alpha chanel is used for alphatest
		SGPST_TEXTURE_ALPHATEST,
#endif
		//! Standard solid material.
		/** Only first texture is used, the vertex colors multiplied by diffuse map*/
		SGPST_VERTEXCOLOR_TEXTURE,
#if !defined(BUILD_OGLES2)
		//! Standard solid material.
		/** Only first texture is used, the vertex colors multiplied by diffuse map*/
		// texture alpha chanel is used for alphatest
		SGPST_VERTEXCOLOR_TEXTURE_ALPHATEST,
#endif

#if 0
		//! Standard solid material.
		/** Only first texture is used, the vertex colors multiplied by diffuse map,
		The texture colors are effectively multiplied by 2 for brightening.*/
		SGPST_VERTEXCOLOR_TEXTURE_M2,
#endif
		//! Shader type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is ignored. */
		SGPST_LIGHTMAP,
#if !defined(BUILD_OGLES2)
		//! Shader type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is ignored. */
		// diffuse map alpha chanel is used for alphatest
		SGPST_LIGHTMAP_ALPHATEST,
#endif
		//! Standard solid material with lightmap
		/** There should be 2 textures be used,The first texture layer is a
		diffuse map, the second is a light map. 
		the vertex colors multiplied by diffuse map, then multiplied by lightmap*/
		SGPST_VERTEXCOLOR_LIGHTMAP,
#if !defined(BUILD_OGLES2)
		//! Standard solid material with lightmap
		/** There should be 2 textures be used,The first texture layer is a
		diffuse map, the second is a light map. 
		the vertex colors multiplied by diffuse map, then multiplied by lightmap*/
		// diffuse map alpha chanel is used for alphatest
		SGPST_VERTEXCOLOR_LIGHTMAP_ALPHATEST,
#endif
#if 0
		//! Material type with lightmap technique like SGPST_LIGHTMAP.
		/** But lightmap and diffuse texture are added instead of modulated. */
		SGPST_LIGHTMAP_ADD,

		//! Material type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is
		ignored. The texture colors are effectively multiplied by 2
		for brightening. Like known in DirectX as D3DTOP_MODULATE2X. */
		SGPST_LIGHTMAP_M2,

		//! Material type with standard lightmap technique
		/** There should be 2 textures: The first texture layer is a
		diffuse map, the second is a light map. Dynamic light is
		ignored. The texture colors are effectively multiplyied by 4
		for brightening. Like known in DirectX as D3DTOP_MODULATE4X. */
		SGPST_LIGHTMAP_M4,

		//! Shader type with only one standard dynamic light technique
		/** The only one texture layer is a diffuse map*/
		SGPST_LIGHTING,

		//! Like SGPST_LIGHTMAP, but also supports dynamic lighting.
		SGPST_LIGHTMAP_LIGHTING,

		//! Like SGPST_LIGHTMAP_M2, but also supports dynamic lighting.
		SGPST_LIGHTMAP_LIGHTING_M2,

		//! Like SGPST_LIGHTMAP_4, but also supports dynamic lighting.
		SGPST_LIGHTMAP_LIGHTING_M4,

		//! Detail mapped material.
		/** The first texture is diffuse color map, the second is added
		to this and usually displayed with a bigger scale value so that
		it adds more detail. The detail map is added to the diffuse map
		using ADD_SIGNED, so that it is possible to add and substract
		color from the diffuse map. For example a value of
		(127,127,127) will not change the appearance of the diffuse map
		at all. Often used for terrain rendering. */
		SGPST_DETAIL_MAP,

		//! Look like a reflection of the environment around it.
		/** To make this possible, a texture called 'sphere map' is
		used, which must be set as the first texture. */
		SGPST_SPHERE_MAP,

		//! Makes the material transparent based on the texture alpha channel.
		/** The final color is blended together from the destination
		color and the texture color, using the alpha channel value as
		blend factor. Only first texture is used. */
		SGPST_TRANSPARENT_ALPHA_CHANNEL,

		//! Makes the material opaque based on the texture alpha channel ref.
		/** If the alpha channel value is greater than 127, a
		pixel is written to the target, otherwise not. This
		material does not use alpha blending and is a lot faster
		than SGPMT_TRANSPARENT_ALPHA_CHANNEL. It is ideal for drawing
		stuff like leafes of plants, because the borders are not
		blurry but sharp. Only first texture is used. */
		// ALPHA-TEST
		SGPST_TRANSPARENT_ALPHA_CHANNEL_REF,


		//! Makes the material transparent based on the vertex alpha value.
		SGPST_TRANSPARENT_VERTEX_ALPHA,


		//! A solid normal map renderer.
		/** First texture is the color map, the second should be the
		normal map. */
		SGPST_NORMAL_MAP_OPAQUE,

		//! A transparent normal map renderer.
		/** First texture is the color map, the second should be the
		normal map, using the alpha channel value as blend factor */
		SGPST_NORMAL_MAP_TRANSPARENT_ALPHA_CHANNEL,

		//! A transparent (based on the vertex alpha value) normal map renderer.
		/** First texture is the color map, the second should be the normal map. */
		SGPST_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA,

		//! A solid Full screen QUAD renderer
		// NO vertex transform, output to PS directly and 1~many textures is used. 
		SGPST_AFTEREFFECT,

#endif

		//! A transparent (based on the vertex alpha value X texture alpha channel)
		// NO vertex transform, output to PS directly
		// Only first texture is used, the vertex colors multiplied by diffuse map,
		// Alpha value is vertex alpha mul texture alpha
		SGPST_UI,

		//! Used for character animation rendering
		// first texture is used, the vertex normal for vertex lighting 
		// also four skeleton bone indices and weights
		SGPST_SKELETONANIM,
#if !defined(BUILD_OGLES2)
		//! Used for character animation rendering
		// first texture is used for diffuse, the vertex normal for vertex lighting 
		// texture alpha chanel is used for alphatest
		// also four skeleton bone indices and weights
		SGPST_SKELETONANIM_ALPHATEST,
#endif
		//! Used for particle point sprites rendering
		// first texture is used, the vertex color is used
		// output alpha value is texture alpha mul vertex alpha
		// Alpha mode BLEND_ADD and BLEND_ALPHA in same shader
		SGPST_PARTICLE_POINTSPRITES,

		//! Used for particle line rendering
		// only the vertex color is used
		// output alpha value is vertex alpha
		// Alpha mode BLEND_ADD and BLEND_ALPHA in same shader
		SGPST_PARTICLE_LINE,

		//! Used for particle QUAD rendering
		// Alpha mode BLEND_ADD and BLEND_ALPHA in same shader
		SGPST_PARTICLE_QUAD_TEXATLAS,	// vertex color with Tiled texture

		//! Used for terrain chunk very detailed rendering
		// Normal map + Dynamic light +  4 layer textures blend + detailmap + slope map
		SGPST_TERRAIN_VERYHIGH,

		//! Used for terrain chunk LOD0 rendering
		// Dynamic light +  4 layer textures blend + slope map
		SGPST_TERRAIN_LOD0,
		
		//! Used for terrain chunk LOD1 rendering
		// one mini color map 128*128 (16¡Á16 chunks)
		SGPST_TERRAIN_LOD1,

		//! Used for terrain chunk Tile LOD blending rendering
		// LOD1 shading effect - one mini color map 128*128 (16¡Á16 chunks)
		SGPST_TERRAIN_LODBLEND,
#if !defined(BUILD_OGLES2)
		//! Used for skydome rendering
		// Only first texture is used, the vertex colors calculated by sky dome height,
		// and multiplied by sky cloud texture
		SGPST_SKYDOME,
#endif
		SGPST_SKYDOMESCATTERING,

		//! A water refraction renderer
		// vertex type likes terrain chunk LOD1 rendering
		SGPST_WATER_REFRACTION,

		//! A dynamic water final renderer
		// vertex type likes terrain chunk LOD1 rendering
		SGPST_WATER_RENDER,

		//! Used for Grass geometry instancing rendering with Tiled texture
		// local vertex position and local UV as static VB
		// instanced position + normal + UV index + vertex color + wind params as dynamic VB
		SGPST_GRASS_INSTANCING,

		//! This value is not used. just for counting the elements num
		SGPST_SHADER_NUM,

		//! This value is not used. It only forces this enumeration to compile to 32 bit.
		SGPST_FORCE_32BIT = 0x7fffffff
	};

	//! Array holding the built in shader type names
	const char* const sBuiltInShaderTypeNames[] =
	{
		"vertexcolor",
		"texture",
#if !defined(BUILD_OGLES2)
		"texture_alphatest",
#endif
		"vertexcolor_texture",
#if !defined(BUILD_OGLES2)
		"vertexcolor_texture_alphatest",
#endif
		//"vertexcolor_texture_m2",
		"lightmap",
#if !defined(BUILD_OGLES2)
		"lightmap_alphatest",
#endif
		"vertexcolor_lightmap",
#if !defined(BUILD_OGLES2)
		"vertexcolor_lightmap_alphatest",

		//"lightmap_add",
		//"lightmap_m2",
		//"lightmap_m4",
		//"onelighting",
		//"lightmap_lighting",
		//"lightmap_lighting_m2",
		//"lightmap_lighting_m4",
		//"detail_map",
		//"sphere_map",
		//"trans_alphachannel",
		//"trans_alphachannel_ref",
		//"trans_vertex_alpha",
		//"normalmap_opaque",
		//"normalmap_trans_alphachannel",
		//"normalmap_trans_vertexalpha",
		//"aftereffect",
		//"water",
#endif
		"ui",

		"skeleton_anim",
#if !defined(BUILD_OGLES2)
		"skeleton_anim_alphatest",
#endif
		"particle_ps",
		"particle_line",
		"particle_quad_texatlas",
		"terrain_veryhigh",
		"terrain_lod0",
		"terrain_lod1",
		"terrain_lodblend",
#if !defined(BUILD_OGLES2)
		"skydome",
#endif
		"skydomeScattering",
		"water_refraction",
		"water_surfacerender",
		"grass_instance",
		0
	};



#endif		// __SGP_SHADERTYPES_HEADER__