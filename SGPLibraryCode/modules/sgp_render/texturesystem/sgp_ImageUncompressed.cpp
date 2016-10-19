
//! Constructor from raw data
SGPImageUncompressed::SGPImageUncompressed(SGP_PIXEL_FORMAT format, const SDimension2D& size, void* data,
			bool ownForeignMemory)
: Data(0), Size(size), Format(format), DeleteMemory(false)
{
	if( ownForeignMemory )
	{
		Data = (uint8*)0xbadf00d;
		initData();
		Data = (uint8*)data;
	}
	else
	{
		Data = 0;
		initData();
		memcpy(Data, data, Size.Height * Pitch);
	}
}

//! Constructor of empty image
SGPImageUncompressed::SGPImageUncompressed(SGP_PIXEL_FORMAT format, const SDimension2D& size)
:Data(0), Size(size), Format(format), DeleteMemory(false)
{
	initData();
}


//! assumes format and size has been set and creates the rest
void SGPImageUncompressed::initData()
{
	BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;

	// Pitch should be aligned...
	Pitch = BytesPerPixel * Size.Width;

	if( !Data )
	{
		DeleteMemory = true;
		Data = new uint8[Size.Height * Pitch];
	}
}


//! destructor
SGPImageUncompressed::~SGPImageUncompressed()
{
	if( DeleteMemory )
		delete [] Data;
	Data = NULL;
}


//! Returns width and height of image data.
const SDimension2D& SGPImageUncompressed::getDimension() const
{
	return Size;
}


//! Returns bits per pixel.
uint32 SGPImageUncompressed::getBitsPerPixel() const
{
	return getBitsPerPixelFromFormat(Format);
}


//! Returns bytes per pixel
uint32 SGPImageUncompressed::getBytesPerPixel() const
{
	return BytesPerPixel;
}


//! Returns image data size in bytes
uint32 SGPImageUncompressed::getImageDataSizeInBytes() const
{
	return Pitch * Size.Height;
}


//! Returns image data size in pixels
uint32 SGPImageUncompressed::getImageDataSizeInPixels() const
{
	return Size.Width * Size.Height;
}


//! returns mask for red value of a pixel
uint32 SGPImageUncompressed::getRedMask() const
{
	switch(Format)
	{
	case SGPPF_A1R5G5B5:
		return 0x1F<<10;
	case SGPPF_R5G6B5:
		return 0x1F<<11;
	case SGPPF_R8G8B8:
		return 0x00FF0000;
	case SGPPF_A8R8G8B8:
		return 0x00FF0000;
	case SGPPF_A4R4G4B4:
		return 0x0F00;
	default:
		return 0x0;
	}
}


//! returns mask for green value of a pixel
uint32 SGPImageUncompressed::getGreenMask() const
{
	switch(Format)
	{
	case SGPPF_A1R5G5B5:
		return 0x1F<<5;
	case SGPPF_R5G6B5:
		return 0x3F<<5;
	case SGPPF_R8G8B8:
		return 0x0000FF00;
	case SGPPF_A8R8G8B8:
		return 0x0000FF00;
	case SGPPF_A4R4G4B4:
		return 0x00F0;
	default:
		return 0x0;
	}
}


//! returns mask for blue value of a pixel
uint32 SGPImageUncompressed::getBlueMask() const
{
	switch(Format)
	{
	case SGPPF_A1R5G5B5:
		return 0x1F;
	case SGPPF_R5G6B5:
		return 0x1F;
	case SGPPF_R8G8B8:
		return 0x000000FF;
	case SGPPF_A8R8G8B8:
		return 0x000000FF;
	case SGPPF_A4R4G4B4:
		return 0x000F;
	default:
		return 0x0;
	}
}


