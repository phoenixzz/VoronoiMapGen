#ifndef __SGP_PIXELFORMAT_HEADER__
#define __SGP_PIXELFORMAT_HEADER__

	//! An enum for the pixel format of textures used by the SGP Engine.
	/** A pixel format specifies how color information is stored. */
	enum SGP_PIXEL_FORMAT
	{
		//! 16 bit color format used by the software driver.
		/** It is thus preferred by all other irrlicht engine video drivers.
		There are 5 bits for every color component, and a single bit is left
		for alpha information. */
		SGPPF_A1R5G5B5 = 0,

		//! Standard 16 bit color format.
		SGPPF_R5G6B5,

		//! 24 bit color, no alpha channel, but 8 bit for red, green and blue.
		SGPPF_R8G8B8,

		//! Default 32 bit color format. 8 bits are used for every component: red, green, blue and alpha.
		SGPPF_A8R8G8B8,

		//! 16 bit color format.
		SGPPF_A4R4G4B4,

		/** Floating Point formats. The following formats may only be used for render target textures. */

		//! 16 bit floating point format using 16 bits for the red channel.
		SGPPF_R16F,

		//! 32 bit floating point format using 16 bits for the red channel and 16 bits for the green channel.
		SGPPF_G16R16F,

		//! 64 bit floating point format 16 bits are used for the red, green, blue and alpha channels.
		SGPPF_A16B16G16R16F,

		//! 32 bit floating point format using 32 bits for the red channel.
		SGPPF_R32F,

		//! 64 bit floating point format using 32 bits for the red channel and 32 bits for the green channel.
		SGPPF_G32R32F,

		//! 128 bit floating point format. 32 bits are used for the red, green, blue and alpha channels.
		SGPPF_A32B32G32R32F,

		//! Unknown color format:
		SGPPF_UNKNOWN
	};


	// Color Converting Functions

	// 1 - Bit Alpha Blending
	// c2 - dest
	// c1 - src
	inline uint16 PixelBlend16 ( const uint16 c2, const uint16 c1 )
	{
		uint16 mask = ((c1 & 0x8000) >> 15 ) + 0x7fff;
		return (c2 & mask ) | ( c1 & ~mask );
	}
	/*!
		Pixel = dest * ( 1 - alpha ) + source * alpha
		alpha [0-32]
	*/
	// c2 - dest
	// c1 - src
	inline uint16 PixelBlend16 ( const uint16 c2, const uint32 c1, const uint16 alpha )
	{
		const uint16 srcRB = c1 & 0x7C1F;
		const uint16 srcXG = c1 & 0x03E0;

		const uint16 dstRB = c2 & 0x7C1F;
		const uint16 dstXG = c2 & 0x03E0;

		uint32 rb = srcRB - dstRB;
		uint32 xg = srcXG - dstXG;

		rb *= alpha;
		xg *= alpha;
		rb >>= 5;
		xg >>= 5;

		rb += dstRB;
		xg += dstXG;

		rb &= 0x7C1F;
		xg &= 0x03E0;

		return (uint16)(rb | xg);
	}


	// 1 - Bit Alpha Blending 16Bit SIMD
	inline uint32 PixelBlend16_simd ( const uint32 c2, const uint32 c1 )
	{
		uint32 mask = ((c1 & 0x80008000) >> 15 ) + 0x7fff7fff;
		return (c2 & mask ) | ( c1 & ~mask );
	}
	/*
	Pixel = c0 * (c1/31).
	*/
	inline uint16 PixelMul16_2 ( uint16 c0, uint16 c1)
	{
		return	(uint16)(( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
				( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
				( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
				( c0 & c1 & 0x8000));
	}

	/*!
		c2 = dest
		c1 = src with srcalpha
		Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
	*/
	inline uint32 PixelBlend32 ( const uint32 c2, const uint32 c1 )
	{
		// alpha test
		uint32 alpha = c1 & 0xFF000000;

		if ( 0 == alpha )
			return c2;

		if ( 0xFF000000 == alpha )
		{
			return c1;
		}

		alpha >>= 24;

		// add highbit alpha, if ( alpha > 127 ) alpha += 1;
		alpha += ( alpha >> 7);

		uint32 srcRB = c1 & 0x00FF00FF;
		uint32 srcXG = c1 & 0x0000FF00;

		uint32 dstRB = c2 & 0x00FF00FF;
		uint32 dstXG = c2 & 0x0000FF00;


		uint32 rb = srcRB - dstRB;
		uint32 xg = srcXG - dstXG;

		rb *= alpha;
		xg *= alpha;
		rb >>= 8;
		xg >>= 8;

		rb += dstRB;
		xg += dstXG;

		rb &= 0x00FF00FF;
		xg &= 0x0000FF00;

		return (c1 & 0xFF000000) | rb | xg;
	}
	/*!
		c2 = dest color
		c1 = src color
		Pixel = dest * ( 1 - alpha ) + source * alpha
		alpha [0-256]
	*/
	inline uint32 PixelBlend32 ( const uint32 c2, const uint32 c1, uint32 alpha )
	{
		uint32 srcRB = c1 & 0x00FF00FF;
		uint32 srcXG = c1 & 0x0000FF00;

		uint32 dstRB = c2 & 0x00FF00FF;
		uint32 dstXG = c2 & 0x0000FF00;


		uint32 rb = srcRB - dstRB;
		uint32 xg = srcXG - dstXG;

		rb *= alpha;
		xg *= alpha;
		rb >>= 8;
		xg >>= 8;

		rb += dstRB;
		xg += dstXG;

		rb &= 0x00FF00FF;
		xg &= 0x0000FF00;

		return rb | xg;
	}

	//! Converts a 32bit (X8R8G8B8) color to a 16bit A1R5G5B5 color
	inline uint16 X8R8G8B8toA1R5G5B5(uint32 color)
	{
		return (uint16)(0x8000 |
			( color & 0x00F80000) >> 9 |
			( color & 0x0000F800) >> 6 |
			( color & 0x000000F8) >> 3);
	}


	//! Converts a 32bit (A8R8G8B8) color to a 16bit A1R5G5B5 color
	inline uint16 A8R8G8B8toA1R5G5B5(uint32 color)
	{
		return (uint16)(( color & 0x80000000) >> 16|
			( color & 0x00F80000) >> 9 |
			( color & 0x0000F800) >> 6 |
			( color & 0x000000F8) >> 3);
	}

	//! Converts a 32bit (A8R8G8B8) color to a 16bit R5G6B5 color
	inline uint16 A8R8G8B8toR5G6B5(uint32 color)
	{
		return (uint16)(( color & 0x00F80000) >> 8 |
			( color & 0x0000FC00) >> 5 |
			( color & 0x000000F8) >> 3);
	}

	//! Converts a 16bit (A4R4G4B48) color to a 16bit R5G6B5 color
	inline uint16 A4R4G4B4toR5G6B5(uint16 color)
	{
		return (uint16)(((color & 0x0F00) >> 8) << 16 |
			((color & 0x00F0) >> 4) << 11 |
			(color & 0x000F) << 1);
	}

	//! Converts a 16bit (A4R4G4B48) color to a 16bit A1R5G5B5 color
	inline uint16 A4R4G4B4toA1R5G5B5(uint16 color)
	{
		return (uint16)( (color & 0x8000) |
			(color & 0x0F00) << 3 |
			(color & 0x00F0) << 2 |
			(color & 0x000F) << 1 );
	}

	//! Converts a 32bit (A8R8G8B8) color to a 16bit A4R4G4B4 color
	inline uint16 A8R8G8B8toA4R4G4B4(uint32 color)
	{
		return (uint16)( (color & 0xF0000000) >> 16 |
			(color & 0x00F00000) >> 12 |
			(color & 0x0000F000) >> 8 |
			(color & 0x000000F0) >> 4 );
	}

	//! Convert 16bits A1R5G5B5 color to 32 bits A8R8G8B8 Color
	/** build a nicer 32bit Color by extending dest lower bits with source high bits. */
	inline uint32 A1R5G5B5toA8R8G8B8(uint16 color)
	{
		return ( (( -( (int32) color & 0x00008000 ) >> (int32) 31 ) & 0xFF000000 ) |
				(( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
				(( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
				(( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
				);
	}

	//! Convert 16bits A4R4G4B4 color to 32 bits A8R8G8B8 Color
	inline uint32 A4R4G4B4toA8R8G8B8(uint16 color)
	{
		return ((color & 0xF000) << 16) |
			((color & 0x0F00) << 12) |
			((color & 0x00F0) << 8) |
			((color & 0x000F) << 4);
	}

	//! Convert 16bits R5G6B5 color to 32 bits A8R8G8B8 Color 
	inline uint32 R5G6B5toA8R8G8B8(uint16 color)
	{
		return 0xFF000000 |
			((color & 0xF800) << 8)|
			((color & 0x07E0) << 5)|
			((color & 0x001F) << 3);
	}

	//! Convert 16bits R5G6B5 color to 16 bits A1R5G5B5 Color
	inline uint16 R5G6B5toA1R5G5B5(uint16 color)
	{
		return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
	}

	//! Convert 16 bits A1R5G5B5 color to 16 bits R5G6B5 Color
	inline uint16 A1R5G5B5toR5G6B5(uint16 color)
	{
		return (((color & 0x7FE0) << 1) | (color & 0x1F));
	}

	//! a more useful memset for pixel
	// (standard memset only works with 8-bit values)
	inline void memset16(void * dest, const uint16 value, uint32 bytesize)
	{
		uint16 * d = (uint16*) dest;

		uint32 i;

		// loops unrolled to reduce the number of increments by factor ~8.
		i = bytesize >> (1 + 3);
		while (i)
		{
			d[0] = value;
			d[1] = value;
			d[2] = value;
			d[3] = value;

			d[4] = value;
			d[5] = value;
			d[6] = value;
			d[7] = value;

			d += 8;
			--i;
		}

		i = (bytesize >> 1 ) & 7;
		while (i)
		{
			d[0] = value;
			++d;
			--i;
		}
	}

	//! a more useful memset for pixel
	// (standard memset only works with 8-bit values)
	inline void memset32(void * dest, const uint32 value, uint32 bytesize)
	{
		uint32 * d = (uint32*) dest;

		uint32 i;

		// loops unrolled to reduce the number of increments by factor ~8.
		i = bytesize >> (2 + 3);
		while (i)
		{
			d[0] = value;
			d[1] = value;
			d[2] = value;
			d[3] = value;

			d[4] = value;
			d[5] = value;
			d[6] = value;
			d[7] = value;

			d += 8;
			i -= 1;
		}

		i = (bytesize >> 2 ) & 7;
		while (i)
		{
			d[0] = value;
			d += 1;
			i -= 1;
		}
	}

	typedef union { uint32 u; int32 s; float f; } inttofloat;

	// integer log2 of a float ieee 754. TODO: non ieee floating point
	inline int32 int32_log2_float( float f )
	{
		inttofloat tmp; 
		tmp.f = f;
		uint32 x = tmp.u;
		return ((x & 0x7F800000) >> 23) - 127;
	}

	inline int32 int32_log2_int32(uint32 x)
	{
		return int32_log2_float( (float) x);
	}

#endif		// __SGP_PIXELFORMAT_HEADER__