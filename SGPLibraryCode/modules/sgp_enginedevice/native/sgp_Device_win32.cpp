
sgp::SGPDeviceWin32* g_SGPDeviceWin32 = NULL;
HKL KEYBOARD_INPUT_HKL = 0;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
	#define WHEEL_DELTA 120
#endif


	sgp::SSGPEvent event;

	static sgp::int32 ClickCount=0;
	if (GetCapture() != hWnd && ClickCount > 0)
		ClickCount = 0;


	struct messageMap
	{
		sgp::int32 group;
		UINT winMessage;
		sgp::int32 sgpMessage;
	};

	static messageMap mouseMap[] =
	{
		{0, WM_LBUTTONDOWN, sgp::SGPMIE_LMOUSE_PRESSED_DOWN},
		{1, WM_LBUTTONUP,   sgp::SGPMIE_LMOUSE_LEFT_UP},
		{0, WM_RBUTTONDOWN, sgp::SGPMIE_RMOUSE_PRESSED_DOWN},
		{1, WM_RBUTTONUP,   sgp::SGPMIE_RMOUSE_LEFT_UP},
		{0, WM_MBUTTONDOWN, sgp::SGPMIE_MMOUSE_PRESSED_DOWN},
		{1, WM_MBUTTONUP,   sgp::SGPMIE_MMOUSE_LEFT_UP},
		{2, WM_MOUSEMOVE,   sgp::SGPMIE_MOUSE_MOVED},
		{3, WM_MOUSEWHEEL,  sgp::SGPMIE_MOUSE_WHEEL},
		{-1, 0, 0}
	};

	// handle grouped events
	messageMap * m = mouseMap;
	while ( m->group >=0 && m->winMessage != message )
		m += 1;

	if ( m->group >= 0 )
	{
		if ( m->group == 0 )	// down
		{
			ClickCount++;
			SetCapture(hWnd);
		}
		else
		if ( m->group == 1 )	// up
		{
			ClickCount--;
			if (ClickCount<1)
			{
				ClickCount=0;
				ReleaseCapture();
			}
		}

		event.EventType = sgp::SGPET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = (sgp::SGP_MOUSE_INPUT_EVENT) m->sgpMessage;
		event.MouseInput.X = LOWORD(lParam);
		event.MouseInput.Y = HIWORD(lParam);
		event.MouseInput.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
		event.MouseInput.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
		// left and right mouse buttons
		event.MouseInput.ButtonStates = wParam & ( MK_LBUTTON | MK_RBUTTON);
		// middle and extra buttons
		if (wParam & MK_MBUTTON)
			event.MouseInput.ButtonStates |= sgp::SGPMBSM_MIDDLE;
#if(_WIN32_WINNT >= 0x0500)
		if (wParam & MK_XBUTTON1)
			event.MouseInput.ButtonStates |= sgp::SGPMBSM_EXTRA1;
		if (wParam & MK_XBUTTON2)
			event.MouseInput.ButtonStates |= sgp::SGPMBSM_EXTRA2;
#endif
		event.MouseInput.Wheel = 0.f;

		// wheel
		if ( m->group == 3 )
		{
			POINT p; // fixed by jox
			p.x = 0; p.y = 0;
			ClientToScreen(hWnd, &p);
			event.MouseInput.X -= p.x;
			event.MouseInput.Y -= p.y;
			event.MouseInput.Wheel = ((float)((short)HIWORD(wParam))) / (float)WHEEL_DELTA;
		}

		if (g_SGPDeviceWin32)
		{
			g_SGPDeviceWin32->postEventFromUser(event);

			if ( event.MouseInput.Event >= sgp::SGPMIE_LMOUSE_PRESSED_DOWN && event.MouseInput.Event <= sgp::SGPMIE_MMOUSE_PRESSED_DOWN )
			{
				sgp::uint32 clicks = g_SGPDeviceWin32->checkSuccessiveClicks(event.MouseInput.X, event.MouseInput.Y, event.MouseInput.Event);
				if ( clicks == 2 )
				{
					event.MouseInput.Event = (sgp::SGP_MOUSE_INPUT_EVENT)(sgp::SGPMIE_LMOUSE_DOUBLE_CLICK + event.MouseInput.Event-sgp::SGPMIE_LMOUSE_PRESSED_DOWN);
					g_SGPDeviceWin32->postEventFromUser(event);
				}
				else if ( clicks == 3 )
				{
					event.MouseInput.Event = (sgp::SGP_MOUSE_INPUT_EVENT)(sgp::SGPMIE_LMOUSE_TRIPLE_CLICK + event.MouseInput.Event-sgp::SGPMIE_LMOUSE_PRESSED_DOWN);
					g_SGPDeviceWin32->postEventFromUser(event);
				}
			}
		}
		return 0;
	}

	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			BYTE allKeys[256];

			event.EventType = sgp::SGPET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (sgp::SGP_KEY_CODE)wParam;
			event.KeyInput.PressedDown = (message==WM_KEYDOWN || message == WM_SYSKEYDOWN);

			const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
			if ( event.KeyInput.Key == sgp::KEY_SHIFT )
			{
				// this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
				event.KeyInput.Key = (sgp::SGP_KEY_CODE)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
			}
			if ( event.KeyInput.Key == sgp::KEY_CONTROL )
			{
				event.KeyInput.Key = (sgp::SGP_KEY_CODE)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
				// some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
				if (lParam & 0x1000000)
					event.KeyInput.Key = sgp::KEY_RCONTROL;
			}
			if ( event.KeyInput.Key == sgp::KEY_MENU )
			{
				event.KeyInput.Key = (sgp::SGP_KEY_CODE)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
				if (lParam & 0x1000000)
					event.KeyInput.Key = sgp::KEY_RMENU;
			}

			GetKeyboardState(allKeys);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);

			// Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
			// Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
			WORD keyChars[2];
			UINT scanCode = HIWORD(lParam);
			int conversionResult = ToAsciiEx(wParam,scanCode,allKeys,keyChars,0,KEYBOARD_INPUT_HKL);
			if (conversionResult == 1)
			{
				sgp::String unicodeChar(sgp::CharPointer_UTF8((const char*) keyChars));				
				event.KeyInput.Char = static_cast<wchar_t>(unicodeChar[0]);
			}
			else
				event.KeyInput.Char = 0;

			// allow composing characters like '@' with Alt Gr on non-US keyboards
			if ((allKeys[VK_MENU] & 0x80) != 0)
				event.KeyInput.Control = 0;


			if (g_SGPDeviceWin32)
				g_SGPDeviceWin32->postEventFromUser(event);

			if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
				return DefWindowProc(hWnd, message, wParam, lParam);
			else
				return 0;
		}

	case WM_SIZE:
		{
			// resize
			if (g_SGPDeviceWin32)
				g_SGPDeviceWin32->OnResized();

			event.EventType = sgp::SGPET_GUI_EVENT;
			event.UserEvent.UserData1 = sgp::SGPGE_RESIZE;

			if (g_SGPDeviceWin32)
				g_SGPDeviceWin32->postEventFromUser(event);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER ||
			(wParam & 0xFFF0) == SC_KEYMENU
			)
			return 0;

		break;

	case WM_ACTIVATE:
		// we need to take care for screen changes, e.g. Alt-Tab
		if (g_SGPDeviceWin32 && g_SGPDeviceWin32->isFullscreen())
		{
			if ((wParam&0xFF)==WA_INACTIVE)
			{
				// If losing focus we minimize the app to show other one
				ShowWindow(hWnd,SW_MINIMIZE);
				// and switch back to default resolution
				g_SGPDeviceWin32->switchToFullScreen(true);
			}
			else
			{
				// Otherwise we retore the fullscreen Irrlicht app
				SetForegroundWindow(hWnd);
				ShowWindow(hWnd, SW_RESTORE);
				// and set the fullscreen resolution again
				g_SGPDeviceWin32->switchToFullScreen();
			}
		}
		break;

	case WM_USER:
		event.EventType = sgp::SGPET_USER_EVENT;
		event.UserEvent.UserData1 = (sgp::int32)wParam;
		event.UserEvent.UserData2 = (sgp::int32)lParam;


		if (g_SGPDeviceWin32)
			g_SGPDeviceWin32->postEventFromUser(event);

		return 0;

	case WM_SETCURSOR:
		break;

	case WM_INPUTLANGCHANGE:
		// get the new codepage used for keyboard input
		KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


