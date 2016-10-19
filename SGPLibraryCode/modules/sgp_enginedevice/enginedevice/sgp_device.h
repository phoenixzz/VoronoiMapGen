#ifndef	__SGP_DEVICE_HEADER__
#define __SGP_DEVICE_HEADER__


class SGPDevice;
class ISGPRenderDevice;

//! Creates an SGP Engine device. The SGP device is the root object for using the engine.
/** If you need more parameters to be passed to the creation of the SGP Engine device,
use the createDeviceEx() function.
\param deviceType: Type of the device. This can currently be SGPDT_OPENGL, SGPDT_DIRECT3D11
\param windowWidth: Width of the window or the video mode in fullscreen mode.
\param windowHeight: Height of the window or the video mode in fullscreen mode.
\param bits: Bits per pixel in fullscreen mode. Ignored if windowed mode.
\param fullscreen: Should be set to true if the device should run in fullscreen. Otherwise
	the device runs in windowed mode.
\param stencilbuffer: Specifies if the stencil buffer should be enabled. Set this to true,
if you want the engine be able to draw stencil buffer. Note that not all
devices are able to use the stencil buffer.
\param vsync: Specifies vertical syncronisation: If set to true, the driver will wait
for the vertical retrace period, otherwise not.
\param receiver: A user created event receiver.
\return Returns pointer to the created SGPDevice or null if the
device could not be created.
*/
extern "C" SGP_API SGPDevice* SGP_CALLTYPE createDevice(
	SGP_DRIVER_TYPE deviceType = SGPDT_OPENGL,
	// parantheses are necessary for some compilers
	uint32 windowWidth = 800,
	uint32 windowHeight = 600,
	uint8 bits = 32,
	bool fullscreen = false,
	bool stencilbuffer = false,
	bool vsync = false,
	bool resLoadingMT = false,
	Logger*	plog = 0,
	ISGPEventReceiver* receiver = 0);

//! typedef for Function Pointer
typedef SGPDevice* (SGP_CALLTYPE *funcptr_createDevice )(
		SGP_DRIVER_TYPE deviceType,
		uint32 windowWidth,
		uint32 windowHeight,
		uint8 bits,
		bool fullscreen,
		bool stencilbuffer,
		bool vsync,
		bool resLoadingMT,
		Logger*	plog,
		ISGPEventReceiver* receiver);


//! Creates an SGP device with the option to specify advanced parameters.
/** Usually you should used createDevice() for creating an SGP Engine device.
Use this function only if you wish to specify advanced parameters like a window
handle in which the device should be created.
\param parameters: Structure containing advanced parameters for the creation of the device.
See SGPCreationParameters for details.
\return Returns pointer to the created SGPDevice or null if the
device could not be created. */
extern "C" SGP_API SGPDevice* SGP_CALLTYPE createDeviceEx(
	const SGPCreationParameters& parameters);

//! typedef for Function Pointer
typedef SGPDevice* (SGP_CALLTYPE *funcptr_createDeviceEx )( const SGPCreationParameters& parameters );



//! The SGP Engine device. You can create it with createDevice() or createDeviceEx().
/** This is the most important class of the Irrlicht Engine. You can
access everything in the engine if you have a pointer to an instance of
this class.  There should be only one instance of this class at any time.
*/
class SGPDevice
{
public:
	virtual ~SGPDevice() {}
	//! Runs the device.
	/** Returns false if device wants to be deleted. Use it in this way:

	while(device->run())
	{
		// draw everything here
	}

	If you want the device to do nothing if the window is inactive
	(recommended), use the slightly enhanced code shown at isWindowActive().

	Note if you are running SGP inside an external, custom
	created window: Calling Device->run() will cause SGP to
	dispatch windows messages internally.
	If you are running SGP Engine in your own custom window, you can
	also simply use your own message loop using GetMessage,
	DispatchMessage and whatever and simply don't use this method.
	But note that SGP will not be able to fetch user input
	then. See SGPCreationParameters::WindowId for more
	informations and example code.
	*/
	virtual bool run() = 0;

	//! Cause the device to temporarily pause execution and let other processes run.
	/** This should bring down processor usage without major
	performance loss for SGP */
	virtual void yield() = 0;

	//! Pause execution and let other processes to run for a specified amount of time.
	/** It may not wait the full given time, as sleep may be interrupted
	\param timeMs: Time to sleep for in milisecs.
	\param pauseTimer: If true, pauses the device timer while sleeping
	*/
	virtual void sleep(uint32 timeMs, bool pauseTimer=false) = 0;

	//! Provides access to the render device for drawing 3d and 2d geometry.
	/** \return Pointer the render device. */
	virtual ISGPRenderDevice* getRenderDevice() = 0;

	//! Provides access to the sound manager interface for playing sound.
	/** \return Pointer the sound manager. */
	virtual ISGPSoundManager* getSoundManager() = 0;

	//! Setting SoundManager
	virtual void setSoundManager(ISGPSoundManager* pSoundManager) = 0;

	//! Provides access to the message logger.
	/** \return Pointer to the logger. */
	virtual Logger* getLogger() = 0;

	//! Gets a list with all video modes available.
	/**
	\return Pointer to a list with all video modes supported
	by the gfx adapter. */
	virtual ISGPVideoModeList* getVideoModeList() = 0;

