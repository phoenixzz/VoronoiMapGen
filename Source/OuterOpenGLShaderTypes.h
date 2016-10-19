#ifndef __OUTER_SHADERTYPES_HEADER__
#define __OUTER_SHADERTYPES_HEADER__


	//! Abstracted and easy to use programmabled pipeline shader modes for Application used
	enum OUTER_SHADER_TYPE
	{
		Filters_Normal = 0,

		//! This value is not used. just for counting the elements num
		OUTER_SHADER_NUM,

		//! This value is not used. It only forces this enumeration to compile to 32 bit.
		OUTER_FORCE_32BIT = 0x7fffffff
	};

	//! Array holding the outer shader type names
	const char* const sOuterShaderTypeNames[] =
	{
		"FNormal",

		0
	};

#endif		// __OUTER_SHADERTYPES_HEADER__