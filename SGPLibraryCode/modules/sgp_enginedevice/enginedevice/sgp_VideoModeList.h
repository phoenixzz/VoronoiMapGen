#ifndef	__SGP_VIDEOMODELIST_HEADER__
#define __SGP_VIDEOMODELIST_HEADER__

	//! A list of all available video modes.
	/** You can get a list via SGPDevice::getVideoModeList(). */
	class ISGPVideoModeList
	{
	public:

		//! Gets amount of video modes in the list.
		/** \return Returns amount of video modes. */
		virtual int32 getVideoModeCount() const = 0;

		//! Get the screen size of a video mode in pixels.
		/** \param modeNumber: zero based index of the video mode.
		\return Size of screen in pixels of the specified video mode. */
		virtual void getVideoModeResolution(int32 modeNumber, uint32& Width, uint32& Height) const = 0;

		//! Get a supported screen size with certain constraints.
		/** \param minSize: Minimum dimensions required.
		\param maxSize: Maximum dimensions allowed.
		\return Size of screen in pixels which matches the requirements.
		as good as possible. */
		virtual void getVideoModeResolution(const uint32 minWidth, const uint32 minHeight,
			const uint32 maxWidth, const uint32 maxHeight,
			uint32& Width, uint32& Height) const = 0;

		//! Get the pixel depth of a video mode in bits.
		/** \param modeNumber: zero based index of the video mode.
		\return Size of each pixel of the specified video mode in bits. */
		virtual int32 getVideoModeDepth(int32 modeNumber) const = 0;

		//! Get current desktop screen resolution.
		/** \return Size of screen in pixels of the current desktop video mode. */
		virtual void getDesktopResolution(uint32& Width, uint32& Height) const = 0;

		//! Get the pixel depth of a video mode in bits.
		/** \return Size of each pixel of the current desktop video mode in bits. */
		virtual int32 getDesktopDepth() const = 0;
	};

	class CVideoModeList : public ISGPVideoModeList
	{
	public:

		//! constructor
		CVideoModeList();

		//! Gets amount of video modes in the list.
		virtual int32 getVideoModeCount() const;

		//! Returns the screen size of a video mode in pixels.
		virtual void getVideoModeResolution(int32 modeNumber, uint32& Width, uint32& Height) const;

		//! Returns the screen size of an optimal video mode in pixels.
		virtual void getVideoModeResolution(const uint32 minWidth, const uint32 minHeight,
			const uint32 maxWidth, const uint32 maxHeight,
			uint32& Width, uint32& Height) const;

		//! Returns the pixel depth of a video mode in bits.
		virtual int32 getVideoModeDepth(int32 modeNumber) const;

		//! Returns current desktop screen resolution.
		virtual void getDesktopResolution(uint32& Width, uint32& Height) const;

		//! Returns the pixel depth of a video mode in bits.
		virtual int32 getDesktopDepth() const;

		//! adds a new mode to the list
		void addMode(uint32 width, uint32 height, int32 depth);
		void setDesktop(int32 desktopDepth, uint32 desktopWidth, uint32 desktopHeight);

	private:
		struct SVideoMode
		{
			uint32 width, height;
			int32 depth;

			bool operator==(const SVideoMode& other) const
			{
				return (width == other.width) && (height == other.height) && (depth == other.depth);
			}

			bool operator <(const SVideoMode& other) const
			{
				return ( width < other.width ||
						(width == other.width && height < other.height) ||
						(width == other.width && height == other.height && depth < other.depth) );
			}
		};

		Array<SVideoMode> VideoModes;
		SVideoMode Desktop;
	};


#endif