	//! Provides access to the engine's timer.
	/** The system time can be retrieved by it as
	well as the virtual time, which also can be manipulated.
	\return Pointer to the Timer object. */
	virtual ISGPTimer* getTimer() = 0;

	//! Provides access to the engine's currently set randomizer.
	/** \return Pointer to the Random object. */
	virtual Random* getRandomizer() const = 0;


	//! Sets the caption of the window.
	/** \param text: New text of the window caption. */
	virtual void setWindowCaption(const wchar_t* text) = 0;

	//! Returns if the window is active.
	/** If the window is inactive,
	nothing needs to be drawn. So if you don't want to draw anything
	when the window is inactive, create your drawing loop this way:

	while(device->run())
	{
		if (device->isWindowActive())
		{
			// draw everything here
		}
		else
			device->yield();
	}

	\return True if window is active. */
	virtual bool isWindowActive() const = 0;

	//! Checks if the SGP window has focus
	/** \return True if window has focus. */
	virtual bool isWindowFocused() const = 0;

	//! Checks if the SGP window is minimized
	/** \return True if window is minimized. */
	virtual bool isWindowMinimized() const = 0;

	//! Checks if the SGP window is running in fullscreen mode
	/** \return True if window is fullscreen. */
	virtual bool isFullscreen() const = 0;

	//! Notifies the device that it should close itself.
	/** SGPDevice::run() will always return false after closeDevice() was called. */
	virtual void closeDevice() = 0;


	//! Sets a new user event receiver which will receive events from the engine.
	/** Return true in ISGPEventReceiver::OnEvent to prevent the event from continuing along
	the chain of event receivers. The path that an event takes through the system depends
	on its type. See SGP_EVENT_TYPE for details.
	\param receiver New receiver to be used. */
	virtual void setEventReceiver(ISGPEventReceiver* receiver) = 0;

	//! Provides access to the current event receiver.
	/** \return Pointer to the current event receiver. Returns 0 if there is none. */
	virtual ISGPEventReceiver* getEventReceiver() = 0;

	//! Sends a user created event to the engine.
	/** Is is usually not necessary to use this. However, if you
	are using an own input library for example for doing joystick
	input, you can use this to post key or mouse input events to
	the engine. Internally, this method only delegates the events
	further to the scene manager and the GUI environment. */
	virtual bool postEventFromUser(const SSGPEvent& event) = 0;

	//! Sets if the window should be resizable in windowed mode.
	/** The default is false. This method only works in windowed
	mode.
	\param resize Flag whether the window should be resizable. */
	virtual void setResizable(bool resize=false) = 0;

	//! Minimizes the window if possible.
	virtual void minimizeWindow() = 0;

	//! Maximizes the window if possible.
	virtual void maximizeWindow() = 0;

	//! Restore the window to normal size if possible.
	virtual void restoreWindow() = 0;

	//! Activate any joysticks, and generate events for them.
	/** SGP contains support for joysticks, but does not generate joystick events by default,
	as this would consume joystick info that 3rd party libraries might rely on. Call this method to
	activate joystick support in SGP and to receive SSGPJoystickEvent events.
	\param joystickInfo On return, this will contain an array of each joystick that was found and activated.
	\return true if joysticks are supported on this device and SGP_WITH_JOYSTICK_EVENTS
			is defined, false if joysticks are not supported or support is compiled out.
	*/
	virtual bool activateJoysticks(Array<SSGPJoystickInfo>& joystickInfo) =0;

	//! Set the current Gamma Value for the Display
	virtual bool setGammaRamp(float red, float green, float blue,
				float relativebrightness, float relativecontrast) = 0;

	//! Get the current Gamma Value for the Display
	virtual bool getGammaRamp(float &red, float &green, float &blue,
				float &brightness, float &contrast) = 0;

	//! Remove messages pending in the system message loop
	/** This function is usually used after messages have been buffered for a longer time, for example
	when loading a large scene. Clearing the message loop prevents that mouse- or buttonclicks which users
	have pressed in the meantime will now trigger unexpected actions in the gui. <br>
	So far the following messages are cleared:<br>
	Win32: All keyboard and mouse messages<br>
	Linux: All keyboard and mouse messages<br>
	All other devices are not yet supported here.<br>
	The function is still somewhat experimental, as the kind of messages we clear is based on just a few use-cases.
	If you think further messages should be cleared, or some messages should not be cleared here, then please tell us. */
	virtual void clearSystemMessages() = 0;

	//! Get the type of the device.
	/** This allows the user to check which windowing system is currently being
	used. */
	virtual bool IsWin32Device() const = 0;
	virtual bool IsLinuxDevice() const = 0;
	virtual bool IsMacOSXDevice() const = 0;
	virtual bool IsIOSDevice() const = 0;
	virtual bool IsAndroidDevice() const = 0;

	//! Check if a driver type is supported by the engine.
	/** Even if true is returned the driver may not be available
	for a configuration requested when creating the device. */
	static bool isDriverSupported(SGP_DRIVER_TYPE driver)
	{
		switch (driver)
		{
			case SGPDT_DIRECT3D11:
				return false;

			case SGPDT_OPENGL:
				return true;

			default:
				return false;
		}
	}
};


#endif		// __SGP_DEVICE_HEADER__