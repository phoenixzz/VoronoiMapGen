#ifndef __SPG_EVENT_HEADER__
#define __SPG_EVENT_HEADER__


enum SGP_KEY_CODE
{
	KEY_LBUTTON          = 0x01,  // Left mouse button
	KEY_RBUTTON          = 0x02,  // Right mouse button
	KEY_CANCEL           = 0x03,  // Control-break processing
	KEY_MBUTTON          = 0x04,  // Middle mouse button (three-button mouse)
	KEY_XBUTTON1         = 0x05,  // Windows 2000/XP: X1 mouse button
	KEY_XBUTTON2         = 0x06,  // Windows 2000/XP: X2 mouse button
	KEY_BACK             = 0x08,  // BACKSPACE key
	KEY_TAB              = 0x09,  // TAB key
	KEY_CLEAR            = 0x0C,  // CLEAR key
	KEY_RETURN           = 0x0D,  // ENTER key
	KEY_SHIFT            = 0x10,  // SHIFT key
	KEY_CONTROL          = 0x11,  // CTRL key
	KEY_MENU             = 0x12,  // ALT key
	KEY_PAUSE            = 0x13,  // PAUSE key
	KEY_CAPITAL          = 0x14,  // CAPS LOCK key
	KEY_KANA             = 0x15,  // IME Kana mode
	KEY_HANGUEL          = 0x15,  // IME Hanguel mode (maintained for compatibility use KEY_HANGUL)
	KEY_HANGUL           = 0x15,  // IME Hangul mode
	KEY_JUNJA            = 0x17,  // IME Junja mode
	KEY_FINAL            = 0x18,  // IME final mode
	KEY_HANJA            = 0x19,  // IME Hanja mode
	KEY_KANJI            = 0x19,  // IME Kanji mode
	KEY_ESCAPE           = 0x1B,  // ESC key
	KEY_CONVERT          = 0x1C,  // IME convert
	KEY_NONCONVERT       = 0x1D,  // IME nonconvert
	KEY_ACCEPT           = 0x1E,  // IME accept
	KEY_MODECHANGE       = 0x1F,  // IME mode change request
	KEY_SPACE            = 0x20,  // SPACEBAR
	KEY_PRIOR            = 0x21,  // PAGE UP key
	KEY_NEXT             = 0x22,  // PAGE DOWN key
	KEY_END              = 0x23,  // END key
	KEY_HOME             = 0x24,  // HOME key
	KEY_LEFT             = 0x25,  // LEFT ARROW key
	KEY_UP               = 0x26,  // UP ARROW key
	KEY_RIGHT            = 0x27,  // RIGHT ARROW key
	KEY_DOWN             = 0x28,  // DOWN ARROW key
	KEY_SELECT           = 0x29,  // SELECT key
	KEY_PRINT            = 0x2A,  // PRINT key
	KEY_EXECUT           = 0x2B,  // EXECUTE key
	KEY_SNAPSHOT         = 0x2C,  // PRINT SCREEN key
	KEY_INSERT           = 0x2D,  // INS key
	KEY_DELETE           = 0x2E,  // DEL key
	KEY_HELP             = 0x2F,  // HELP key
	KEY_KEY_0            = 0x30,  // 0 key
	KEY_KEY_1            = 0x31,  // 1 key
	KEY_KEY_2            = 0x32,  // 2 key
	KEY_KEY_3            = 0x33,  // 3 key
	KEY_KEY_4            = 0x34,  // 4 key
	KEY_KEY_5            = 0x35,  // 5 key
	KEY_KEY_6            = 0x36,  // 6 key
	KEY_KEY_7            = 0x37,  // 7 key
	KEY_KEY_8            = 0x38,  // 8 key
	KEY_KEY_9            = 0x39,  // 9 key
	KEY_KEY_A            = 0x41,  // A key
	KEY_KEY_B            = 0x42,  // B key
	KEY_KEY_C            = 0x43,  // C key
	KEY_KEY_D            = 0x44,  // D key
	KEY_KEY_E            = 0x45,  // E key
	KEY_KEY_F            = 0x46,  // F key
	KEY_KEY_G            = 0x47,  // G key
	KEY_KEY_H            = 0x48,  // H key
	KEY_KEY_I            = 0x49,  // I key
	KEY_KEY_J            = 0x4A,  // J key
	KEY_KEY_K            = 0x4B,  // K key
	KEY_KEY_L            = 0x4C,  // L key
	KEY_KEY_M            = 0x4D,  // M key
	KEY_KEY_N            = 0x4E,  // N key
	KEY_KEY_O            = 0x4F,  // O key
	KEY_KEY_P            = 0x50,  // P key
	KEY_KEY_Q            = 0x51,  // Q key
	KEY_KEY_R            = 0x52,  // R key
	KEY_KEY_S            = 0x53,  // S key
	KEY_KEY_T            = 0x54,  // T key
	KEY_KEY_U            = 0x55,  // U key
	KEY_KEY_V            = 0x56,  // V key
	KEY_KEY_W            = 0x57,  // W key
	KEY_KEY_X            = 0x58,  // X key
	KEY_KEY_Y            = 0x59,  // Y key
	KEY_KEY_Z            = 0x5A,  // Z key
	KEY_LWIN             = 0x5B,  // Left Windows key (Microsoft Natural keyboard)
	KEY_RWIN             = 0x5C,  // Right Windows key (Natural keyboard)
	KEY_APPS             = 0x5D,  // Applications key (Natural keyboard)
	KEY_SLEEP            = 0x5F,  // Computer Sleep key
	KEY_NUMPAD0          = 0x60,  // Numeric keypad 0 key
	KEY_NUMPAD1          = 0x61,  // Numeric keypad 1 key
	KEY_NUMPAD2          = 0x62,  // Numeric keypad 2 key
	KEY_NUMPAD3          = 0x63,  // Numeric keypad 3 key
	KEY_NUMPAD4          = 0x64,  // Numeric keypad 4 key
	KEY_NUMPAD5          = 0x65,  // Numeric keypad 5 key
	KEY_NUMPAD6          = 0x66,  // Numeric keypad 6 key
	KEY_NUMPAD7          = 0x67,  // Numeric keypad 7 key
	KEY_NUMPAD8          = 0x68,  // Numeric keypad 8 key
	KEY_NUMPAD9          = 0x69,  // Numeric keypad 9 key
	KEY_MULTIPLY         = 0x6A,  // Multiply key
	KEY_ADD              = 0x6B,  // Add key
	KEY_SEPARATOR        = 0x6C,  // Separator key
	KEY_SUBTRACT         = 0x6D,  // Subtract key
	KEY_DECIMAL          = 0x6E,  // Decimal key
	KEY_DIVIDE           = 0x6F,  // Divide key
	KEY_F1               = 0x70,  // F1 key
	KEY_F2               = 0x71,  // F2 key
	KEY_F3               = 0x72,  // F3 key
	KEY_F4               = 0x73,  // F4 key
	KEY_F5               = 0x74,  // F5 key
	KEY_F6               = 0x75,  // F6 key
	KEY_F7               = 0x76,  // F7 key
	KEY_F8               = 0x77,  // F8 key
	KEY_F9               = 0x78,  // F9 key
	KEY_F10              = 0x79,  // F10 key
	KEY_F11              = 0x7A,  // F11 key
	KEY_F12              = 0x7B,  // F12 key
	KEY_F13              = 0x7C,  // F13 key
	KEY_F14              = 0x7D,  // F14 key
	KEY_F15              = 0x7E,  // F15 key
	KEY_F16              = 0x7F,  // F16 key
	KEY_F17              = 0x80,  // F17 key
	KEY_F18              = 0x81,  // F18 key
	KEY_F19              = 0x82,  // F19 key
	KEY_F20              = 0x83,  // F20 key
	KEY_F21              = 0x84,  // F21 key
	KEY_F22              = 0x85,  // F22 key
	KEY_F23              = 0x86,  // F23 key
	KEY_F24              = 0x87,  // F24 key
	KEY_NUMLOCK          = 0x90,  // NUM LOCK key
	KEY_SCROLL           = 0x91,  // SCROLL LOCK key
	KEY_LSHIFT           = 0xA0,  // Left SHIFT key
	KEY_RSHIFT           = 0xA1,  // Right SHIFT key
	KEY_LCONTROL         = 0xA2,  // Left CONTROL key
	KEY_RCONTROL         = 0xA3,  // Right CONTROL key
	KEY_LMENU            = 0xA4,  // Left MENU key
	KEY_RMENU            = 0xA5,  // Right MENU key
	KEY_OEM_1            = 0xBA,  // for US    ";:"
	KEY_PLUS             = 0xBB,  // Plus Key   "+"
	KEY_COMMA            = 0xBC,  // Comma Key  ","
	KEY_MINUS            = 0xBD,  // Minus Key  "-"
	KEY_PERIOD           = 0xBE,  // Period Key "."
	KEY_OEM_2            = 0xBF,  // for US    "/?"
	KEY_OEM_3            = 0xC0,  // for US    "`~"
	KEY_OEM_4            = 0xDB,  // for US    "[{"
	KEY_OEM_5            = 0xDC,  // for US    "\|"
	KEY_OEM_6            = 0xDD,  // for US    "]}"
	KEY_OEM_7            = 0xDE,  // for US    "'""
	KEY_OEM_8            = 0xDF,  // None
	KEY_OEM_AX           = 0xE1,  // for Japan "AX"
	KEY_OEM_102          = 0xE2,  // "<>" or "\|"
	KEY_ATTN             = 0xF6,  // Attn key
	KEY_CRSEL            = 0xF7,  // CrSel key
	KEY_EXSEL            = 0xF8,  // ExSel key
	KEY_EREOF            = 0xF9,  // Erase EOF key
	KEY_PLAY             = 0xFA,  // Play key
	KEY_ZOOM             = 0xFB,  // Zoom key
	KEY_PA1              = 0xFD,  // PA1 key
	KEY_OEM_CLEAR        = 0xFE,   // Clear key