//! returns mask for alpha value of a pixel
uint32 SGPImageUncompressed::getAlphaMask() const
{
	switch(Format)
	{
	case SGPPF_A1R5G5B5:
		return 0x1<<15;
	case SGPPF_R5G6B5:
		return 0x0;
	case SGPPF_R8G8B8:
		return 0x0;
	case SGPPF_A8R8G8B8:
		return 0xFF000000;
	case SGPPF_A4R4G4B4:
		return 0xF000;
	default:
		return 0x0;
	}
}


//! sets a pixel
void SGPImageUncompressed::setPixel(uint32 x, uint32 y, const Colour &color, bool blend)
{
	if (x >= Size.Width || y >= Size.Height)
		return;

	switch(Format)
	{
		case SGPPF_A1R5G5B5:
		{
			uint16 * dest = (uint16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toA1R5G5B5( color.getARGB() );
		} break;

		case SGPPF_R5G6B5:
		{
			uint16 * dest = (uint16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toR5G6B5( color.getARGB() );
		} break;

		case SGPPF_A4R4G4B4:
		{
			uint16 * dest = (uint16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = A8R8G8B8toA4R4G4B4( color.getARGB() );
		} break;

		case SGPPF_R8G8B8:
		{
			uint8* dest = Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = (uint8)color.getRed();
			dest[1] = (uint8)color.getGreen();
			dest[2] = (uint8)color.getBlue();
		} break;

		case SGPPF_A8R8G8B8:
		{
			uint32 * dest = (uint32*) (Data + ( y * Pitch ) + ( x << 2 ));
			*dest = blend ? PixelBlend32 ( *dest, color.getARGB() ) : color.getARGB();
		} break;
        default:
            break;
	}
}


//! returns a pixel
Colour SGPImageUncompressed::getPixel(uint32 x, uint32 y) const
{
	if (x >= Size.Width || y >= Size.Height)
		return Colour(0);

	uint32 _argb = 0;
	switch(Format)
	{
	case SGPPF_A1R5G5B5:
		_argb = A1R5G5B5toA8R8G8B8(((uint16*)Data)[y*Size.Width + x]);
		return Colour(_argb);
	case SGPPF_R5G6B5:
		_argb = R5G6B5toA8R8G8B8(((uint16*)Data)[y*Size.Width + x]);
		return Colour(_argb);
	case SGPPF_A4R4G4B4:
		_argb = A4R4G4B4toA8R8G8B8(((uint16*)Data)[y*Size.Width + x]);
		return Colour(_argb);
	case SGPPF_A8R8G8B8:
		_argb = ((uint32*)Data)[y*Size.Width + x];
		return Colour(_argb);
	case SGPPF_R8G8B8:
		{
			uint8* p = Data+(y*3)*Size.Width + (x*3);
			return Colour((uint8)p[0],(uint8)p[1],(uint8)p[2],(uint8)255);
		}
    default:
        break;
	}

	return Colour(0);
}


//! returns the color format
SGP_PIXEL_FORMAT SGPImageUncompressed::getColorFormat() const
{
	return Format;
}

//! fills the surface with given color
void SGPImageUncompressed::fill(const Colour &color)
{
	uint32 c;

	switch ( Format )
	{
		case SGPPF_A1R5G5B5:
			c = A8R8G8B8toA1R5G5B5( color.getARGB() );
			c |= c << 16;
			break;
		case SGPPF_R5G6B5:
			c = A8R8G8B8toR5G6B5( color.getARGB() );
			c |= c << 16;
			break;
		case SGPPF_A4R4G4B4:
			c = A8R8G8B8toA4R4G4B4( color.getARGB() );
			c |= c << 16;
			break;
		case SGPPF_A8R8G8B8:
			c = color.getARGB();
			break;
		case SGPPF_R8G8B8:
		{
			uint8 rgb[3];
			rgb[0] = color.getRed();
			rgb[1] = color.getGreen();
			rgb[2] = color.getBlue();
			const uint32 size = getImageDataSizeInBytes();
			for (uint32 i=0; i<size; i+=3)
			{
				memcpy(Data+i, rgb, 3);
			}
			return;
		}
		break;
		default:
		// TODO: Handle other formats
			return;
	}
	memset32( Data, c, getImageDataSizeInBytes() );
}


//! get a filtered pixel
inline Colour SGPImageUncompressed::getPixelBox( int32 x, int32 y, int32 fx, int32 fy, int32 bias ) const
{
	Colour c;
	int32 a = 0, r = 0, g = 0, b = 0;

	for ( int32 dx = 0; dx != fx; ++dx )
	{
		for ( int32 dy = 0; dy != fy; ++dy )
		{
			c = getPixel(	jmin ( x + dx, int32(Size.Width - 1) ) ,
							jmin ( y + dy, int32(Size.Height - 1) )
						);

			a += c.getAlpha();
			r += c.getRed();
			g += c.getGreen();
			b += c.getBlue();
		}

	}

	int32 sdiv = int32_log2_int32(fx * fy);

	a = jlimit( 0, 255, ( a >> sdiv ) + bias );
	r = jlimit( 0, 255, ( r >> sdiv ) + bias );
	g = jlimit( 0, 255, ( g >> sdiv ) + bias );
	b = jlimit( 0, 255, ( b >> sdiv ) + bias );

	c = Colour::fromRGBA( uint8(r), uint8(g), uint8(b), uint8(a) );
	return c;
}

//! copies this surface into another at given position
void SGPImageUncompressed::copyTo(ISGPImage* target, const SDimension2D& pos)
{
	Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0);
}


//! copies this surface partially into another at given position
void SGPImageUncompressed::copyTo(ISGPImage* target, const SDimension2D& pos, const SRect& sourceRect, const SRect* clipRect)
{
	Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
}



//! copies this surface into another, scaling it to the target image size
// note: this is very very slow.
void SGPImageUncompressed::copyToScaling(void* _target, uint32 _width, uint32 _height, SGP_PIXEL_FORMAT _format, uint32 _pitch)
{
	if (!_target || !_width || !_height)
		return;

	const uint32 bpp = getBitsPerPixelFromFormat(_format)/8;
	if (0 == _pitch)
		_pitch = _width * bpp;

	if (Format == _format && Size.Width == _width && Size.Height == _height)
	{
		if (_pitch == Pitch)
		{
			memcpy(_target, Data, _height*_pitch);
			return;
		}
		else
		{
			uint8* tgtpos = (uint8*) _target;
			uint8* srcpos = Data;
			const uint32 bwidth = _width * bpp;
			const uint32 rest = _pitch - bwidth;
			for (uint32 y=0; y<_height; ++y)
			{
				// copy scanline
				memcpy(tgtpos, srcpos, bwidth);
				// clear pitch
				memset(tgtpos+bwidth, 0, rest);
				tgtpos += _pitch;
				srcpos += Pitch;
			}
			return;
		}
	}

	const float sourceXStep = (float)Size.Width / (float)_width;
	const float sourceYStep = (float)Size.Height / (float)_height;
	int32 yval=0, syval=0;
	float sy = 0.0f;
	for (uint32 y=0; y<_height; ++y)
	{
		float sx = 0.0f;
		for (uint32 x=0; x<_width; ++x)
		{
			CColorConverter::convert_viaFormat(Data+ syval + ((int32)sx)*BytesPerPixel, Format, 1, ((uint8*)_target)+ yval + (x*bpp), _format);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=((int32)sy)*Pitch;
		yval+=_pitch;
	}
}


//! copies this surface into another, scaling it to the target image size
// note: this is very very slow.
void SGPImageUncompressed::copyToScaling(ISGPImage* target)
{
	if( !target )
		return;

	const SDimension2D& targetSize = target->getDimension();

	if( (targetSize.Width == Size.Width) && (targetSize.Height == Size.Height) )
	{
		copyTo(target, SDimension2D(0,0));
		return;
	}

	copyToScaling(target->lock(), targetSize.Width, targetSize.Height, target->getColorFormat());
	target->unlock();
}