namespace sgp
{

COpenGLRenderDevice* createOpenGLDriver(const SGPCreationParameters& params, SGPDeviceWin32* device);


//! constructor
SGPDeviceWin32::SGPDeviceWin32(const SGPCreationParameters& params)
	: HWnd(0), ChangedToFullScreen(false), Resized(false), ExternalWindow(false), bClosing(false),
	CreationParams(params),	m_UserEventReceiver(params.EventReceiver), m_pLogger(params.plog),
	m_pRenderDevice(nullptr), m_pJoyControl(nullptr), m_pSoundManager(nullptr), 
	m_pTimer(nullptr), m_pRandomizer(nullptr)
{
	// get handle to exe file
	HINSTANCE hInstance = GetModuleHandle(0);

	// Store original desktop mode.
	memset(&DesktopMode, 0, sizeof(DesktopMode));
	DesktopMode.dmSize = sizeof(DesktopMode);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DesktopMode);

	// create the window if we need to and we do not use the null device
	if( !params.WindowId )
	{
		const wchar_t* ClassName = TEXT("SGPDeviceWin32");

		// Register Class
		WNDCLASSEX wcex;
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);;
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= ClassName;
		wcex.hIconSm		= 0;

		RegisterClassEx(&wcex);

		// calculate client size
		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = params.WindowWidth;
		clientSize.bottom = params.WindowHeight;

