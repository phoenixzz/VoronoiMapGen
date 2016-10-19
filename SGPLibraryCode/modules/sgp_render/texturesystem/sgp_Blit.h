#ifndef	__SGP_BLIT_HEADER__
#define __SGP_BLIT_HEADER__

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4505)
#endif

struct AbsRectangle
{
	int32 x0;
	int32 y0;
	int32 x1;
	int32 y1;
};

//! 2D Intersection test
inline bool intersect ( AbsRectangle &dest, const AbsRectangle& a, const AbsRectangle& b)
{
	dest.x0 = jmax( a.x0, b.x0 );
	dest.y0 = jmax( a.y0, b.y0 );
	dest.x1 = jmin( a.x1, b.x1 );
	dest.y1 = jmin( a.y1, b.y1 );
	return dest.x0 < dest.x1 && dest.y0 < dest.y1;
}

struct SBlitJob
{
	AbsRectangle Dest;
	AbsRectangle Source;

	uint32 argb;

	void * src;
	void * dst;

	int32 width;
	int32 height;

	uint32 srcPitch;
	uint32 dstPitch;

	uint32 srcPixelMul;
	uint32 dstPixelMul;

	bool stretch;
	float x_stretch;
	float y_stretch;

	SBlitJob() : stretch(false) {}
};




/*
	return alpha in [0-256] Granularity from 32-Bit ARGB
	add highbit alpha ( alpha > 127 ? + 1 )
*/
static inline uint32 extractAlpha(const uint32 c)
{
	return ( c >> 24 ) + ( c >> 31 );
}

/*!
	Scale Color by (1/value)
	value 0 - 256 ( alpha )
*/
inline uint32 PixelLerp32(const uint32 source, const uint32 value)
{
	uint32 srcRB = source & 0x00FF00FF;
	uint32 srcXG = (source & 0xFF00FF00) >> 8;

	srcRB *= value;
	srcXG *= value;

	srcRB >>= 8;
	//srcXG >>= 8;

	srcXG &= 0xFF00FF00;
	srcRB &= 0x00FF00FF;

	return srcRB | srcXG;
}


