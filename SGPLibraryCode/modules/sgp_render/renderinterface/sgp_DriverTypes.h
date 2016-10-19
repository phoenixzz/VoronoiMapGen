#ifndef __SGP_DRIVERTYPE_HEADER__
#define __SGP_DRIVERTYPE_HEADER__

	//! An enum for all types of drivers the SGP Engine supports.
	enum SGP_DRIVER_TYPE
	{
		//! Direct3D 11 device, only available on Win32 platforms.
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		SGPDT_DIRECT3D11,

		//! OpenGL device, available on most platforms.
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		SGPDT_OPENGL,

		//! OpenGL ES 2.0 device
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		SGPDT_OPENGLES2,

		//! No driver, just for counting the elements
		SGPDT_COUNT
	};

#endif	// __SGP_DRIVERTYPE_HEADER__