		DWORD style = WS_POPUP;

		if (!params.Fullscreen)
			style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		AdjustWindowRect(&clientSize, style, FALSE);

		const int32 realWidth = clientSize.right - clientSize.left;
		const int32 realHeight = clientSize.bottom - clientSize.top;

		int32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		int32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if ( windowLeft < 0 )
			windowLeft = 0;
		if ( windowTop < 0 )
			windowTop = 0;	// make sure window menus are in screen on creation

		if (params.Fullscreen)
		{
			windowLeft = 0;
			windowTop = 0;
		}

		// create window

		HWnd = CreateWindow( ClassName, TEXT(""), style, windowLeft, windowTop,
					realWidth, realHeight, NULL, NULL, hInstance, NULL);
		CreationParams.WindowId = HWnd;


		ShowWindow(HWnd, SW_SHOWNORMAL);
		UpdateWindow(HWnd);

		// fix ugly ATI driver bugs. Thanks to ariaci
		MoveWindow(HWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

		// make sure everything gets updated to the real sizes
		Resized = true;
	}
	else if (params.WindowId)
	{
		// attach external window
		HWnd = static_cast<HWND>(params.WindowId);
		RECT r;
		GetWindowRect(HWnd, &r);
		CreationParams.WindowWidth = r.right - r.left;
		CreationParams.WindowHeight = r.bottom - r.top;
		CreationParams.Fullscreen = false;
		ExternalWindow = true;
	}

	m_pJoyControl = new SSGPJoystickWin32Control(this);
	m_pTimer = new CSGPEngineTimer();
	m_pTimer->setTime(0);
	m_pRandomizer = new Random();


	// initialize doubleclicks with system values
	m_MouseMultiClicks.DoubleClickTime = GetDoubleClickTime();

	// create driver
	m_pRenderDevice = createDriver();
	
	g_SGPDeviceWin32 = this;

	// set this as active window
	if (!ExternalWindow)
	{
		SetActiveWindow(HWnd);
		SetForegroundWindow(HWnd);
	}

	KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);

	// inform driver about the window size etc.
	resizeIfNecessary();
}

//! destructor
SGPDeviceWin32::~SGPDeviceWin32()
{
	m_pRenderDevice = nullptr;
	m_pJoyControl = nullptr;
	m_pTimer = nullptr;
	m_pRandomizer = nullptr;
	m_pSoundManager = nullptr;

	g_SGPDeviceWin32 = NULL;

	switchToFullScreen(true);
}

//! returns the render device
ISGPRenderDevice* SGPDeviceWin32::getRenderDevice()
{
	return m_pRenderDevice;
}

//! returns the SoundManager
ISGPSoundManager* SGPDeviceWin32::getSoundManager()
{
	return m_pSoundManager;
}

//! Setting SoundManager
void SGPDeviceWin32::setSoundManager(ISGPSoundManager* pSoundManager)
{
	m_pSoundManager = pSoundManager;
}

//! Returns a pointer to the logger.
Logger* SGPDeviceWin32::getLogger()
{
	return m_pLogger;
}

//! Returns a pointer to the Time object. With it the current Time can be received.
ISGPTimer* SGPDeviceWin32::getTimer()
{
	return m_pTimer;
}

//! Provides access to the engine's currently set randomizer.
Random* SGPDeviceWin32::getRandomizer() const
{
	return m_pRandomizer;
}

//! Checks if the window is running in fullscreen mode
bool SGPDeviceWin32::isFullscreen() const
{
	return CreationParams.Fullscreen;
}