/*!
*/
static void executeBlit_TextureCopy_x_to_x( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	if (job->stretch)
	{
		const uint32 *src = static_cast<const uint32*>(job->src);
		uint32 *dst = static_cast<uint32*>(job->dst);
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint32*) ( (uint8*) (job->src) + job->srcPitch*src_y );
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint32 src_x = (uint32)(dx*wscale);
				dst[dx] = src[src_x];
			}
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		const uint32 widthPitch = job->width * job->dstPixelMul;
		const void *src = (void*) job->src;
		void *dst = (void*) job->dst;

		for ( uint32 dy = 0; dy != h; ++dy )
		{
			memcpy( dst, src, widthPitch );

			src = (void*) ( (uint8*) (src) + job->srcPitch );
			dst = (void*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}

/*!
*/
static void executeBlit_TextureCopy_32_to_16( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint32 *src = static_cast<const uint32*>(job->src);
	uint16 *dst = static_cast<uint16*>(job->dst);

	if (job->stretch)
	{
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint32*) ( (uint8*) (job->src) + job->srcPitch*src_y );
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint32 src_x = (uint32)(dx*wscale);
				//16 bit Blitter depends on pre-multiplied color
				const uint32 s = PixelLerp32( src[src_x] | 0xFF000000, extractAlpha( src[src_x] ) );
				dst[dx] = A8R8G8B8toA1R5G5B5( s );
			}
			dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		for ( uint32 dy = 0; dy != h; ++dy )
		{
			for ( uint32 dx = 0; dx != w; ++dx )
			{
				//16 bit Blitter depends on pre-multiplied color
				const uint32 s = PixelLerp32( src[dx] | 0xFF000000, extractAlpha( src[dx] ) );
				dst[dx] = A8R8G8B8toA1R5G5B5( s );
			}

			src = (uint32*) ( (uint8*) (src) + job->srcPitch );
			dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}


//! Creates a 16 bit A1R5G5B5 color
inline uint16 RGBA16(uint32 r, uint32 g, uint32 b, uint32 a=0xFF)
{
	return (uint16)((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}
/*!
*/
static void executeBlit_TextureCopy_24_to_16( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint8 *src = static_cast<const uint8*>(job->src);
	uint16 *dst = static_cast<uint16*>(job->dst);

	if (job->stretch)
	{
		const float wscale = 3.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint8*)(job->src) + job->srcPitch*src_y;
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint8* src_x = src+(uint32)(dx*wscale);
				dst[dx] = RGBA16(src_x[0], src_x[1], src_x[2]);
			}
			dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		for ( uint32 dy = 0; dy != h; ++dy )
		{
			const uint8* s = src;
			for ( uint32 dx = 0; dx != w; ++dx )
			{
				dst[dx] = RGBA16(s[0], s[1], s[2]);
				s += 3;
			}

			src = src+job->srcPitch;
			dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}


/*!
*/
static void executeBlit_TextureCopy_16_to_32( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint16 *src = static_cast<const uint16*>(job->src);
	uint32 *dst = static_cast<uint32*>(job->dst);

	if (job->stretch)
	{
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint16*) ( (uint8*) (job->src) + job->srcPitch*src_y );
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint32 src_x = (uint32)(dx*wscale);
				dst[dx] = A1R5G5B5toA8R8G8B8(src[src_x]);
			}
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		for ( uint32 dy = 0; dy != h; ++dy )
		{
			for ( uint32 dx = 0; dx != w; ++dx )
			{
				dst[dx] = A1R5G5B5toA8R8G8B8( src[dx] );
			}

			src = (uint16*) ( (uint8*) (src) + job->srcPitch );
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}

static void executeBlit_TextureCopy_16_to_24( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint16 *src = static_cast<const uint16*>(job->src);
	uint8 *dst = static_cast<uint8*>(job->dst);

	if (job->stretch)
	{
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint16*) ( (uint8*) (job->src) + job->srcPitch*src_y );
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint32 src_x = (uint32)(dx*wscale);
				uint32 color = A1R5G5B5toA8R8G8B8(src[src_x]);
				uint8 * writeTo = &dst[dx * 3];
				*writeTo++ = (color >> 16)& 0xFF;
				*writeTo++ = (color >> 8) & 0xFF;
				*writeTo++ = color & 0xFF;
			}
			dst += job->dstPitch;
		}
	}
	else
	{
		for ( uint32 dy = 0; dy != h; ++dy )
		{
			for ( uint32 dx = 0; dx != w; ++dx )
			{
				uint32 color = A1R5G5B5toA8R8G8B8(src[dx]);
				uint8 * writeTo = &dst[dx * 3];
				*writeTo++ = (color >> 16)& 0xFF;
				*writeTo++ = (color >> 8) & 0xFF;
				*writeTo++ = color & 0xFF;
			}

			src = (uint16*) ( (uint8*) (src) + job->srcPitch );
			dst += job->dstPitch;
		}
	}
}

/*!
*/
static void executeBlit_TextureCopy_24_to_32( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint8 *src = static_cast<const uint8*>(job->src);
	uint32 *dst = static_cast<uint32*>(job->dst);

	if (job->stretch)
	{
		const float wscale = 3.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (const uint8*)job->src+(job->srcPitch*src_y);
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint8* s = src+(uint32)(dx*wscale);
				dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
			}
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		for ( int32 dy = 0; dy != job->height; ++dy )
		{
			const uint8* s = src;

			for ( int32 dx = 0; dx != job->width; ++dx )
			{				
				dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
				s += 3;
			}

			src = src + job->srcPitch;
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}

static void executeBlit_TextureCopy_32_to_24( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint32 *src = static_cast<const uint32*>(job->src);
	uint8 *dst = static_cast<uint8*>(job->dst);

	if (job->stretch)
	{
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;

		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint32*) ( (uint8*) (job->src) + job->srcPitch*src_y);

			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint32 src_x = src[(uint32)(dx*wscale)];
				uint8 * writeTo = &dst[dx * 3];
				*writeTo++ = (src_x >> 16)& 0xFF;
				*writeTo++ = (src_x >> 8) & 0xFF;
				*writeTo++ = src_x & 0xFF;
			}
			dst += job->dstPitch;
		}
	}
	else
	{
		for ( uint32 dy = 0; dy != h; ++dy )
		{
			for ( uint32 dx = 0; dx != w; ++dx )
			{
				uint8 * writeTo = &dst[dx * 3];
				*writeTo++ = (src[dx] >> 16)& 0xFF;
				*writeTo++ = (src[dx] >> 8) & 0xFF;
				*writeTo++ = src[dx] & 0xFF;
			}

			src = (uint32*) ( (uint8*) (src) + job->srcPitch );
			dst += job->dstPitch;
		}
	}
}