	KEY_KEY_CODES_COUNT  = 0xFF // this is not a key, but the amount of keycodes there are.
};

//! Enumeration for all event types there are.
enum SGP_EVENT_TYPE
{
	//! An event of the graphical user interface.
	/** GUI events are created by the GUI environment or the GUI elements in response
	to mouse or keyboard events. When a GUI element receives an event it will either
	process it and return true, or pass the event to its parent. If an event is not absorbed
	before it reaches the root element then it will then be passed to the user receiver. */
	SGPET_GUI_EVENT = 0,

	//! A mouse input event.
	/** Mouse events are created by the device and passed to SGPDevice::postEventFromUser
	in response to mouse input received from the operating system.
	Mouse events are first passed to the user receiver, then to the GUI environment and its elements,
	then finally the input receiving scene manager where it is passed to the active camera.
	*/
	SGPET_MOUSE_INPUT_EVENT,

	//! A key input event.
	/** Like mouse events, keyboard events are created by the device and passed to
	SGPDevice::postEventFromUser. They take the same path as mouse events. */
	SGPET_KEY_INPUT_EVENT,

	//! A joystick (joypad, gamepad) input event.
	/** Joystick events are created by polling all connected joysticks once per
	device run() and then passing the events to SGPDevice::postEventFromUser.
	They take the same path as mouse events.
	Windows: Implemented.
	Linux: Implemented, with POV hat issues.
	MacOS / Other: Not yet implemented.
	*/
	SGPET_JOYSTICK_INPUT_EVENT,

