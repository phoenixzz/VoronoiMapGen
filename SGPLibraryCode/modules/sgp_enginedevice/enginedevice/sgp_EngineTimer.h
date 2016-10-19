#ifndef __SPG_ENGINETIMER_HEADER__
#define __SPG_ENGINETIMER_HEADER__

//! Interface for getting and manipulating the virtual time
class ISGPTimer
{
public:
	virtual ~ISGPTimer() {}

	//! Returns current real time in milliseconds of the system.
	/** This value does not start with 0 when the application starts.
	For example in one implementation the value returned could be the
	amount of milliseconds which have elapsed since the system was started.
	*/
	virtual double getRealTime() const = 0;

	//! Returns current virtual time in milliseconds.
	/** This value could starts with 0 after you using setTime function. 
	This value depends on the set speed of
	the timer if the timer is stopped, etc. If you need the system time,
	use getRealTime() */
	virtual uint32 getTime() const = 0;

	//! sets current virtual time
	virtual void setTime(uint32 time) = 0;


	//! Sets the speed of the timer
	/** The speed is the factor with which the time is running faster or
	slower then the real system time. */
	virtual void setSpeed(float speed = 1.0f) = 0;

	//! Returns current speed of the timer
	/** The speed is the factor with which the time is running faster or
	slower then the real system time. */
	virtual float getSpeed() const = 0;

	//! Returns virtual time elapsed since previous tick in milliseconds.
	virtual double GetElapsedTime() = 0;


	//! Advances the virtual time
	/** Makes the virtual timer update the time value based on the real
	time. This is called automatically when calling SGPDevice::run(),
	but you can call it manually if you don't use this method. */
	virtual void tick() = 0;
};

class CSGPEngineTimer : public ISGPTimer
{
public:
	CSGPEngineTimer() : m_Timer(Time::getCurrentTime())
	{
		VirtualTimerSpeed = 1.0f;
		VirtualTimeElapsed = 0;
		LastVirtualTime = 0;

		StaticTime = getRealTime();
		StartRealTime = StaticTime;
	}

	//! Returns current real time in milliseconds of the system.
	/** Returns the number of millisecs since a fixed event (usually system startup).  */
	virtual double getRealTime() const
	{
		return m_Timer.getMillisecondCounterHiRes();
	}

	//! Returns current virtual time in milliseconds.
	/** This value starts with 0 and can be manipulated using setTime().
	If you need the system time, use getRealTime() */
	virtual uint32 getTime() const
	{
		return LastVirtualTime + (uint32)((StaticTime - StartRealTime) * (double)(VirtualTimerSpeed));
	}

	//! sets current virtual time
	virtual void setTime(uint32 _time)
	{
		StaticTime = getRealTime();
		LastVirtualTime = _time;
		StartRealTime = StaticTime;
	}

	//! Returns virtual time elapsed since previous tick in milliseconds.
	virtual double GetElapsedTime()
	{
		return VirtualTimeElapsed;
	}

	//! Sets the speed of the timer
	/** The speed is the factor with which the time is running faster or slower then the
	real system time. */
	virtual void setSpeed(float speed = 1.0f)
	{
		setTime(getTime());

		VirtualTimerSpeed = speed;
		if (VirtualTimerSpeed < 0.0f)
			VirtualTimerSpeed = 0.0f;
	}

	//! Returns current speed of the timer
	/** The speed is the factor with which the time is running faster or slower then the
	real system time. */
	virtual float getSpeed() const
	{
		return VirtualTimerSpeed;
	}


	//! Advances the virtual time
	/** Makes the virtual timer update the time value based on the real time. This is
	called automaticly when calling SGPDevice::run(), but you can call it manually
	if you don't use this method. */
	virtual void tick()
	{
		VirtualTimeElapsed = (getRealTime() - StaticTime) * (double)(VirtualTimerSpeed);
		StaticTime = getRealTime();
	}

private:
	Time m_Timer;				// Core Timer

	double StartRealTime;		// Real-time computer Data Time
	double StaticTime;			// Computer Data Time when this timer created

	uint32 LastVirtualTime;		// User given Virtual Time (could set 0)
	float VirtualTimerSpeed;	// scaling factor for time 

	double VirtualTimeElapsed;	// Virtual time elapsed since previous tick (in milliseconds)
};



#endif		// __SPG_ENGINETIMER_HEADER__