//! conditional set based on mask and arithmetic shift
inline uint32 if_c_a_else_b ( const int32 condition, const uint32 a, const uint32 b )
{
	return ( ( -condition >> 31 ) & ( a ^ b ) ) ^ b;
}
/*!
*/
static void executeBlit_TextureBlend_16_to_16( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint32 rdx = w>>1;

	const uint32 *src = (uint32*) job->src;
	uint32 *dst = (uint32*) job->dst;

	if (job->stretch)
	{
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;
		const uint32 off = if_c_a_else_b(w&1, (uint32)((w-1)*wscale), 0);
		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint32*) ( (uint8*) (job->src) + job->srcPitch*src_y );
			
			for ( uint32 dx = 0; dx < rdx; ++dx )
			{
				const uint32 src_x = (uint32)(dx*wscale);
				dst[dx] = PixelBlend16_simd( dst[dx], src[src_x] );
			}
			if ( off )
			{
				((uint16*) dst)[off] = PixelBlend16( ((uint16*) dst)[off], ((uint16*) src)[off] );
			}

			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		const uint32 off = if_c_a_else_b(w&1, w-1, 0);
		for (uint32 dy = 0; dy != h; ++dy )
		{
			for (uint32 dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd( dst[dx], src[dx] );
			}

			if ( off )
			{
				((uint16*) dst)[off] = PixelBlend16( ((uint16*) dst)[off], ((uint16*) src)[off] );
			}

			src = (uint32*) ( (uint8*) (src) + job->srcPitch );
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}

/*!
*/
static void executeBlit_TextureBlend_32_to_32( const SBlitJob * job )
{
	const uint32 w = job->width;
	const uint32 h = job->height;
	const uint32 *src = (uint32*) job->src;
	uint32 *dst = (uint32*) job->dst;

	if (job->stretch)
	{
		const float wscale = 1.f/job->x_stretch;
		const float hscale = 1.f/job->y_stretch;
		for ( uint32 dy = 0; dy < h; ++dy )
		{
			const uint32 src_y = (uint32)(dy*hscale);
			src = (uint32*) ( (uint8*) (job->src) + job->srcPitch*src_y );
			
			for ( uint32 dx = 0; dx < w; ++dx )
			{
				const uint32 src_x = (uint32)(dx*wscale);
				dst[dx] = PixelBlend32( dst[dx], src[src_x] );
			}

			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		for ( uint32 dy = 0; dy != h; ++dy )
		{
			for ( uint32 dx = 0; dx != w; ++dx )
			{
				dst[dx] = PixelBlend32( dst[dx], src[dx] );
			}
			src = (uint32*) ( (uint8*) (src) + job->srcPitch );
			dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
		}
	}
}

/*!
*/
static void executeBlit_TextureBlendColor_16_to_16( const SBlitJob * job )
{
	uint16 *src = (uint16*) job->src;
	uint16 *dst = (uint16*) job->dst;

	uint16 blend = A8R8G8B8toA1R5G5B5 ( job->argb );
	for ( int32 dy = 0; dy != job->height; ++dy )
	{
		for ( int32 dx = 0; dx != job->width; ++dx )
		{
			if ( 0 == (src[dx] & 0x8000) )
				continue;

			dst[dx] = PixelMul16_2( src[dx], blend );
		}
		src = (uint16*) ( (uint8*) (src) + job->srcPitch );
		dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
	}
}


/*
	Pixel = c0 * (c1/255).
*/
inline uint32 PixelMul32_2 ( const uint32 c0, const uint32 c1)
{
	return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}
/*!
*/
static void executeBlit_TextureBlendColor_32_to_32( const SBlitJob * job )
{
	uint32 *src = (uint32*) job->src;
	uint32 *dst = (uint32*) job->dst;

	for ( int32 dy = 0; dy != job->height; ++dy )
	{
		for ( int32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], PixelMul32_2( src[dx], job->argb ) );
		}
		src = (uint32*) ( (uint8*) (src) + job->srcPitch );
		dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_Color_16_to_16( const SBlitJob * job )
{
	const uint16 c = A8R8G8B8toA1R5G5B5(job->argb);
	uint16 *dst = (uint16*) job->dst;

	for ( int32 dy = 0; dy != job->height; ++dy )
	{
		memset16(dst, c, job->srcPitch);
		dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_Color_32_to_32( const SBlitJob * job )
{
	uint32 *dst = (uint32*) job->dst;

	for ( int32 dy = 0; dy != job->height; ++dy )
	{
		memset32( dst, job->argb, job->srcPitch );
		dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_16_to_16( const SBlitJob * job )
{
	uint16 *dst = (uint16*) job->dst;

	const uint16 alpha = uint16(extractAlpha( job->argb ) >> 3);
	if ( 0 == alpha )
		return;
	const uint32 src = A8R8G8B8toA1R5G5B5( job->argb );

	for ( int32 dy = 0; dy != job->height; ++dy )
	{
		for ( int32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = 0x8000 | PixelBlend16( dst[dx], src, alpha );
		}
		dst = (uint16*) ( (uint8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_32_to_32( const SBlitJob * job )
{
	uint32 *dst = (uint32*) job->dst;

	const uint32 alpha = extractAlpha( job->argb );
	const uint32 src = job->argb;

	for ( int32 dy = 0; dy != job->height; ++dy )
	{
		for ( int32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = (job->argb & 0xFF000000 ) | PixelBlend32( dst[dx], src, alpha );
		}
		dst = (uint32*) ( (uint8*) (dst) + job->dstPitch );
	}
}




// Blitter Operation
enum eBlitter
{
	BLITTER_INVALID = 0,
	BLITTER_COLOR,
	BLITTER_COLOR_ALPHA,
	BLITTER_TEXTURE,
	BLITTER_TEXTURE_ALPHA_BLEND,
	BLITTER_TEXTURE_ALPHA_COLOR_BLEND
};

typedef void (*tExecuteBlit) ( const SBlitJob * job );


struct blitterTable
{
	eBlitter operation;
	int32 destFormat;
	int32 sourceFormat;
	tExecuteBlit func;
};

static const blitterTable blitTable[] =
{
	{ BLITTER_TEXTURE, -2, -2, executeBlit_TextureCopy_x_to_x },
	{ BLITTER_TEXTURE, SGPPF_A1R5G5B5, SGPPF_A8R8G8B8, executeBlit_TextureCopy_32_to_16 },
	{ BLITTER_TEXTURE, SGPPF_A1R5G5B5, SGPPF_R8G8B8, executeBlit_TextureCopy_24_to_16 },
	{ BLITTER_TEXTURE, SGPPF_A8R8G8B8, SGPPF_A1R5G5B5, executeBlit_TextureCopy_16_to_32 },
	{ BLITTER_TEXTURE, SGPPF_A8R8G8B8, SGPPF_R8G8B8, executeBlit_TextureCopy_24_to_32 },
	{ BLITTER_TEXTURE, SGPPF_R8G8B8, SGPPF_A1R5G5B5, executeBlit_TextureCopy_16_to_24 },
	{ BLITTER_TEXTURE, SGPPF_R8G8B8, SGPPF_A8R8G8B8, executeBlit_TextureCopy_32_to_24 },
	{ BLITTER_TEXTURE_ALPHA_BLEND, SGPPF_A1R5G5B5, SGPPF_A1R5G5B5, executeBlit_TextureBlend_16_to_16 },
	{ BLITTER_TEXTURE_ALPHA_BLEND, SGPPF_A8R8G8B8, SGPPF_A8R8G8B8, executeBlit_TextureBlend_32_to_32 },
	{ BLITTER_TEXTURE_ALPHA_COLOR_BLEND, SGPPF_A1R5G5B5, SGPPF_A1R5G5B5, executeBlit_TextureBlendColor_16_to_16 },
	{ BLITTER_TEXTURE_ALPHA_COLOR_BLEND, SGPPF_A8R8G8B8, SGPPF_A8R8G8B8, executeBlit_TextureBlendColor_32_to_32 },
	{ BLITTER_COLOR, SGPPF_A1R5G5B5, -1, executeBlit_Color_16_to_16 },
	{ BLITTER_COLOR, SGPPF_A8R8G8B8, -1, executeBlit_Color_32_to_32 },
	{ BLITTER_COLOR_ALPHA, SGPPF_A1R5G5B5, -1, executeBlit_ColorAlpha_16_to_16 },
	{ BLITTER_COLOR_ALPHA, SGPPF_A8R8G8B8, -1, executeBlit_ColorAlpha_32_to_32 },
	{ BLITTER_INVALID, -1, -1, 0 }
};


static inline tExecuteBlit getBlitter2( eBlitter operation,const ISGPImage * dest,const ISGPImage * source )
{
	SGP_PIXEL_FORMAT sourceFormat = (SGP_PIXEL_FORMAT) ( source ? source->getColorFormat() : -1 );
	SGP_PIXEL_FORMAT destFormat = (SGP_PIXEL_FORMAT) ( dest ? dest->getColorFormat() : -1 );

	const blitterTable * b = blitTable;

	while ( b->operation != BLITTER_INVALID )
	{
		if ( b->operation == operation )
		{
			if (( b->destFormat == -1 || b->destFormat == destFormat ) &&
				( b->sourceFormat == -1 || b->sourceFormat == sourceFormat ) )
					return b->func;
			else if ( b->destFormat == -2 && ( sourceFormat == destFormat ) )
					return b->func;
		}
		b += 1;
	}
	return 0;
}


// bounce clipping to texture
inline void setClip ( AbsRectangle &out, const SRect *clip,
					 const ISGPImage * tex, int32 passnative )
{
	if ( clip && 0 == tex && passnative )
	{
		out.x0 = clip->Left;
		out.x1 = clip->Right;
		out.y0 = clip->Top;
		out.y1 = clip->Bottom;
		return;
	}

	const int32 w = tex ? tex->getDimension().Width : 0;
	const int32 h = tex ? tex->getDimension().Height : 0;
	if ( clip )
	{
		out.x0 = jlimit ( 0, w, clip->Left );
		out.x1 = jlimit ( out.x0, w, clip->Right );
		out.y0 = jlimit ( 0, h, clip->Top );
		out.y1 = jlimit ( out.y0, h, clip->Bottom );
	}
	else
	{
		out.x0 = 0;
		out.y0 = 0;
		out.x1 = w;
		out.y1 = h;
	}

}



/*!
	a generic 2D Blitter
*/
static int32 Blit(eBlitter operation,
		ISGPImage * dest,
		const SRect *destClipping,
		const SDimension2D *destPos,
		ISGPImage * const source,
		const SRect *sourceClipping,
		uint32 argb)
{
	if (!dest || !source)
		return 0;

	tExecuteBlit blitter = getBlitter2( operation, dest, source );
	if ( 0 == blitter )
		return 0;

	// Clipping
	AbsRectangle sourceClip;
	AbsRectangle destClip;
	AbsRectangle v;

	SBlitJob job;

	setClip ( sourceClip, sourceClipping, source, 1 );
	setClip ( destClip, destClipping, dest, 0 );

	v.x0 = destPos ? destPos->Width : 0;
	v.y0 = destPos ? destPos->Height : 0;
	v.x1 = v.x0 + ( sourceClip.x1 - sourceClip.x0 );
	v.y1 = v.y0 + ( sourceClip.y1 - sourceClip.y0 );

	if ( !intersect( job.Dest, destClip, v ) )
		return 0;

	job.width = job.Dest.x1 - job.Dest.x0;
	job.height = job.Dest.y1 - job.Dest.y0;

	job.Source.x0 = sourceClip.x0 + ( job.Dest.x0 - v.x0 );
	job.Source.x1 = job.Source.x0 + job.width;
	job.Source.y0 = sourceClip.y0 + ( job.Dest.y0 - v.y0 );
	job.Source.y1 = job.Source.y0 + job.height;

	job.argb = argb;

	if ( source )
	{
		job.srcPitch = source->getPitch();
		job.srcPixelMul = source->getBytesPerPixel();
		job.src = (void*) ( (uint8*) source->lock() + ( job.Source.y0 * job.srcPitch ) + ( job.Source.x0 * job.srcPixelMul ) );
	}
	else
	{
		// use srcPitch for color operation on dest
		job.srcPitch = job.width * dest->getBytesPerPixel();
	}

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = dest->getBytesPerPixel();
	job.dst = (void*) ( (uint8*) dest->lock() + ( job.Dest.y0 * job.dstPitch ) + ( job.Dest.x0 * job.dstPixelMul ) );

	blitter( &job );

	if ( source )
		source->unlock();

	if ( dest )
		dest->unlock();

	return 1;
}




#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_BLIT_HEADER__