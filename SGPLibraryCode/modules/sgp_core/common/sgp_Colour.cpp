namespace ColourHelpers
{
    static uint8 floatToUInt8 (const float n) noexcept
    {
        return n <= 0.0f ? 0 : (n >= 1.0f ? 255 : (uint8) (n * 255.0f));
    }
}

Colour BLACKCOLOR(0,0,0);
Colour WHITECOLOR(255,255,255);
Colour REDCOLOR(255,0,0);
Colour GREENCOLOR(0,255,0);
Colour BLUECOLOR(0,0,255);

//==============================================================================
Colour::Colour() noexcept
    : red(0), green(0), blue(0), alpha(0)
{
}

Colour::Colour (const Colour& other) noexcept
    : red(other.red), green(other.green), blue(other.blue), alpha(other.alpha)
{
}

Colour& Colour::operator= (const Colour& other) noexcept
{
    red = other.red;
	green = other.green;
	blue = other.blue;
	alpha = other.alpha;
    return *this;
}

bool Colour::operator== (const Colour& other) const noexcept    
{ 
	return (red == other.red) && (green == other.green) && (blue == other.blue) &&
		(alpha == other.alpha); 
}
bool Colour::operator!= (const Colour& other) const noexcept 
{ 
	return (red != other.red) || (green != other.green) || (blue != other.blue) ||
		(alpha != other.alpha); 
}

//==============================================================================
Colour::Colour (const uint32 _argb) noexcept
{
	alpha = uint8((_argb & 0xFF000000) >> 24);
	red = uint8((_argb & 0x00FF0000) >> 16);
	green = uint8((_argb & 0x0000FF00) >> 8);
	blue = uint8(_argb & 0x000000FF);
}

Colour::Colour (const uint8 _red, const uint8 _green, const uint8 _blue) noexcept
{
	alpha = 0xff;
	red = _red;
	green = _green;
	blue = _blue;
}

Colour Colour::fromRGB (const uint8 _red, const uint8 _green, const uint8 _blue) noexcept
{
    return Colour (_red, _green, _blue);
}

Colour::Colour (const uint8 _red, const uint8 _green, const uint8 _blue, const uint8 _alpha) noexcept
{
	red = _red;
	green = _green;
	blue = _blue;
	alpha = _alpha;
}

Colour Colour::fromRGBA (const uint8 _red, const uint8 _green, const uint8 _blue, const uint8 _alpha) noexcept
{
    return Colour (_red, _green, _blue, _alpha);
}

Colour::Colour (const uint8 _red, const uint8 _green, const uint8 _blue, const float _alpha) noexcept
{
    alpha = ColourHelpers::floatToUInt8 (_alpha);
	red = _red;
	green = _green;
	blue = _blue;
}

Colour Colour::fromFloatRGBA (const float _red, const float _green, const float _blue, const float _alpha) noexcept
{
    return Colour (ColourHelpers::floatToUInt8 (_red), ColourHelpers::floatToUInt8 (_green), ColourHelpers::floatToUInt8 (_blue), _alpha);
}

Colour::~Colour() noexcept
{
}

float Colour::getFloatRed() const noexcept      { return getRed()   / 255.0f; }
float Colour::getFloatGreen() const noexcept    { return getGreen() / 255.0f; }
float Colour::getFloatBlue() const noexcept     { return getBlue()  / 255.0f; }
float Colour::getFloatAlpha() const noexcept    { return getAlpha() / 255.0f; }

uint32 Colour::getARGB() const noexcept
{
    return uint32((alpha << 24) | (red << 16) | (green << 8) | blue);
}
uint32 Colour::getRGBA() const noexcept
{
	return uint32((red << 24) | (green << 16) | (blue << 8) | alpha);
}

//==============================================================================
bool Colour::isTransparent() const noexcept
{
    return getAlpha() == 0;
}

bool Colour::isOpaque() const noexcept
{
    return getAlpha() == 0xff;
}

void Colour::setAlpha (const uint8 newAlpha) noexcept
{
    alpha = newAlpha;
}

void Colour::setAlpha (const float newAlpha) noexcept
{
    jassert (newAlpha >= 0 && newAlpha <= 1.0f);

    alpha = ColourHelpers::floatToUInt8 (newAlpha);
}

//==============================================================================
Colour Colour::greyLevel (const float brightness) noexcept
{
    const uint8 level = ColourHelpers::floatToUInt8 (brightness);
    return Colour (level, level, level);
}

//==============================================================================
String Colour::toString() const
{
    return String::toHexString ((int)getARGB());
}

Colour Colour::fromString (const String& encodedColourString)
{
    return Colour ((uint32) encodedColourString.getHexValue32());
}

String Colour::toDisplayStringRGBA (const bool includeAlphaValue) const
{
    return String::toHexString ((int) (getRGBA() & (includeAlphaValue ? 0xffffffff : 0xffffff00)))
                  .dropLastCharacters ( includeAlphaValue ? 0 : 2)
                  .toUpperCase();
}

String Colour::toDisplayStringARGB (const bool includeAlphaValue) const
{
    return String::toHexString ((int) (getARGB() & (includeAlphaValue ? 0xffffffff : 0x00ffffff)))
                  .paddedLeft ('0', includeAlphaValue ? 8 : 6)
                  .toUpperCase();
}