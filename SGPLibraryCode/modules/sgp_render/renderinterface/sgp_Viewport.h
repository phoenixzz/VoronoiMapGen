#ifndef __SGP_VIEWPORT_HEADER__ 
#define __SGP_VIEWPORT_HEADER__

enum SGP_CAMERAMODE_TYPE
{
	SGPCT_PERSPECTIVE = 0,		// perspective projection
	SGPCT_ORTHOGONAL,			// orthogonal projection
	SGPCT_TWOD					// world coords equal to screen coords (2D mode)
};

struct SViewPort
{
	uint32 X;
	uint32 Y;  
	uint32 Width;
	uint32 Height;
	SViewPort()
	{ X = Y = Width = Height = 0; }
	SViewPort(uint32 _X, uint32 _Y, uint32 _Width, uint32 _Height)
	{ X = _X; Y = _Y; Width = _Width; Height = _Height; }
};

struct SDimension2D
{
	uint32 Width, Height;
	SDimension2D()
	{ Width = Height = 0; }
	SDimension2D(uint32 _width, uint32 _height)
	{
		Width = _width; Height = _height;
	}

	//! Get the optimal size according to some properties
	/** This is a function often used for texture dimension
	calculations. The function returns the next larger or
	smaller dimension which is a power-of-two dimension
	(2^n,2^m) and/or square (Width=Height).
	\param requirePowerOfTwo Forces the result to use only
	powers of two as values.
	\param requireSquare Makes width==height in the result
	\param larger Choose whether the result is larger or
	smaller than the current dimension. If one dimension
	need not be changed it is kept with any value of larger.
	\param maxValue Maximum texturesize. if value > 0 size is clamped to maxValue
	\return The optimal dimension under the given constraints. */
	SDimension2D getOptimalSize(
		bool requirePowerOfTwo = true,
		bool requireSquare = false,
		bool larger = true,
		uint32 maxValue = 0) const
	{
		uint32 i=1;
		uint32 j=1;
		if (requirePowerOfTwo)
		{
			while (i<(uint32)Width)
				i<<=1;
			if (!larger && i!=1 && i!=(uint32)Width)
				i>>=1;
			while (j<(uint32)Height)
				j<<=1;
			if (!larger && j!=1 && j!=(uint32)Height)
				j>>=1;
		}
		else
		{
			i=(uint32)Width;
			j=(uint32)Height;
		}

		if (requireSquare)
		{
			if ((larger && (i>j)) || (!larger && (i<j)))
				j=i;
			else
				i=j;
		}

		if ( maxValue > 0 && i > maxValue)
			i = maxValue;

		if ( maxValue > 0 && j > maxValue)
			j = maxValue;

		return SDimension2D(i,j);
	}
};

struct SRect
{
	int32 Left, Top;
	int32 Right, Bottom;
	SRect()
	{ Left = Top = Right = Bottom = 0; }
};

#endif		// __SGP_VIEWPORT_HEADER__