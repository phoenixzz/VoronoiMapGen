
//! converts a monochrome bitmap to A1R5G5B5 data
void CColorConverter::convert1BitTo16Bit(const uint8* in, int16* out, int32 width, int32 height, int32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (int32 y=0; y<height; ++y)
	{
		int32 shift = 7;
		if (flip)
			out -= width;

		for (int32 x=0; x<width; ++x)
		{
			out[x] = *in>>shift & 0x01 ? 0xffff : 0x8000;

			if ((--shift)<0) // 8 pixel done
			{
				shift=7;
				++in;
			}
		}

		if (shift != 7) // width did not fill last byte
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts a 4 bit palettized image to A1R5G5B5
void CColorConverter::convert4BitTo16Bit(const uint8* in, int16* out, int32 width, int32 height, const int32* palette, int32 linepad, bool flip)
{
	if (!in || !out || !palette)
		return;

	if (flip)
		out += width*height;

	for (int32 y=0; y<height; ++y)
	{
		int32 shift = 4;
		if (flip)
			out -= width;

		for (int32 x=0; x<width; ++x)
		{
			out[x] = X8R8G8B8toA1R5G5B5(palette[(uint8)((*in >> shift) & 0xf)]);

			if (shift==0)
			{
				shift = 4;
				++in;
			}
			else
				shift = 0;
		}

		if (shift == 0) // odd width
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts a 8 bit palettized image into A1R5G5B5
void CColorConverter::convert8BitTo16Bit(const uint8* in, int16* out, int32 width, int32 height, const int32* palette, int32 linepad, bool flip)
{
	if (!in || !out || !palette)
		return;

	if (flip)
		out += width * height;

	for (int32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width; // one line back
		for (int32 x=0; x<width; ++x)
		{
			out[x] = X8R8G8B8toA1R5G5B5(palette[(uint8)(*in)]);
			++in;
		}
		if (!flip)
			out += width;
		in += linepad;
	}
}

//! converts a 8 bit palettized or non palettized image (A8) into R8G8B8
void CColorConverter::convert8BitTo24Bit(const uint8* in, uint8* out, int32 width, int32 height, const uint8* palette, int32 linepad, bool flip)
{
	if (!in || !out )
		return;

	const int32 lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (int32 y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth; // one line back
		for (int32 x=0; x< lineWidth; x += 3)
		{
			if ( palette )
			{
#if defined(SGP_BIG_ENDIAN)
				out[x+0] = palette[ (in[0] << 2 ) + 0];
				out[x+1] = palette[ (in[0] << 2 ) + 1];
				out[x+2] = palette[ (in[0] << 2 ) + 2];
#else
				out[x+0] = palette[ (in[0] << 2 ) + 2];
				out[x+1] = palette[ (in[0] << 2 ) + 1];
				out[x+2] = palette[ (in[0] << 2 ) + 0];
#endif
			}
			else
			{
				out[x+0] = in[0];
				out[x+1] = in[0];
				out[x+2] = in[0];
			}
			++in;
		}
		if (!flip)
			out += lineWidth;
		in += linepad;
	}
}

//! converts a 8 bit palettized or non palettized image (A8) into R8G8B8
void CColorConverter::convert8BitTo32Bit(const uint8* in, uint8* out, int32 width, int32 height, const uint8* palette, int32 linepad, bool flip)
{
	if (!in || !out )
		return;

	const uint32 lineWidth = 4 * width;
	if (flip)
		out += lineWidth * height;

	uint32 x;
	register uint32 c;
	for (uint32 y=0; y < (uint32) height; ++y)
	{
		if (flip)
			out -= lineWidth; // one line back

		if ( palette )
		{
			for (x=0; x < (uint32) width; x += 1)
			{
				c = in[x];
				((uint32*)out)[x] = ((uint32*)palette)[ c ];
			}
		}
		else
		{
			for (x=0; x < (uint32) width; x += 1)
			{
				c = in[x];
#if defined(SGP_BIG_ENDIAN)
				((uint32*)out)[x] = c << 24 | c << 16 | c << 8 | 0x000000FF;
#else
				((uint32*)out)[x] = 0xFF000000 | c << 16 | c << 8 | c;
#endif
			}

		}

		if (!flip)
			out += lineWidth;
		in += width + linepad;
	}
}



//! converts 16bit data to 16bit data
void CColorConverter::convert16BitTo16Bit(const int16* in, int16* out, int32 width, int32 height, int32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (int32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
#if defined(SGP_BIG_ENDIAN)
		for (int32 x=0; x<width; ++x)
			out[x] = ByteOrder::swap((uint16)in[x]);
#else
		memcpy(out, in, width*sizeof(int16));
#endif
		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}



//! copies R8G8B8 24bit data to 24bit data
void CColorConverter::convert24BitTo24Bit(const uint8* in, uint8* out, int32 width, int32 height, int32 linepad, bool flip, bool bgr)
{
	if (!in || !out)
		return;

	const int32 lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (int32 y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth;
		if (bgr)
		{
			for (int32 x=0; x<lineWidth; x+=3)
			{
				out[x+0] = in[x+2];
				out[x+1] = in[x+1];
				out[x+2] = in[x+0];
			}
		}
		else
		{
			memcpy(out,in,lineWidth);
		}
		if (!flip)
			out += lineWidth;
		in += lineWidth;
		in += linepad;
	}
}


#if 0
//! Resizes the surface to a new size and converts it at the same time
//! to an A8R8G8B8 format, returning the pointer to the new buffer.
void CColorConverter::convert16bitToA8R8G8B8andResize(const int16* in, int32* out, int32 newWidth, int32 newHeight, int32 currentWidth, int32 currentHeight)
{
	if (!newWidth || !newHeight)
		return;

	// note: this is very very slow. (i didn't want to write a fast version.
	// but hopefully, nobody wants to convert surfaces every frame.	

	float sourceXStep = (float)currentWidth / (float)newWidth;
	float sourceYStep = (float)currentHeight / (float)newHeight;
	float sy;
	int32 t;

	for (int32 x=0; x<newWidth; ++x)
	{
		sy = 0.0f;

		for (int32 y=0; y<newHeight; ++y)
		{
			t = in[(int32)(((int32)sy)*currentWidth + x*sourceXStep)];
			t = (((t >> 15)&0x1)<<31) |	(((t >> 10)&0x1F)<<19) |
				(((t >> 5)&0x1F)<<11) |	(t&0x1F)<<3;
			out[(int32)(y*newWidth + x)] = t;

			sy+=sourceYStep;
		}
	}
}
#endif


//! copies X8R8G8B8 32 bit data
void CColorConverter::convert32BitTo32Bit(const int32* in, int32* out, int32 width, int32 height, int32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (int32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
#if defined(SGP_BIG_ENDIAN)
		for (int32 x=0; x<width; ++x)
			out[x] = ByteOrder::swap(in[x]);
#else
		memcpy(out, in, width*sizeof(int32));
#endif
		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}



void CColorConverter::convert_A1R5G5B5toR8G8B8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint8 * dB = (uint8 *)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		dB[2] = uint8((*sB & 0x7c00) >> 7);
		dB[1] = uint8((*sB & 0x03e0) >> 2);
		dB[0] = uint8((*sB & 0x1f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_A1R5G5B5toB8G8R8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint8 * dB = (uint8 *)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		dB[0] = uint8((*sB & 0x7c00) >> 7);
		dB[1] = uint8((*sB & 0x03e0) >> 2);
		dB[2] = uint8((*sB & 0x1f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_A1R5G5B5toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint32* dB = (uint32*)dP;

	for (int32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toA8R8G8B8(*sB++);
}

void CColorConverter::convert_A1R5G5B5toA4R4G4B4(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint16* dB = (uint16*)dP;
	for (int32 x = 0; x < sN; ++x)
	{
		uint32 temp32 = A1R5G5B5toA8R8G8B8(*sB++);
		*dB++ = A8R8G8B8toA4R4G4B4(temp32);
	}
}

void CColorConverter::convert_A1R5G5B5toA1R5G5B5(const void* sP, int32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_A1R5G5B5toR5G6B5(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint16* dB = (uint16*)dP;

	for (int32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toR5G6B5(*sB++);
}

void CColorConverter::convert_A8R8G8B8toR8G8B8(const void* sP, int32 sN, void* dP)
{
	uint8* sB = (uint8*)sP;
	uint8* dB = (uint8*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

void CColorConverter::convert_A8R8G8B8toB8G8R8(const void* sP, int32 sN, void* dP)
{
	uint8* sB = (uint8*)sP;
	uint8* dB = (uint8*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

void CColorConverter::convert_A8R8G8B8toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	memcpy(dP, sP, sN * 4);
}

void CColorConverter::convert_A8R8G8B8toA1R5G5B5(const void* sP, int32 sN, void* dP)
{
	uint32* sB = (uint32*)sP;
	uint16* dB = (uint16*)dP;

	for (int32 x = 0; x < sN; ++x)
		*dB++ = A8R8G8B8toA1R5G5B5(*sB++);
}

void CColorConverter::convert_A8R8G8B8toA4R4G4B4(const void* sP, int32 sN, void* dP)
{
	uint32* sB = (uint32*)sP;
	uint16* dB = (uint16*)dP;
	for (int32 x = 0; x < sN; ++x)
		*dB++ = A8R8G8B8toA4R4G4B4(*sB++);
}

void CColorConverter::convert_A8R8G8B8toR5G6B5(const void* sP, int32 sN, void* dP)
{
	uint8 * sB = (uint8 *)sP;
	uint16* dB = (uint16*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		int32 r = sB[2] >> 3;
		int32 g = sB[1] >> 2;
		int32 b = sB[0] >> 3;

		dB[0] = uint16((r << 11) | (g << 5) | (b));

		sB += 4;
		dB += 1;
	}
}

void CColorConverter::convert_A8R8G8B8toR3G3B2(const void* sP, int32 sN, void* dP)
{
	uint8* sB = (uint8*)sP;
	uint8* dB = (uint8*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		uint8 r = sB[2] & 0xe0;
		uint8 g = (sB[1] & 0xe0) >> 3;
		uint8 b = (sB[0] & 0xc0) >> 6;

		dB[0] = (r | g | b);

		sB += 4;
		dB += 1;
	}
}

void CColorConverter::convert_R8G8B8toR8G8B8(const void* sP, int32 sN, void* dP)
{
	memcpy(dP, sP, sN * 3);
}

void CColorConverter::convert_R8G8B8toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	uint8*  sB = (uint8* )sP;
	uint32* dB = (uint32*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 3;
		++dB;
	}
}

void CColorConverter::convert_R8G8B8toA4R4G4B4(const void* sP, int32 sN, void* dP)
{
	uint8*  sB = (uint8* )sP;
	uint16* dB = (uint16*)dP;
	for (int32 x = 0; x < sN; ++x)
	{
		*dB = 0xF000 | (sB[0]<<8) | (sB[1]<<4) | sB[2];

		sB += 3;
		++dB;
	}
}

void CColorConverter::convert_R8G8B8toA1R5G5B5(const void* sP, int32 sN, void* dP)
{
	uint8 * sB = (uint8 *)sP;
	uint16* dB = (uint16*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		int32 r = sB[0] >> 3;
		int32 g = sB[1] >> 3;
		int32 b = sB[2] >> 3;

		dB[0] = uint16((0x8000) | (r << 10) | (g << 5) | (b));

		sB += 3;
		dB += 1;
	}
}

void CColorConverter::convert_B8G8R8toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	uint8*  sB = (uint8* )sP;
	uint32* dB = (uint32*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[2]<<16) | (sB[1]<<8) | sB[0];

		sB += 3;
		++dB;
	}
}

void CColorConverter::convert_B8G8R8A8toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	uint8* sB = (uint8*)sP;
	uint8* dB = (uint8*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		dB[0] = sB[3];
		dB[1] = sB[2];
		dB[2] = sB[1];
		dB[3] = sB[0];

		sB += 4;
		dB += 4;
	}

}

void CColorConverter::convert_R8G8B8toR5G6B5(const void* sP, int32 sN, void* dP)
{
	uint8 * sB = (uint8 *)sP;
	uint16* dB = (uint16*)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		int32 r = sB[0] >> 3;
		int32 g = sB[1] >> 2;
		int32 b = sB[2] >> 3;

		dB[0] = uint16((r << 11) | (g << 5) | (b));

		sB += 3;
		dB += 1;
	}
}

void CColorConverter::convert_R5G6B5toR5G6B5(const void* sP, int32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_R5G6B5toR8G8B8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint8 * dB = (uint8 *)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		dB[0] = uint8((*sB & 0xf800) >> 8);
		dB[1] = uint8((*sB & 0x07e0) >> 3);
		dB[2] = uint8((*sB & 0x001f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_R5G6B5toB8G8R8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint8 * dB = (uint8 *)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		dB[2] = uint8((*sB & 0xf800) >> 8);
		dB[1] = uint8((*sB & 0x07e0) >> 3);
		dB[0] = uint8((*sB & 0x001f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_R5G6B5toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint32* dB = (uint32*)dP;

	for (int32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA8R8G8B8(*sB++);
}

void CColorConverter::convert_R5G6B5toA4R4G4B4(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint16* dB = (uint16*)dP;

	for(int32 x = 0; x < sN; ++x)
	{
		uint32 temp32 = R5G6B5toA8R8G8B8(*sB++);
		*dB++ = A8R8G8B8toA4R4G4B4(temp32);
	}
}

void CColorConverter::convert_R5G6B5toA1R5G5B5(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint16* dB = (uint16*)dP;

	for (int32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA1R5G5B5(*sB++);
}

void CColorConverter::convert_A4R4G4B4toA1R5G5B5(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint16* dB = (uint16*)dP;
	for (int32 x = 0; x < sN; ++x)
		*dB++ = A4R4G4B4toA1R5G5B5(*sB++);
}

void CColorConverter::convert_A4R4G4B4toR5G6B5(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint16* dB = (uint16*)dP;
	for (int32 x = 0; x < sN; ++x)
		*dB++ = A4R4G4B4toR5G6B5(*sB++);
}

void CColorConverter::convert_A4R4G4B4toA4R4G4B4(const void* sP, int32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_A4R4G4B4toA8R8G8B8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint32* dB = (uint32*)dP;

	for (int32 x = 0; x < sN; ++x)
		*dB++ = A4R4G4B4toA8R8G8B8(*sB++);
}

void CColorConverter::convert_A4R4G4B4toR8G8B8(const void* sP, int32 sN, void* dP)
{
	uint16* sB = (uint16*)sP;
	uint8 * dB = (uint8 *)dP;

	for (int32 x = 0; x < sN; ++x)
	{
		dB[0] = uint8((*sB & 0x0F00) >> 8);
		dB[1] = uint8((*sB & 0x00F0) >> 4);
		dB[2] = uint8((*sB & 0x000F));

		sB += 1;
		dB += 3;
	}
}


void CColorConverter::convert_viaFormat(const void* sP, SGP_PIXEL_FORMAT sF, int32 sN,
				void* dP, SGP_PIXEL_FORMAT dF)
{
	switch (sF)
	{
		case SGPPF_A1R5G5B5:
			switch (dF)
			{
				case SGPPF_A1R5G5B5:
					convert_A1R5G5B5toA1R5G5B5(sP, sN, dP);
				break;
				case SGPPF_R5G6B5:
					convert_A1R5G5B5toR5G6B5(sP, sN, dP);
				break;
				case SGPPF_A8R8G8B8:
					convert_A1R5G5B5toA8R8G8B8(sP, sN, dP);
				break;
				case SGPPF_A4R4G4B4:
					convert_A1R5G5B5toA4R4G4B4(sP, sN, dP);
				break;
				case SGPPF_R8G8B8:
					convert_A1R5G5B5toR8G8B8(sP, sN, dP);
				break;

				default:
					break;

			}
		break;
		case SGPPF_R5G6B5:
			switch (dF)
			{
				case SGPPF_A1R5G5B5:
					convert_R5G6B5toA1R5G5B5(sP, sN, dP);
				break;
				case SGPPF_R5G6B5:
					convert_R5G6B5toR5G6B5(sP, sN, dP);
				break;
				case SGPPF_A4R4G4B4:
					convert_R5G6B5toA4R4G4B4(sP, sN, dP);
				break;
				case SGPPF_A8R8G8B8:
					convert_R5G6B5toA8R8G8B8(sP, sN, dP);
				break;
				case SGPPF_R8G8B8:
					convert_R5G6B5toR8G8B8(sP, sN, dP);
				break;
                default:
                break;
			}
		break;
		case SGPPF_A4R4G4B4:
			switch (dF)
			{
				case SGPPF_A1R5G5B5:
					convert_A4R4G4B4toA1R5G5B5(sP, sN, dP);
				break;
				case SGPPF_R5G6B5:
					convert_A4R4G4B4toR5G6B5(sP, sN, dP);
				break;
				case SGPPF_A4R4G4B4:
					convert_A4R4G4B4toA4R4G4B4(sP, sN, dP);
				break;
				case SGPPF_A8R8G8B8:
					convert_A4R4G4B4toA8R8G8B8(sP, sN, dP);
				break;
				case SGPPF_R8G8B8:
					convert_A4R4G4B4toR8G8B8(sP, sN, dP);
				break;
                default:
					break;
			}
		break;
		case SGPPF_A8R8G8B8:
			switch (dF)
			{
				case SGPPF_A1R5G5B5:
					convert_A8R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case SGPPF_R5G6B5:
					convert_A8R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case SGPPF_A4R4G4B4:
					convert_A8R8G8B8toA4R4G4B4(sP, sN, dP);
				break;
				case SGPPF_A8R8G8B8:
					convert_A8R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case SGPPF_R8G8B8:
					convert_A8R8G8B8toR8G8B8(sP, sN, dP);
				break;
                default:
					break;
			}
		break;
		case SGPPF_R8G8B8:
			switch (dF)
			{
				case SGPPF_A1R5G5B5:
					convert_R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case SGPPF_R5G6B5:
					convert_R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case SGPPF_A4R4G4B4:
					convert_R8G8B8toA4R4G4B4(sP, sN, dP);
				break;
				case SGPPF_A8R8G8B8:
					convert_R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case SGPPF_R8G8B8:
					convert_R8G8B8toR8G8B8(sP, sN, dP);
				break;
                default:
					break;
			}
		break;
        default:
            break;
	}
}
