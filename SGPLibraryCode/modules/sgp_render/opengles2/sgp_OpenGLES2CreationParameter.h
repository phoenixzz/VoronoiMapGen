#ifndef __SGP_OPENGLES2CREATIONPARAMETERS_HEADER__
#define __SGP_OPENGLES2CREATIONPARAMETERS_HEADER__



struct SGPOpenGLES2CreationParameters
{
		//! Constructs a SGPCreationParameters structure with default values.
		SGPOpenGLES2CreationParameters() :
			DriverType(SGPDT_OPENGLES2),
			Bits(32),
			ZBufferBits(16),
			Fullscreen(false),
			Stencilbuffer(false),
			Vsync(false),
			AntiAlias(0),
			HandleSRGB(false),
			bPowerSaving(false),
			bShowCursor(true),
			bDiscardColor(false),
			bDiscardDepth(true),
			bDiscardStencil(true),
			MultiThreadResLoading(false),
			bRotated(false),
			plog(0),
#if SGP_DEBUG
			LoggingLevel(ELL_DEBUG)
#else
			LoggingLevel(ELL_WARNING)
#endif
		{
			WindowWidth = 640;
			WindowHeight = 1136;
			WindowPositionX = 0;
			WindowPositionY = 0;
		}

		SGPOpenGLES2CreationParameters(const SGPOpenGLES2CreationParameters& other)
		{ *this = other; }

		SGPOpenGLES2CreationParameters& operator=(const SGPOpenGLES2CreationParameters& other)
		{
			DriverType = other.DriverType;
			WindowWidth = other.WindowWidth;
			WindowHeight = other.WindowHeight;
			WindowPositionX = other.WindowPositionX;
			WindowPositionY = other.WindowPositionY;
			Bits = other.Bits;
			ZBufferBits = other.ZBufferBits;
			Fullscreen = other.Fullscreen;
			Stencilbuffer = other.Stencilbuffer;
			Vsync = other.Vsync;
			AntiAlias = other.AntiAlias;
			HandleSRGB = other.HandleSRGB;
			bPowerSaving = other.bPowerSaving;
			bShowCursor = other.bShowCursor;
			bDiscardColor = other.bDiscardColor;
			bDiscardDepth = other.bDiscardDepth;
			bDiscardStencil = other.bDiscardStencil;
			MultiThreadResLoading = other.MultiThreadResLoading;
			bRotated = other.bRotated;
			LoggingLevel = other.LoggingLevel;
			plog = other.plog;
			return *this;
		}


		//! Type of video driver used to render graphics.
		/** This can currently be SGPDT_DIRECT3D11, and SGPDT_OPENGL.
		Default: SGPDT_OPENGLES2. */
		SGP_DRIVER_TYPE DriverType;

		//! Size of the window or the video mode in fullscreen mode. Default: 800x600
		uint32 WindowWidth;
		uint32 WindowHeight;

		//! X and Y position of the window
		int32  WindowPositionX;
		int32  WindowPositionY;

		//! Minimum Bits per pixel of the color buffer in fullscreen mode. Ignored if windowed mode. Default: 32.
		uint8 Bits;

		//! Minimum Bits per pixel of the depth buffer. Default: 16.
		uint8 ZBufferBits;

		//! Should be set to true if the device should run in fullscreen.
		/** Otherwise the device runs in windowed mode. Default: false. */
		bool Fullscreen;

		//! Specifies if the stencil buffer should be enabled.
		/** Set this to true, if you want the engine be able to draw
		stencil buffer. Note that not all drivers are able to
		use the stencil buffer, hence it can be ignored during device creation. 
		Default: false. */
		bool Stencilbuffer;

		//! Specifies vertical syncronisation.
		/** If set to true, the driver will wait for the vertical
		retrace period, otherwise not. May be silently ignored.
		Default: false */
		bool Vsync;

		//! Specifies if the device should use fullscreen anti aliasing
		/** Makes sharp/pixelated edges softer, but requires more
		performance. Also, 2D elements might look blurred with this
		switched on. The resulting rendering quality also depends on
		the hardware and driver you are using, your program might look
		different on different hardware with this. So if you are
		writing a game/application with AntiAlias switched on, it would
		be a good idea to make it possible to switch this option off
		again by the user.
		The value is the maximal antialiasing factor requested for
		the device. The cretion method will automatically try smaller
		values if no window can be created with the given value.
		Value one is usually the same as 0 (disabled), but might be a
		special value on some platforms. On D3D devices it maps to
		NONMASKABLE.
		Default value: 0 - disabled */
		uint32 AntiAlias;

		//! Flag to enable proper sRGB and linear color handling
		/** In most situations, it is desireable to have the color handling in
		non-linear sRGB color space, and only do the intermediate color
		calculations in linear RGB space. If this flag is enabled, the device and
		driver try to assure that all color input and output are color corrected
		and only the internal color representation is linear. This means, that
		the color output is properly gamma-adjusted to provide the brighter
		colors for monitor display. And that blending and lighting give a more
		natural look, due to proper conversion from non-linear colors into linear
		color space for blend operations. If this flag is enabled, all texture colors
		(which are usually in sRGB space) are correctly displayed. However vertex colors
		and other explicitly set values have to be manually encoded in linear color space.
		Default value: false. */
		bool HandleSRGB;


		//! Whether the resource loading uses Multi-threading.
		/** resource usually include Model files and texture files from hard disk
		    In real-time game, this should be usually enabled, in order to avoid render
			stalling and keep smooth FPS.
			In other tools,  Default value: false
		*/
		bool MultiThreadResLoading;

		//! If true then the app will go into powersaving mode (if available) when not in use. Default value: false
		bool bPowerSaving;

		//! Set to: true to show the cursor; false to hide it. Default value: true
		bool bShowCursor;

		/*!< GLES: Whether or not to discard color data at the end of a render, to save bandwidth. Requires specific functionality. (default: false) */
		bool bDiscardColor;
		/*!< GLES: Whether or not to discard depth data at the end of a render, to save bandwidth. Requires specific functionality. (default: true) */
		bool bDiscardDepth; 
		/*!< GLES: Whether or not to discard stencil data at the end of a render, to save bandwidth. Requires specific functionality. (default: true) */
		bool bDiscardStencil;

		// Is the viewport in portrait or landscape mode
		bool bRotated;

		//! Specifies the logging level used in the logging interface.
		/** The default value is ELL_INFORMATION. You can access the ILogger interface
		later on from the IrrlichtDevice with getLogger() and set another level.
		But if you need more or less logging information already from device creation,
		then you have to change it here.
		*/
		ESGPLOG_LEVEL LoggingLevel;

		//! Specifies the logger class used in the logging interface.
		Logger*	plog;
};



#endif		// __SGP_OPENGLES2CREATIONPARAMETERS_HEADER__