	//! A log event
	/** Log events are only passed to the user receiver if there is one. If they are absorbed by the
	user receiver then no text will be sent to the console. */
	SGPET_LOG_TEXT_EVENT,

	//! A user event with user data.
	/** This is not used by SGP and can be used to send user
	specific data though the system. The SGP 'window handle'
	can be obtained from SGPDevice::getExposedVideoData()
	The usage and behavior depends on the operating system:
	Windows: send a WM_USER message to the SGP Window; the
		wParam and lParam will be used to populate the
		UserData1 and UserData2 members of the SUserEvent.
	Linux: send a ClientMessage via XSendEvent to the SGP
		Window; the data.l[0] and data.l[1] members will be
		casted to int32 and used as UserData1 and UserData2.
	MacOS: Not yet implemented
	*/
	SGPET_USER_EVENT,

	//! This enum is never used, it only forces the compiler to
	//! compile these enumeration values to 32 bit.
	SGPET_FORCE_32_BIT = 0x7fffffff

};

//! Enumeration for all GUI input/system events
enum SGP_GUI_EVENT
{
	//! window / viewport is resized.
	SGPGE_RESIZE = 0,

	//! No real event. Just for convenience to get number of events
	SGPGE_COUNT
};

//! Enumeration for all mouse input events
enum SGP_MOUSE_INPUT_EVENT
{
	//! Left mouse button was pressed down.
	SGPMIE_LMOUSE_PRESSED_DOWN = 0,

	//! Right mouse button was pressed down.
	SGPMIE_RMOUSE_PRESSED_DOWN,

	//! Middle mouse button was pressed down.
	SGPMIE_MMOUSE_PRESSED_DOWN,

	//! Left mouse button was left up.
	SGPMIE_LMOUSE_LEFT_UP,

	//! Right mouse button was left up.
	SGPMIE_RMOUSE_LEFT_UP,

	//! Middle mouse button was left up.
	SGPMIE_MMOUSE_LEFT_UP,

