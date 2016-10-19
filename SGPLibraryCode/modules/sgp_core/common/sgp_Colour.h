#ifndef __SGP_COLOUR_HEADER__
#define __SGP_COLOUR_HEADER__

//==============================================================================
/**
    Represents a colour, also including a transparency value.

    The colour is stored internally as unsigned 8-bit red, green, blue and alpha values.
*/
class SGP_API  Colour
{
public:
    //==============================================================================
    /** Creates a transparent black colour. */
    Colour() noexcept;

    /** Creates a copy of another Colour object. */
    Colour (const Colour& other) noexcept;

    /** Creates a colour from a 32-bit ARGB value.

        The format of this number is:
            ((alpha << 24) | (red << 16) | (green << 8) | blue).

        All components in the range 0x00 to 0xff.
        An alpha of 0x00 is completely transparent, alpha of 0xff is opaque.

    */
    explicit Colour (const uint32 _argb) noexcept;

    /** Creates an opaque colour using 8-bit red, green and blue values */
    Colour (const uint8 _red,
            const uint8 _green,
            const uint8 _blue) noexcept;

    /** Creates an opaque colour using 8-bit red, green and blue values */
    static Colour fromRGB (const uint8 _red,
                           const uint8 _green,
                           const uint8 _blue) noexcept;

    /** Creates a colour using 8-bit red, green, blue and alpha values. */
    Colour (const uint8 _red,
            const uint8 _green,
            const uint8 _blue,
            const uint8 _alpha) noexcept;

    /** Creates a colour using 8-bit red, green, blue and alpha values. */
    static Colour fromRGBA (const uint8 _red,
                            const uint8 _green,
                            const uint8 _blue,
                            const uint8 _alpha) noexcept;

    /** Creates a colour from 8-bit red, green, and blue values, and a floating-point alpha.

        Alpha of 0.0 is transparent, alpha of 1.0f is opaque.
        Values outside the valid range will be clipped.
    */
    Colour (const uint8 _red,
            const uint8 _green,
            const uint8 _blue,
            const float _alpha) noexcept;

    /** Creates a colour using floating point red, green, blue and alpha values.
        Numbers outside the range 0..1 will be clipped.
    */
    static Colour fromFloatRGBA (const float _red,
                                 const float _green,
                                 const float _blue,
                                 const float _alpha) noexcept;


    /** Destructor. */
    ~Colour() noexcept;

    /** Copies another Colour object. */
    Colour& operator= (const Colour& other) noexcept;

    /** Compares two colours. */
    bool operator== (const Colour& other) const noexcept;
    /** Compares two colours. */
    bool operator!= (const Colour& other) const noexcept;

    //==============================================================================
    /** Returns the red component of this colour.

        @returns a value between 0x00 and 0xff.
    */
    uint8 getRed() const noexcept                       { return red; }

    /** Returns the green component of this colour.

        @returns a value between 0x00 and 0xff.
    */
    uint8 getGreen() const noexcept                     { return green; }

    /** Returns the blue component of this colour.

        @returns a value between 0x00 and 0xff.
    */
    uint8 getBlue() const noexcept                      { return blue; }

    /** Returns the red component of this colour as a floating point value.

        @returns a value between 0.0 and 1.0
    */
    float getFloatRed() const noexcept;

    /** Returns the green component of this colour as a floating point value.

        @returns a value between 0.0 and 1.0
    */
    float getFloatGreen() const noexcept;

    /** Returns the blue component of this colour as a floating point value.

        @returns a value between 0.0 and 1.0
    */
    float getFloatBlue() const noexcept;

    /** Returns a 32-bit integer that represents this colour.

        The format of this number is:
            ((alpha << 24) | (red << 16) | (green << 16) | blue).
    */
    uint32 getARGB() const noexcept;
	/*
	    The format of this number is:
            ( (red << 24) | (green << 16) | (blue << 8) | alpha).
	*/
	uint32 getRGBA() const noexcept;

    //==============================================================================
    /** Returns the colour's alpha (opacity).

        Alpha of 0x00 is completely transparent, 0xff is completely opaque.
    */
    uint8 getAlpha() const noexcept                     { return alpha; }

    /** Returns the colour's alpha (opacity) as a floating point value.

        Alpha of 0.0 is completely transparent, 1.0 is completely opaque.
    */
    float getFloatAlpha() const noexcept;

    /** Returns true if this colour is completely opaque.

        Equivalent to (getAlpha() == 0xff).
    */
    bool isOpaque() const noexcept;

    /** Returns true if this colour is completely transparent.

        Equivalent to (getAlpha() == 0x00).
    */
    bool isTransparent() const noexcept;

    /** Set a colour that's the same colour as this one, but with a new alpha value. */
    void setAlpha (const uint8 newAlpha) noexcept;

    /** Set a colour that's the same colour as this one, but with a new alpha value. */
    void setAlpha (const float newAlpha) noexcept;

    //==============================================================================
    /** Returns an opaque shade of grey.

        @param brightness the level of grey to return - 0 is black, 1.0 is white
    */
    static Colour greyLevel (float brightness) noexcept;

    //==============================================================================
    /** Returns a stringified version of this colour.

        The string can be turned back into a colour using the fromString() method.
    */
    String toString() const;

    /** Reads the colour from a string that was created with toString().
    */
    static Colour fromString (const String& encodedColourString);

    /** Returns the colour as a hex string in the form RRGGBB or RRGGBBAA. */
    String toDisplayStringRGBA (bool includeAlphaValue) const;

    /** Returns the colour as a hex string in the form RRGGBB or AARRGGBB. */
    String toDisplayStringARGB (bool includeAlphaValue) const;

private:
	uint8 red, green, blue, alpha;
};

extern Colour BLACKCOLOR;
extern Colour WHITECOLOR;
extern Colour REDCOLOR;
extern Colour GREENCOLOR;
extern Colour BLUECOLOR;


#endif