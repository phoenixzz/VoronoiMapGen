#ifndef __SGP_MATERIALFLAGS_HEADER__
#define __SGP_MATERIALFLAGS_HEADER__

//! Maximum number of texture an Material can have, up to 9 are supported by SGP.
#define SGP_MATERIAL_MAX_TEXTURES 9

	//! Material flags
	//enum SGP_MATERIAL_FLAG
	//{
	//	//! Draw as wireframe or filled triangles? Default: false
	//	SGPMF_WIREFRAME = 0x1,

	//	//! Draw as point or filled triangles? Default: false
	//	SGPMF_POINT = 0x2,

	//	//! Flat or Gouraud shading? Default: true
	//	SGPMF_GOURAUD_SHADING = 0x4,

	//	//! Will this material be lighted? Default: false
	//	SGPMF_LIGHTING = 0x8,

	//	//! Is the ZBuffer enabled? Default: true
	//	SGPMF_ZBUFFER = 0x10,

	//	//! May be written to the zbuffer or is it readonly. Default: true
	//	/** This flag is ignored, if the material type is a transparent type. */
	//	SGPMF_ZWRITE_ENABLE = 0x20,

	//	//! Is backface culling enabled? Default: true
	//	SGPMF_BACK_FACE_CULLING = 0x40,

	//	//! Is frontface culling enabled? Default: false
	//	/** Overrides EMF_BACK_FACE_CULLING if both are enabled. */
	//	SGPMF_FRONT_FACE_CULLING = 0x80,

	//	//! Is bilinear filtering enabled? Default: true
	//	SGPMF_BILINEAR_FILTER = 0x100,

	//	//! Is trilinear filtering enabled? Default: false
	//	/** If the trilinear filter flag is enabled,
	//	the bilinear filtering flag is ignored. */
	//	SGPMF_TRILINEAR_FILTER = 0x200,

	//	//! Is anisotropic filtering? Default: false
	//	/** In SGP you can use anisotropic texture filtering in
	//	conjunction with bilinear or trilinear texture filtering
	//	to improve rendering results. Primitives will look less
	//	blurry with this flag switched on. */
	//	SGPMF_ANISOTROPIC_FILTER = 0x400,

	//	//! Is fog enabled? Default: false
	//	SGPMF_FOG_ENABLE = 0x800,

	//	//! Normalizes normals. Default: false
	//	/** You can enable this if you need to scale a dynamic lighted
	//	model. Usually, its normals will get scaled too then and it
	//	will get darker. If you enable the SGPMF_NORMALIZE_NORMALS flag,
	//	the normals will be normalized again, and the model will look
	//	as bright as it should. */
	//	SGPMF_NORMALIZE_NORMALS = 0x1000,

	//	//! Access to all layers texture wrap settings. Overwrites separate layer settings.
	//	SGPMF_TEXTURE_WRAP = 0x2000,

	//	//! AntiAliasing mode
	//	SGPMF_ANTI_ALIASING = 0x4000,

	//	//! ColorMask bits, for enabling the color channels
	//	SGPMF_COLOR_MASK = 0x8000,

	//	//! ColorMaterial enum for vertex color interpretation
	//	SGPMF_COLOR_MATERIAL = 0x10000,

	//	//! Flag for enabling/disabling mipmap usage
	//	SGPMF_USE_MIP_MAPS = 0x20000,

	//	//! Flag for blend operation
	//	SGPMF_BLEND_OPERATION = 0x40000,

	//	//! Flag for polygon offset
	//	SGPMF_POLYGON_OFFSET = 0x80000
	//};

	//! Values defining the blend operation used when blend is enabled
	enum SGP_BLEND_OPERATION
	{
		SGPBO_NONE = 0,		//!< No blending happens
		SGPBO_ADD,			//!< Default blending adds the color values
		SGPBO_SUBTRACT,		//!< This mode subtracts the color values
		SGPBO_REVSUBTRACT,	//!< This modes subtracts destination from source
		SGPBO_MIN,			//!< Choose minimum value of each color channel
		SGPBO_MAX,			//!< Choose maximum value of each color channel
	};

	//! Flag for blend operation, ( BlendFactor ) BlendFunc = source * sourceFactor + dest * destFactor
	enum SGP_BLEND_FACTOR
	{
		SGPBF_ZERO	= 0,				//!<	(0, 0, 0, 0)
		SGPBF_ONE,						//!<	(1, 1, 1, 1)
		SGPBF_DST_COLOR, 				//!< 	(destR, destG, destB, destA)
		SGPBF_ONE_MINUS_DST_COLOR,		//!< 	(1-destR, 1-destG, 1-destB, 1-destA)
		SGPBF_SRC_COLOR,				//!< 	(srcR, srcG, srcB, srcA)
		SGPBF_ONE_MINUS_SRC_COLOR, 		//!< 	(1-srcR, 1-srcG, 1-srcB, 1-srcA)
		SGPBF_SRC_ALPHA,				//!< 	(srcA, srcA, srcA, srcA)
		SGPBF_ONE_MINUS_SRC_ALPHA,		//!< 	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
		SGPBF_DST_ALPHA,				//!< 	(destA, destA, destA, destA)
		SGPBF_ONE_MINUS_DST_ALPHA,		//!< 	(1-destA, 1-destA, 1-destA, 1-destA)
		SGPBF_SRC_ALPHA_SATURATE		//!< 	(min(srcA, 1-destA), idem, ...)
	};

	//! Comparison function, e.g. for depth buffer test
	enum SGP_COMPARISON_FUNC
	{
		//! Test never succeeds, this equals disable
		SGPCFN_NEVER=0,
		//! <= test, default for e.g. depth test
		SGPCFN_LESSEQUAL=1,
		//! Exact equality
		SGPCFN_EQUAL=2,
		//! exclusive less comparison, i.e. <
		SGPCFN_LESS,
		//! Succeeds almost always, except for exact equality
		SGPCFN_NOTEQUAL,
		//! >= test
		SGPCFN_GREATEREQUAL,
		//! inverse of <=
		SGPCFN_GREATER,
		//! test succeeds always
		SGPCFN_ALWAYS
	};

	//! These flags are used to specify the anti-aliasing and smoothing modes
	/** Techniques supported are multisampling, geometry smoothing, and alpha
	to coverage.
	Some drivers don't support a per-material setting of the anti-aliasing
	modes. In those cases, FSAA/multisampling is defined by the device mode
	chosen upon creation via SGPCreationParameters.
	*/
	enum SGP_ANTI_ALIASING_MODE
	{
		//! Use to turn off anti-aliasing for this material
		SGPAAM_OFF=0,
		//! Default anti-aliasing mode
		SGPAAM_SIMPLE=1,
		//! High-quality anti-aliasing, not always supported, automatically enables SIMPLE mode
		SGPAAM_QUALITY=3,
		//! Line smoothing
		SGPAAM_LINE_SMOOTH=4,
		//! point smoothing, often in software and slow, only with OpenGL
		SGPAAM_POINT_SMOOTH=8,
		//! All typical anti-alias and smooth modes
		SGPAAM_FULL_BASIC=15,
		//! Enhanced anti-aliasing for transparent materials
		/** Usually used with EMT_TRANSPARENT_ALPHA_REF and multisampling. */
		SGPAAM_ALPHA_TO_COVERAGE=16
	};

	//! MaterialTypeParam: e.g. DirectX: D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X
	enum SGP_MODULATE_FUNC
	{
		SGPMFN_MODULATE_1X	= 1,
		SGPMFN_MODULATE_2X	= 2,
		SGPMFN_MODULATE_4X	= 4
	};

	//! Source of the alpha value to take
	/** You can use an or 'OR' combination of values. Alpha values are modulated (multiplicated). */
	enum SGP_ALPHA_SOURCE
	{
		//! Use no alpha, somewhat redundant with other settings
		SGPAS_NONE = 0,
		//! Use vertex color alpha
		SGPAS_VERTEX_COLOR = 1,
		//! Use texture alpha channel
		SGPAS_TEXTURE = 1 << 1
	};

	//! pack srcFact, dstFact, Modulate and alpha source to MaterialTypeParam
	/** alpha source can be an OR'ed combination of SGP_ALPHA_SOURCE values. */
	inline uint32 pack_textureBlendFunc ( const SGP_BLEND_FACTOR srcFact, const SGP_BLEND_FACTOR dstFact, const SGP_MODULATE_FUNC modulate=SGPMFN_MODULATE_1X, const uint32 alphaSource=SGPAS_TEXTURE )
	{
		const uint32 tmp = (alphaSource << 12) | (modulate << 8) | (srcFact << 4) | dstFact;
		return tmp;
	}

	//! unpack srcFact & dstFact and Modulo to MaterialTypeParam
	/** The fields don't use the full byte range, so we could pack even more... */
	inline void unpack_textureBlendFunc ( SGP_BLEND_FACTOR &srcFact, SGP_BLEND_FACTOR &dstFact,
			SGP_MODULATE_FUNC &modulo, uint32& alphaSource, const uint32 param )
	{
		alphaSource = (param & 0x0000F000) >> 12;
		modulo	= SGP_MODULATE_FUNC( ( param & 0x00000F00 ) >> 8 );
		srcFact = SGP_BLEND_FACTOR ( ( param & 0x000000F0 ) >> 4 );
		dstFact = SGP_BLEND_FACTOR ( ( param & 0x0000000F ) );
	}


#endif	// __SGP_MATERIALFLAGS_HEADER__