	//! The mouse cursor changed its position.
	SGPMIE_MOUSE_MOVED,

	//! The mouse wheel was moved. Use Wheel value in event data to find out
	//! in what direction and how fast.
	SGPMIE_MOUSE_WHEEL,

	//! Left mouse button double click.
	//! This event is generated after the second SGPMIE_LMOUSE_PRESSED_DOWN event.
	SGPMIE_LMOUSE_DOUBLE_CLICK,

	//! Right mouse button double click.
	//! This event is generated after the second SGPMIE_RMOUSE_PRESSED_DOWN event.
	SGPMIE_RMOUSE_DOUBLE_CLICK,

	//! Middle mouse button double click.
	//! This event is generated after the second SGPMIE_MMOUSE_PRESSED_DOWN event.
	SGPMIE_MMOUSE_DOUBLE_CLICK,

	//! Left mouse button triple click.
	//! This event is generated after the third SGPMIE_LMOUSE_PRESSED_DOWN event.
	SGPMIE_LMOUSE_TRIPLE_CLICK,

	//! Right mouse button triple click.
	//! This event is generated after the third EMIE_RMOUSE_PRESSED_DOWN event.
	SGPMIE_RMOUSE_TRIPLE_CLICK,

	//! Middle mouse button triple click.
	//! This event is generated after the third EMIE_MMOUSE_PRESSED_DOWN event.
	SGPMIE_MMOUSE_TRIPLE_CLICK,

	//! No real event. Just for convenience to get number of events
	SGPMIE_COUNT
};

//! Masks for mouse button states
enum ESGP_MOUSE_BUTTON_STATE_MASK
{
	SGPMBSM_LEFT    = 0x01,
	SGPMBSM_RIGHT   = 0x02,
	SGPMBSM_MIDDLE  = 0x04,

	//! currently only on windows
	SGPMBSM_EXTRA1  = 0x08,

	//! currently only on windows
	SGPMBSM_EXTRA2  = 0x10,

	SGPMBSM_FORCE_32_BIT = 0x7fffffff
};

//! SSGPEvent hold information about an event. See ISGPEventReceiver for details on event handling.
struct SSGPEvent
{
	//! Any kind of mouse event.
	struct SSGPMouseInput
	{
		//! X position of mouse cursor
		uint32 X;

		//! Y position of mouse cursor
		uint32 Y;

		//! mouse wheel delta, often 1.0 or -1.0, but can have other values < 0.f or > 0.f;
		/** Only valid if event was SGPMIE_MOUSE_WHEEL */
		float Wheel;

		//! True if shift was also pressed
		bool Shift:1;

		//! True if ctrl was also pressed
		bool Control:1;

		//! A bitmap of button states. You can use isButtonPressed() to determine
		//! if a button is pressed or not.
		//! Currently only valid if the event was SGPMIE_MOUSE_MOVED
		uint32 ButtonStates;

		//! Is the left button pressed down?
		bool isLeftPressed() const { return 0 != ( ButtonStates & SGPMBSM_LEFT ); }

		//! Is the right button pressed down?
		bool isRightPressed() const { return 0 != ( ButtonStates & SGPMBSM_RIGHT ); }

		//! Is the middle button pressed down?
		bool isMiddlePressed() const { return 0 != ( ButtonStates & SGPMBSM_MIDDLE ); }

		//! Type of mouse event
		SGP_MOUSE_INPUT_EVENT Event;
	};

	//! Any kind of keyboard event.
	struct SSGPKeyInput
	{
		//! Character corresponding to the key (0, if not a character)
		wchar_t Char;

		//! Key which has been pressed or released
		SGP_KEY_CODE Key;

		//! If not true, then the key was left up
		bool PressedDown:1;

		//! True if shift was also pressed
		bool Shift:1;

		//! True if ctrl was also pressed
		bool Control:1;
	};

	//! A joystick event.
	/** Unlike other events, joystick events represent the result of polling
	 * each connected joystick once per run() of the device. Joystick events will
	 * not be generated by default.  If joystick support is available for the
	 * active device, SGP_WITH_JOYSTICK_EVENTS is defined, and
	 * @ref SGPDevice::activateJoysticks() has been called, an event of
	 * this type will be generated once per joystick per @ref SGPDevice::run()
	 * regardless of whether the state of the joystick has actually changed. */
	struct SSGPJoystickEvent
	{
		enum
		{
			NUMBER_OF_BUTTONS = 32,