//! create the driver
ISGPRenderDevice* SGPDeviceWin32::createDriver()
{
	ISGPRenderDevice *pDEV = NULL;
	switch(CreationParams.DriverType)
	{
	case SGPDT_DIRECT3D11:
		m_pLogger->writeToLog(String("Unable to create DIRECT3D11 driver."), ELL_ERROR);
		break;
	case SGPDT_OPENGL:
		switchToFullScreen();

		pDEV = createOpenGLDriver(CreationParams, this);
		if (!pDEV)
		{
			m_pLogger->writeToLog(String("Could not create OpenGL driver."), ELL_ERROR);
		}
		break;
	default:
		m_pLogger->writeToLog(String("Unable to create video driver of unknown type."), ELL_ERROR);
		break;
	}

	return pDEV;
}

//! notifies the device that it should close itself
void SGPDeviceWin32::closeDevice()
{
	MSG msg;
	PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	PostQuitMessage(0);
	PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	if (!ExternalWindow)
	{
		DestroyWindow(HWnd);
		const wchar_t* ClassName = TEXT("SGPDeviceWin32");
		HINSTANCE hInstance = GetModuleHandle(0);
		UnregisterClass(ClassName, hInstance);
	}
	bClosing = true;
}

//! runs the device. Returns false if device wants to be deleted
bool SGPDeviceWin32::run()
{
	m_pTimer->tick();

	handleSystemMessages();

	if(!bClosing)
		resizeIfNecessary();

	if(!bClosing && m_pJoyControl)
		m_pJoyControl->pollJoysticks();


	return !bClosing;
}


//! Pause the current process for the minimum time allowed only to allow other processes to execute
void SGPDeviceWin32::yield()
{
	Sleep(1);
}

//! Pause execution and let other processes to run for a specified amount of time.
void SGPDeviceWin32::sleep(uint32 timeMs, bool)
{
	Sleep(timeMs);
}


void SGPDeviceWin32::resizeIfNecessary()
{
	if (!Resized || !getRenderDevice())
		return;

	RECT r;
	GetClientRect(HWnd, &r);

	if (r.right < 2 || r.bottom < 2)
	{
	}
	else
	{
		String tempStr("Resizing window : ");
		tempStr += r.right; tempStr += String(" X "); tempStr += r.bottom;
		m_pLogger->writeToLog(tempStr);

		getRenderDevice()->onResize( (uint32)r.right, (uint32)r.bottom );
	}

	Resized = false;
}

//! sets the caption of the window
void SGPDeviceWin32::setWindowCaption(const wchar_t* text)
{
	// We use SendMessage instead of SetText to ensure proper
	// function even in cases where the HWND was created in a different thread
	DWORD_PTR dwResult;
	SendMessageTimeoutW(HWnd, WM_SETTEXT, 0,
			reinterpret_cast<LPARAM>(text),
			SMTO_ABORTIFHUNG, 2000, &dwResult);
}

//! returns if window is active. if not, nothing needs to be drawn
bool SGPDeviceWin32::isWindowActive() const
{
	return (GetActiveWindow() == HWnd);
}

//! returns if window has focus
bool SGPDeviceWin32::isWindowFocused() const
{
	bool ret = (GetFocus() == HWnd);
	return ret;
}


//! returns if window is minimized
bool SGPDeviceWin32::isWindowMinimized() const
{
	WINDOWPLACEMENT plc;
	plc.length=sizeof(WINDOWPLACEMENT);
	bool ret=false;
	if (GetWindowPlacement(HWnd,&plc))
		ret=(plc.showCmd & SW_SHOWMINIMIZED)!=0;

	return ret;
}

