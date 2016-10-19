#ifndef __SGP_CREATIONPARAMETERS_HEADER__
#define __SGP_CREATIONPARAMETERS_HEADER__

class ISGPEventReceiver;

struct SGPCreationParameters
{
		//! Constructs a SGPCreationParameters structure with default values.
		SGPCreationParameters() :
			DriverType(SGPDT_OPENGL),
			Bits(32),
			ZBufferBits(16),
			Fullscreen(false),
			Stencilbuffer(false),
			Vsync(false),
			AntiAlias(0),
			HandleSRGB(false),
			WithAlphaChannel(false),
			Doublebuffer(true),
			MultiThreadResLoading(false),
			EventReceiver(0),
			WindowId(0),
			plog(0),
#if SGP_DEBUG
			LoggingLevel(ELL_DEBUG)
#else
			LoggingLevel(ELL_WARNING)
#endif
		{
			WindowWidth = 800;
			WindowHeight = 600;
		}

		SGPCreationParameters(const SGPCreationParameters& other)
		{ *this = other; }

		SGPCreationParameters& operator=(const SGPCreationParameters& other)
		{
			DriverType = other.DriverType;
			WindowWidth = other.WindowWidth;
			WindowHeight = other.WindowHeight;
			Bits = other.Bits;
			ZBufferBits = other.ZBufferBits;
			Fullscreen = other.Fullscreen;
			Stencilbuffer = other.Stencilbuffer;
			Vsync = other.Vsync;
			AntiAlias = other.AntiAlias;
			HandleSRGB = other.HandleSRGB;
			WithAlphaChannel = other.WithAlphaChannel;
			Doublebuffer = other.Doublebuffer;
			MultiThreadResLoading = other.MultiThreadResLoading;
			EventReceiver = other.EventReceiver;
			WindowId = other.WindowId;
			LoggingLevel = other.LoggingLevel;
			plog = other.plog;
			return *this;
		}


		//! Type of video driver used to render graphics.
		/** This can currently be SGPDT_DIRECT3D11, and SGPDT_OPENGL.
		Default: SGPDT_OPENGL. */
		SGP_DRIVER_TYPE DriverType;

		//! Size of the window or the video mode in fullscreen mode. Default: 800x600
		uint32 WindowWidth;
		uint32 WindowHeight;

		//! Minimum Bits per pixel of the color buffer in fullscreen mode. Ignored if windowed mode. Default: 16.
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

		//! Whether the main framebuffer uses an alpha channel.
		/** In some situations it might be desireable to get a color
		buffer with an alpha channel, e.g. when rendering into a
		transparent window or overlay. If this flag is set the device
		tries to create a framebuffer with alpha channel.
		If this flag is set, only color buffers with alpha channel
		are considered. Otherwise, it depends on the actual hardware
		if the colorbuffer has an alpha channel or not.
		Default value: false */
		bool WithAlphaChannel;

		//! Whether the main framebuffer uses doublebuffering.
		/** This should be usually enabled, in order to avoid render
		artifacts on the visible framebuffer. However, it might be
		useful to use only one buffer on very small devices. If no
		doublebuffering is available, the drivers will fall back to
		single buffers. Default value: true */
		bool Doublebuffer;

		//! Whether the resource loading uses Multi-threading.
		/** resource usually include Model files and texture files from hard disk
		    In real-time game, this should be usually enabled, in order to avoid render
			stalling and keep smooth FPS.
			In other tools,  Default value: false
		*/
		bool MultiThreadResLoading;


		//! A user created event receiver.
		ISGPEventReceiver* EventReceiver;

		//! Window Id.
		/** If this is set to a value other than 0, the Irrlicht Engine
		will be created in an already existing window. For windows, set
		this to the HWND of the window you want. The windowSize and
		FullScreen options will be ignored when using the WindowId
		parameter. Default this is set to 0.
		To make Irrlicht run inside the custom window, you still will
		have to draw Irrlicht on your own. You can use this loop, as
		usual:
		\code
		while (device->run())
		{
			driver->beginScene(true, true, 0);
			smgr->drawAll();
			driver->endScene();
		}
		\endcode
		Instead of this, you can also simply use your own message loop
		using GetMessage, DispatchMessage and whatever. Calling
		IrrlichtDevice::run() will cause Irrlicht to dispatch messages
		internally too.  You need not call Device->run() if you want to
		do your own message dispatching loop, but Irrlicht will not be
		able to fetch user input then and you have to do it on your own
		using the window messages, DirectInput, or whatever. Also,
		you'll have to increment the Irrlicht timer.
		An alternative, own message dispatching loop without
		device->run() would look like this:
		\code
		MSG msg;
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					break;
			}

			// increase virtual timer time
			device->getTimer()->tick();

			// draw engine picture
			driver->beginScene(true, true, 0);
			smgr->drawAll();
			driver->endScene();
		}
		\endcode
		However, there is no need to draw the picture this often. Just
		do it how you like. */
		void* WindowId;

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



#endif		// __SGP_CREATIONPARAMETERS_HEADER__