			AXIS_X = 0, // e.g. analog stick 1 left to right
			AXIS_Y,		// e.g. analog stick 1 top to bottom
			AXIS_Z,		// e.g. throttle, or analog 2 stick 2 left to right
			AXIS_R,		// e.g. rudder, or analog 2 stick 2 top to bottom
			AXIS_U,
			AXIS_V,
			NUMBER_OF_AXES
		};

		/** A bitmap of button states.  You can use IsButtonPressed() to
		 ( check the state of each button from 0 to (NUMBER_OF_BUTTONS - 1) */
		uint32 ButtonStates;

		/** For AXIS_X, AXIS_Y, AXIS_Z, AXIS_R, AXIS_U and AXIS_V
		 * Values are in the range -32768 to 32767, with 0 representing
		 * the center position.  You will receive the raw value from the
		 * joystick, and so will usually want to implement a dead zone around
		 * the center of the range. Axes not supported by this joystick will
		 * always have a value of 0. On Linux, POV hats are represented as axes,
		 * usually the last two active axis.
		 */
		int16 Axis[NUMBER_OF_AXES];

		/** The POV represents the angle of the POV hat in degrees * 100,
		 * from 0 to 35,900.  A value of 65535 indicates that the POV hat
		 * is centered (or not present).
		 * This value is only supported on Windows.  On Linux, the POV hat
		 * will be sent as 2 axes instead. */
		uint16 POV;

		//! The ID of the joystick which generated this event.
		/** This is an internal SGP index; it does not map directly
		 * to any particular hardware joystick. */
		uint8 Joystick;

		//! A helper function to check if a button is pressed.
		bool IsButtonPressed(uint32 button) const
		{
			if(button >= (uint32)NUMBER_OF_BUTTONS)
				return false;

			return (ButtonStates & (1 << button)) ? true : false;
		}
	};


	//! Any kind of log event.
	struct SSGPLogEvent
	{
		//! Pointer to text which has been logged
		const char* Text;

		//! Log level in which the text has been logged
		ESGPLOG_LEVEL Level;
	};

	//! Any kind of user event.
	struct SSGPUserEvent
	{
		//! Some user specified data as int
		int32 UserData1;

		//! Another user specified data as int
		int32 UserData2;
	};

	SGP_EVENT_TYPE EventType;

	union
	{
		struct SSGPMouseInput MouseInput;
		struct SSGPKeyInput KeyInput;
		struct SSGPJoystickEvent JoystickEvent;
		struct SSGPLogEvent LogEvent;
		struct SSGPUserEvent UserEvent;
	};
};

//! Interface of an object which can receive events.
/** Many of the engine's classes inherit IEventReceiver so they are able to
process events. Events usually start at a postEventFromUser function and are
passed down through a chain of event receivers until OnEvent returns true. See
SGP_EVENT_TYPE for a description of where each type of event starts, and the
path it takes through the system. */
class ISGPEventReceiver
{
public:

	//! Destructor
	virtual ~ISGPEventReceiver() {}

	//! Called if an event happened.
	/** Please take care that you should only return 'true' when you don't want to 
	* process the event any further. So 'true' does mean that an event is completely done.
	* Therefore your return value for all unprocessed events should be 'false'.
	\return True if the event was processed.
	*/
	virtual bool OnEvent(const SSGPEvent& event) = 0;
};


//! Information on a joystick, returned from @ref SGPDevice::activateJoysticks()
struct SSGPJoystickInfo
{
	//! The ID of the joystick
	/** This is an internal SGP index; it does not map directly
	 * to any particular hardware joystick. It corresponds to the
	 * SSGPJoystickEvent Joystick ID. */
	uint8				Joystick;

	//! The name that the joystick uses to identify itself.
	char*				Name;

	//! The number of buttons that the joystick has.
	uint32				Buttons;

	//! The number of axes that the joystick has, i.e. X, Y, Z, R, U, V.
	/** Note: with a Linux device, the POV hat (if any) will use two axes. These
	 *  will be included in this count. */
	uint32				Axes;

	//! An indication of whether the joystick has a POV hat.
	/** A Windows device will identify the presence or absence or the POV hat.  A
	 *  Linux device cannot, and will always return POV_HAT_UNKNOWN. */
	enum
	{
		//! A hat is definitely present.
		POV_HAT_PRESENT,

		//! A hat is definitely not present.
		POV_HAT_ABSENT,

		//! The presence or absence of a hat cannot be determined.
		POV_HAT_UNKNOWN
	} PovHat;
}; // struct SSGPJoystickInfo	


#endif		// __SPG_EVENT_HEADER__