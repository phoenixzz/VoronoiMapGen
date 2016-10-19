#ifndef	__SGP_DEVICEWIN32_HEADER__
#define __SGP_DEVICEWIN32_HEADER__


class SGPDeviceWin32;

struct SSGPJoystickWin32Control
{
	SGPDeviceWin32* Device;


#if defined(SGP_WITH_JOYSTICK_EVENTS)
	struct JoystickInfo
	{
		uint32 Index;
		JOYCAPS Caps;
	};
	Array<JoystickInfo> ActiveJoysticks;
#endif

	SSGPJoystickWin32Control(SGPDeviceWin32* dev) : Device(dev)
	{
	}

	~SSGPJoystickWin32Control()
	{
	}


	void pollJoysticks()
	{
	#if defined SGP_WITH_JOYSTICK_EVENTS

		if (0 == ActiveJoysticks.size())
			return;

		uint32 joystick;
		JOYINFOEX info;

		for(joystick = 0; joystick < ActiveJoysticks.size(); ++joystick)
		{
			// needs to be reset for each joystick
			// request ALL values and POV as continuous if possible
			info.dwSize = sizeof(info);
			info.dwFlags = JOY_RETURNALL|JOY_RETURNPOVCTS;
			const JOYCAPS & caps = ActiveJoysticks[joystick].Caps;
			// if no POV is available don't ask for POV values
			if (!(caps.wCaps & JOYCAPS_HASPOV))
				info.dwFlags &= ~(JOY_RETURNPOV|JOY_RETURNPOVCTS);
			if(JOYERR_NOERROR == joyGetPosEx(ActiveJoysticks[joystick].Index, &info))
			{
				SSGPEvent event;

				event.EventType = SGPET_JOYSTICK_INPUT_EVENT;
				event.JoystickEvent.Joystick = (uint8)joystick;

				event.JoystickEvent.POV = (uint16)info.dwPOV;
				// set to undefined if no POV value was returned or the value
				// is out of range
				if (!(info.dwFlags & JOY_RETURNPOV) || (event.JoystickEvent.POV > 35900))
					event.JoystickEvent.POV = 65535;

				for(int axis = 0; axis < SSGPEvent::SSGPJoystickEvent::NUMBER_OF_AXES; ++axis)
					event.JoystickEvent.Axis[axis] = 0;

				event.JoystickEvent.ButtonStates = info.dwButtons;

				switch(caps.wNumAxes)
				{
				default:
				case 6:
					event.JoystickEvent.Axis[SSGPEvent::SSGPJoystickEvent::AXIS_V] =
						(int16)((65535 * (info.dwVpos - caps.wVmin)) / (caps.wVmax - caps.wVmin) - 32768);

				case 5:
					event.JoystickEvent.Axis[SSGPEvent::SSGPJoystickEvent::AXIS_U] =
						(int16)((65535 * (info.dwUpos - caps.wUmin)) / (caps.wUmax - caps.wUmin) - 32768);

				case 4:
					event.JoystickEvent.Axis[SSGPEvent::SSGPJoystickEvent::AXIS_R] =
						(int16)((65535 * (info.dwRpos - caps.wRmin)) / (caps.wRmax - caps.wRmin) - 32768);

				case 3:
					event.JoystickEvent.Axis[SSGPEvent::SSGPJoystickEvent::AXIS_Z] =
						(int16)((65535 * (info.dwZpos - caps.wZmin)) / (caps.wZmax - caps.wZmin) - 32768);

				case 2:
					event.JoystickEvent.Axis[SSGPEvent::SSGPJoystickEvent::AXIS_Y] =
						(int16)((65535 * (info.dwYpos - caps.wYmin)) / (caps.wYmax - caps.wYmin) - 32768);

				case 1:
					event.JoystickEvent.Axis[SSGPEvent::SSGPJoystickEvent::AXIS_X] =
						(int16)((65535 * (info.dwXpos - caps.wXmin)) / (caps.wXmax - caps.wXmin) - 32768);
				}

				(void)Device->postEventFromUser(event);
			}
		}

	#endif // SGP_WITH_JOYSTICK_EVENTS
	}