//! switches to fullscreen
bool SGPDeviceWin32::switchToFullScreen(bool reset)
{
	if (!CreationParams.Fullscreen)
		return true;

	if (reset)
	{
		if (ChangedToFullScreen)
		{
			return (ChangeDisplaySettings(&DesktopMode,0)==DISP_CHANGE_SUCCESSFUL);
		}
		else
			return true;
	}

	// use default values from current setting

	DEVMODE dm;
	memset(&dm, 0, sizeof(dm));
	dm.dmSize = sizeof(dm);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	dm.dmPelsWidth = CreationParams.WindowWidth;
	dm.dmPelsHeight = CreationParams.WindowHeight;
	dm.dmBitsPerPel = CreationParams.Bits;
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	LONG res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	if (res != DISP_CHANGE_SUCCESSFUL)
	{ // try again without forcing display frequency
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}

	bool ret = false;
	switch(res)
	{
	case DISP_CHANGE_SUCCESSFUL:
		ChangedToFullScreen = true;
		ret = true;
		break;
	case DISP_CHANGE_RESTART:
		m_pLogger->writeToLog(String("Switch to fullscreen: The computer must be restarted in order for the graphics mode to work."), ELL_ERROR);
		break;
	case DISP_CHANGE_BADFLAGS:
		m_pLogger->writeToLog(String("Switch to fullscreen: An invalid set of flags was passed in."), ELL_ERROR);
		break;
	case DISP_CHANGE_BADPARAM:
		m_pLogger->writeToLog(String("Switch to fullscreen: An invalid parameter was passed in. This can include an invalid flag or combination of flags."), ELL_ERROR);
		break;
	case DISP_CHANGE_FAILED:
		m_pLogger->writeToLog(String("Switch to fullscreen: The display driver failed the specified graphics mode."), ELL_ERROR);
		break;
	case DISP_CHANGE_BADMODE:
		m_pLogger->writeToLog(String("Switch to fullscreen: The graphics mode is not supported."), ELL_ERROR);
		break;
	default:
		m_pLogger->writeToLog(String("An unknown error occured while changing to fullscreen."), ELL_ERROR);
		break;
	}
	
	return ret;
}

//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
ISGPVideoModeList* SGPDeviceWin32::getVideoModeList()
{
	if (!m_VideoModeList.getVideoModeCount())
	{
		// enumerate video modes.
		DWORD i=0;
		DEVMODE mode;
		memset(&mode, 0, sizeof(mode));
		mode.dmSize = sizeof(mode);

		while (EnumDisplaySettings(NULL, i, &mode))
		{
			m_VideoModeList.addMode(mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel);
			++i;
		}

		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode))
			m_VideoModeList.setDesktop(mode.dmBitsPerPel, mode.dmPelsWidth, mode.dmPelsHeight);
	}

	return &m_VideoModeList;
}

//! Notifies the device, that it has been resized
void SGPDeviceWin32::OnResized()
{
	Resized = true;
}

//! Sets if the window should be resizable in windowed mode.
void SGPDeviceWin32::setResizable(bool resize)
{
	if (ExternalWindow || !getRenderDevice() || CreationParams.Fullscreen)
		return;

	LONG_PTR style = WS_POPUP;

	if (!resize)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	else
		style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if (!SetWindowLongPtr(HWnd, GWL_STYLE, style))
		m_pLogger->writeToLog(String("Could not change window style."));

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = getRenderDevice()->getScreenSize().Width;
	clientSize.bottom = getRenderDevice()->getScreenSize().Height;

	AdjustWindowRect(&clientSize, style, FALSE);

	const int32 realWidth = clientSize.right - clientSize.left;
	const int32 realHeight = clientSize.bottom - clientSize.top;

	const int32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	const int32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	SetWindowPos(HWnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight,
		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);
}


//! Minimizes the window.
void SGPDeviceWin32::minimizeWindow()
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(HWnd, &wndpl);
	wndpl.showCmd = SW_SHOWMINNOACTIVE;
	SetWindowPlacement(HWnd, &wndpl);
}


//! Maximizes the window.
void SGPDeviceWin32::maximizeWindow()
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(HWnd, &wndpl);
	wndpl.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement(HWnd, &wndpl);
}


//! Restores the window to its original size.
void SGPDeviceWin32::restoreWindow()
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(HWnd, &wndpl);
	wndpl.showCmd = SW_SHOWNORMAL;
	SetWindowPlacement(HWnd, &wndpl);
}

bool SGPDeviceWin32::activateJoysticks(Array<SSGPJoystickInfo>& joystickInfo)
{
	if (m_pJoyControl)
		return m_pJoyControl->activateJoysticks(joystickInfo);
	else
		return false;
}

