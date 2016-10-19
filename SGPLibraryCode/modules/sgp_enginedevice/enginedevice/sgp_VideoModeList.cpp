
CVideoModeList::CVideoModeList()
{
	Desktop.depth = 0;
	Desktop.width = Desktop.height = 0;
}


void CVideoModeList::setDesktop(int32 desktopDepth, uint32 desktopWidth, uint32 desktopHeight)
{
	Desktop.depth = desktopDepth;
	Desktop.width = desktopWidth;
	Desktop.height = desktopHeight;
}


//! Gets amount of video modes in the list.
int32 CVideoModeList::getVideoModeCount() const
{
	return (int32)VideoModes.size();
}


//! Returns the screen size of a video mode in pixels.
void CVideoModeList::getVideoModeResolution(int32 modeNumber, uint32& Width, uint32& Height) const
{
	if (modeNumber < 0 || modeNumber > (int32)VideoModes.size())
		Width = Height = 0;

	Width = VideoModes[modeNumber].width;
	Height = VideoModes[modeNumber].height;
}


void CVideoModeList::getVideoModeResolution(
	const uint32 minWidth, const uint32 minHeight,
	const uint32 maxWidth, const uint32 maxHeight,
	uint32& Width, uint32& Height) const
{
	int best = VideoModes.size();
	// if only one or no mode
	if (best < 2)
	{
		getVideoModeResolution(0, Width, Height);
		return;
	}

	int i;
	for (i=0; i<VideoModes.size(); ++i)
	{
		if( VideoModes[i].width  >= minWidth &&
			VideoModes[i].height >= minHeight &&
			VideoModes[i].width  <= maxWidth &&
			VideoModes[i].height <= maxHeight )
			best = i;
	}

	// we take the last one found, the largest one fitting
	if (best < VideoModes.size())
	{
		Width = VideoModes[best].width;
		Height = VideoModes[best].height;
		return;
	}
	const uint32 minArea = minWidth * minHeight;
	const uint32 maxArea = maxWidth * maxHeight;
	uint32 minDist = 0xffffffff;
	best = 0;
	for (i=0; i<VideoModes.size(); ++i)
	{
		const uint32 area = VideoModes[i].width * VideoModes[i].height;
		const uint32 dist = jmin(abs(int(minArea-area)), abs(int(maxArea-area)));
		if (dist < minDist)
		{
			minDist = dist;
			best = i;
		}
	}

	Width = VideoModes[best].width;
	Height = VideoModes[best].height;

	return;
}


//! Returns the pixel depth of a video mode in bits.
int32 CVideoModeList::getVideoModeDepth(int32 modeNumber) const
{
	if (modeNumber < 0 || modeNumber > (int32)VideoModes.size())
		return 0;

	return VideoModes[modeNumber].depth;
}


//! Returns current desktop screen resolution.
void CVideoModeList::getDesktopResolution(uint32& Width, uint32& Height) const
{
	Width = Desktop.width;
	Height = Desktop.height;
	return;
}


//! Returns the pixel depth of a video mode in bits.
int32 CVideoModeList::getDesktopDepth() const
{
	return Desktop.depth;
}


//! adds a new mode to the list
void CVideoModeList::addMode(uint32 width, uint32 height, int32 depth)
{
	SVideoMode m;
	m.depth = depth;
	m.width = width;
	m.height = height;

	for (int i=0; i<VideoModes.size(); ++i)
	{
		if (VideoModes[i] == m)
			return;
	}

	uint32 currentarea = width * height;
	int best = 0;
	for (int i=0; i<VideoModes.size(); ++i)
	{
		const uint32 area = VideoModes[i].width * VideoModes[i].height;
		if( currentarea >= area )
			best = i+1;
	}

	VideoModes.insert(best,m);
}
