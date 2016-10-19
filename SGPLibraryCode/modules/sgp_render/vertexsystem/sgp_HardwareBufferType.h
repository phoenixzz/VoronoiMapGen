#ifndef __SGP_HWBUFFERTYPE_HEADER__
#define __SGP_HWBUFFERTYPE_HEADER__
/*
STATIC    The data store contents will be created once and used many times.
DYNAMIC   The data store contents will be modified repeatedly and used many times.
STREAM    The data store contents will be created once and used at most a few times.
*/
	enum SGP_HARDWARE_MAPPING
	{
		//! Rarely changed, always used. usually stored completely on the hardware
		SGPHM_STATIC = 0,

		//! Always changed, always used. cache optimizing on the GPU		
		SGPHM_DYNAMIC,

		//! Sometimes changed, less used. driver optimized placement
		SGPHM_STREAM
	};

	enum SGP_BUFFER_TYPE
	{
		//! Does not change anything
		SGPBT_NONE=0,
		//! Change the vertex mapping
		SGPBT_VERTEX,
		//! Change the index mapping
		SGPBT_INDEX,
		//! Change both vertex and index mapping to the same value
		SGPBT_VERTEX_AND_INDEX
	};

#endif		// __SGP_HWBUFFERTYPE_HEADER__