	bool activateJoysticks(Array<SSGPJoystickInfo> & joystickInfo)
	{
	#if defined SGP_WITH_JOYSTICK_EVENTS

		joystickInfo.clear();
		ActiveJoysticks.clear();

		const uint32 numberOfJoysticks = ::joyGetNumDevs();
		JOYINFOEX info;
		info.dwSize = sizeof(info);
		info.dwFlags = JOY_RETURNALL;

		JoystickInfo activeJoystick;
		SSGPJoystickInfo returnInfo;

		joystickInfo.resize(numberOfJoysticks);
		ActiveJoysticks.resize(numberOfJoysticks);

		uint32 joystick = 0;
		for(; joystick < numberOfJoysticks; ++joystick)
		{
			if(	JOYERR_NOERROR == joyGetPosEx(joystick, &info) &&
				JOYERR_NOERROR == joyGetDevCaps(joystick,
											&activeJoystick.Caps,
											sizeof(activeJoystick.Caps)))
			{
				activeJoystick.Index = joystick;
				ActiveJoysticks.add(activeJoystick);

				returnInfo.Joystick = (uint8)joystick;
				returnInfo.Axes = activeJoystick.Caps.wNumAxes;
				returnInfo.Buttons = activeJoystick.Caps.wNumButtons;
				returnInfo.Name = activeJoystick.Caps.szPname;
				returnInfo.PovHat = ((activeJoystick.Caps.wCaps & JOYCAPS_HASPOV) == JOYCAPS_HASPOV)
									? SSGPJoystickInfo::POV_HAT_PRESENT : SSGPJoystickInfo::POV_HAT_ABSENT;

				joystickInfo.add(returnInfo);
			}
		}

		for(joystick = 0; joystick < joystickInfo.size(); ++joystick)
		{
			char logString[256];
			sprintf(logString, "Found joystick %d, %d axes, %d buttons '%s'",
				joystick, joystickInfo[joystick].Axes,
				joystickInfo[joystick].Buttons, joystickInfo[joystick].Name);
			Logger::getCurrentLogger()->writeToLog(String(logString), ELL_INFORMATION);
		}

		return true;
	#else
		return false;
	#endif // SGP_WITH_JOYSTICK_EVENTS
	}
};


class SGPDeviceWin32 : public SGPDevice
{
public:

	//! constructor
	SGPDeviceWin32(const SGPCreationParameters& params);
	//! destructor
	virtual ~SGPDeviceWin32();

	//! runs the device. Returns false if device wants to be deleted
	virtual bool run();

	//! Cause the device to temporarily pause execution and let other processes to run
	// This should bring down processor usage without major performance loss for Irrlicht
	virtual void yield();

	//! Pause execution and let other processes to run for a specified amount of time.
	virtual void sleep(uint32 timeMs, bool pauseTimer);

	//! returns the render device
	virtual ISGPRenderDevice* getRenderDevice();

	//! returns the sound manager
	virtual ISGPSoundManager* getSoundManager();

	//! Setting SoundManager
	virtual void setSoundManager(ISGPSoundManager* pSoundManager);

	//! Returns a pointer to the logger.
	virtual Logger* getLogger();

	//! Returns a pointer to a list with all video modes supported by the gfx adapter.
	virtual ISGPVideoModeList* getVideoModeList();

	//! Returns a pointer to the ISGPTimer object. With it the current Time can be received.
	virtual ISGPTimer* getTimer();

	//! Provides access to the engine's currently set randomizer.
	virtual Random* getRandomizer() const;


	//! sets the caption of the window
	virtual void setWindowCaption(const wchar_t* text);

	//! returns if window is active. if not, nothing need to be drawn
	virtual bool isWindowActive() const;