void SGPDeviceWin32::calculateGammaRamp ( uint16 *ramp, float gamma, float relativebrightness, float relativecontrast )
{
	int32 i;
	int32 value;
	int32 rbright = (int32) ( relativebrightness * (65535.0f / 4 ) );
	float rcontrast = 1.0f / (255.0f - ( relativecontrast * 127.5f ) );

	gamma = gamma > 0.f ? 1.0f / gamma : 0.f;

	for ( i = 0; i < 256; ++i )
	{
		value = (int32)(pow( rcontrast * i, gamma)*65535.0f + 0.5f );
		ramp[i] = (uint16) jlimit( 0, 65535, value + rbright );
	}

}
void SGPDeviceWin32::calculateGammaFromRamp ( float &gamma, const uint16 *ramp )
{
	/* The following is adapted from a post by Garrett Bass on OpenGL
	Gamedev list, March 4, 2000.
	*/
	float sum = 0.0f;
	int32 i, count = 0;

	gamma = 1.0;
	for ( i = 1; i < 256; ++i ) {
		if ( (ramp[i] != 0) && (ramp[i] != 65535) ) {
			float B = (float)i / 256.f;
			float A = ramp[i] / 65535.f;
			sum += (float) ( logf(A) / logf(B) );
			count++;
		}
	}
	if ( count && sum ) {
		gamma = 1.0f / (sum / count);
	}

}

//! Set the current Gamma Value for the Display
bool SGPDeviceWin32::setGammaRamp( float red, float green, float blue, float brightness, float contrast )
{
	bool r;
	uint16 ramp[3][256];

	calculateGammaRamp( ramp[0], red, brightness, contrast );
	calculateGammaRamp( ramp[1], green, brightness, contrast );
	calculateGammaRamp( ramp[2], blue, brightness, contrast );

	HDC dc = GetDC(0);
	r = (SetDeviceGammaRamp ( dc, ramp ) != FALSE);
	ReleaseDC(HWnd, dc);
	return r;
}

//! Get the current Gamma Value for the Display
bool SGPDeviceWin32::getGammaRamp( float &red, float &green, float &blue, float &brightness, float &contrast )
{
	bool r;
	uint16 ramp[3][256];

	HDC dc = GetDC(0);
	r = (GetDeviceGammaRamp ( dc, ramp ) != FALSE);
	ReleaseDC(HWnd, dc);

	if ( r )
	{
		calculateGammaFromRamp(red, ramp[0]);
		calculateGammaFromRamp(green, ramp[1]);
		calculateGammaFromRamp(blue, ramp[2]);
	}

	brightness = 0.0f;
	contrast = 0.0f;

	return r;

}

//! Process system events
void SGPDeviceWin32::handleSystemMessages()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// No message translation because we don't use WM_CHAR and it would conflict with our
		// deadkey handling.

		if (ExternalWindow && msg.hwnd == HWnd)
			WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
		else
			DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			bClosing = true;
	}
}

//! Remove all messages pending in the system message loop
void SGPDeviceWin32::clearSystemMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
	{}
	while (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
	{}
}

//! Compares to the last call of this function to return double and triple clicks.
uint32 SGPDeviceWin32::checkSuccessiveClicks(int32 mouseX, int32 mouseY, SGP_MOUSE_INPUT_EVENT inputEvent )
{
	const int32 MAX_MOUSEMOVE = 3;

	double clickTime = getTimer()->getRealTime();

	if ( (clickTime-m_MouseMultiClicks.LastClickTime) < m_MouseMultiClicks.DoubleClickTime &&
		abs(m_MouseMultiClicks.LastClick_X - mouseX ) <= MAX_MOUSEMOVE &&
		abs(m_MouseMultiClicks.LastClick_Y - mouseY ) <= MAX_MOUSEMOVE &&
		m_MouseMultiClicks.CountSuccessiveClicks < 3 &&
		m_MouseMultiClicks.LastMouseInputEvent == inputEvent )
	{
		++m_MouseMultiClicks.CountSuccessiveClicks;
	}
	else
	{
		m_MouseMultiClicks.CountSuccessiveClicks = 1;
	}

	m_MouseMultiClicks.LastMouseInputEvent = inputEvent;
	m_MouseMultiClicks.LastClickTime = clickTime;
	m_MouseMultiClicks.LastClick_X = mouseX;
	m_MouseMultiClicks.LastClick_Y = mouseY;

	return m_MouseMultiClicks.CountSuccessiveClicks;
}


//! send the event to the right receiver
bool SGPDeviceWin32::postEventFromUser(const SSGPEvent& event)
{
	bool absorbed = false;

	if (m_UserEventReceiver)
		absorbed = m_UserEventReceiver->OnEvent(event);

	return absorbed;
}

//! Sets a new event receiver to receive events
void SGPDeviceWin32::setEventReceiver(ISGPEventReceiver* receiver)
{
	m_UserEventReceiver = receiver;
}

//! Returns poinhter to the current event receiver. Returns 0 if there is none.
ISGPEventReceiver* SGPDeviceWin32::getEventReceiver()
{
	return m_UserEventReceiver;
}




}