	//! returns if window has focus
	virtual bool isWindowFocused() const;

	//! returns if window is minimized
	virtual bool isWindowMinimized() const;

	//! Checks if the window is running in fullscreen mode.
	virtual bool isFullscreen() const;

	//! notifies the device that it should close itself
	virtual void closeDevice();

	//! Sets a new event receiver to receive events
	virtual void setEventReceiver(ISGPEventReceiver* receiver);

	//! Returns pointer to the current event receiver. Returns 0 if there is none.
	virtual ISGPEventReceiver* getEventReceiver();

	//! send the event to the right receiver
	virtual bool postEventFromUser(const SSGPEvent& event);

	//! Sets if the window should be resizable in windowed mode.
	virtual void setResizable(bool resize=false);

	//! Minimizes the window.
	virtual void minimizeWindow();

	//! Maximizes the window.
	virtual void maximizeWindow();

	//! Restores the window size.
	virtual void restoreWindow();

	//! Activate any joysticks, and generate events for them.
	virtual bool activateJoysticks(Array<SSGPJoystickInfo> & joystickInfo);

	//! Set the current Gamma Value for the Display
	virtual bool setGammaRamp( float red, float green, float blue, float brightness, float contrast );

	//! Get the current Gamma Value for the Display
	virtual bool getGammaRamp( float &red, float &green, float &blue, float &brightness, float &contrast );

	//! Remove all messages pending in the system message loop
	virtual void clearSystemMessages();

	virtual bool IsWin32Device() const {return true;}
	virtual bool IsLinuxDevice() const {return false;}
	virtual bool IsMacOSXDevice() const {return false;}
	virtual bool IsIOSDevice() const {return false;}
	virtual bool IsAndroidDevice() const {return false;}

public:
	//! switchs to fullscreen
	bool switchToFullScreen(bool reset = false);
	//! Compares to the last call of this function to return double and triple clicks.
	//! \return Returns only 1,2 or 3. A 4th click will start with 1 again.
	uint32 checkSuccessiveClicks(int32 mouseX, int32 mouseY, SGP_MOUSE_INPUT_EVENT inputEvent );

	void calculateGammaRamp ( uint16 *ramp, float gamma, float relativebrightness, float relativecontrast );
	void calculateGammaFromRamp ( float &gamma, const uint16 *ramp );

	//! Notifies the device, that it has been resized
	void OnResized();
	
	// Gets the window handle.
	HWND getWindowHandle() { return HWnd; }

private:
	//! create the driver
	ISGPRenderDevice* createDriver();

	//! Process system events
	void handleSystemMessages();

	void resizeIfNecessary();

private:
	struct SMouseMultiClicks
	{
		SMouseMultiClicks()
			: DoubleClickTime(500.0), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(SGPMIE_COUNT)
		{}

		double DoubleClickTime;
		uint32 CountSuccessiveClicks;
		double LastClickTime;
		int32 LastClick_X, LastClick_Y;
		SGP_MOUSE_INPUT_EVENT LastMouseInputEvent;
	};

	SMouseMultiClicks			m_MouseMultiClicks;
	SGPCreationParameters		CreationParams;
	CVideoModeList				m_VideoModeList;

	ScopedPointer<ISGPRenderDevice>			m_pRenderDevice;
	ScopedPointer<SSGPJoystickWin32Control>	m_pJoyControl;
	ScopedPointer<ISGPTimer>				m_pTimer;
	ScopedPointer<Random>					m_pRandomizer;
	ScopedPointer<ISGPSoundManager>			m_pSoundManager;

	Logger*						m_pLogger;
	ISGPEventReceiver*			m_UserEventReceiver;


	HWND	HWnd;
	DEVMODE DesktopMode;

	bool	ChangedToFullScreen;
	bool	Resized;
	bool	ExternalWindow;
	bool	bClosing;
};

#endif		// __SGP_DEVICEWIN32_HEADER__