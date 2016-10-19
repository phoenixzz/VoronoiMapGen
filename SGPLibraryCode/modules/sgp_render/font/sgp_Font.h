#ifndef __SGP_FONT_HEADER__
#define __SGP_FONT_HEADER__

class SGP_TTFFont
{
public:
	enum AlignFont	{ AlignFont_Left, AlignFont_Center, AlignFont_Right };

	SGP_TTFFont() {}
	virtual ~SGP_TTFFont() {}


	// load a TTF font from disk file
	virtual bool LoadTTFFontFromDisk( const char* strname, const String& strpath, uint16 iSize, bool bBold, bool bItalic ) = 0;

	// unload the TTF font and free all variables
	virtual void UnloadFont(void) = 0;

	// flush all font vertex buffer
	virtual void FlushAllVB(void) = 0;

	// clear all the cache entries
	virtual void ClearAllCache(void) = 0;

	// draw a string
	virtual bool DrawTextInPos(const String& sText, float x, float y, Colour FontColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl) = 0;
	virtual bool DwawTextShadowInPos(const String& sText, float x, float y, Colour FontShadowColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl) = 0;

	virtual bool DrawTextAlignWrap(const String& sText, float x, float y, Colour FontColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl, float xmax, float xOffset = 0, float lineSpacing = 0, AlignFont align = AlignFont_Left) = 0;
	virtual bool DwawTextShadowAlignWrap(const String& sText, float x, float y, Colour FontShadowColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl, float xmax, float xOffset = 0, float lineSpacing = 0, AlignFont align = AlignFont_Left) = 0;

	// draw a Line
	virtual bool DrawHorizonLineInPos(float fXStart, float fXEnd, float fY, Colour color) = 0;

	// get the last index of texts of fitting text render rect range.
	// if all characters fit the RECT, return -1
	// if none of characters fit the RECT, return 0
	virtual int32 GetFitHeightTextIndex(const String& sText, float xmin, float xmax, float xOffset, float totalHeight,
		float fFontSize, bool bKerning, float lineSpacing, AlignFont align) = 0;

	virtual uint32 GetTextTotalHeight(const String& sText, float xmin, float xmax, float xOffset,
		float fFontSize, bool bKerning, float lineSpacing, AlignFont align) = 0;

	// get the font name 
	virtual const char* GetFontName(void) = 0;

	virtual int GetFontHeight() = 0;

	// set x scale when rendering
	virtual void SetXScale(float Scale) = 0;
	// set y scale when rendering
	virtual void SetYScale(float Scale) = 0;
	// set x/y scale when rendering
	virtual void SetScale(float Scale) = 0;
	// set x hotspot when rendering
	virtual void SetXHotspot(float spot) = 0;
	// set y hotspot when rendering
	virtual void SetYHotspot(float spot) = 0;
	// set x/y hotspot when rendering
	virtual void SetHotspot(float spot) = 0;

	// In typography, kerning is the process of adjusting the spacing 
	// between characters in a proportional font.
	// Kerning adjusts the space between individual letter forms
	// does font face has kerning
	virtual bool IsKerningSupportedByFace() = 0;

	// char pre-cache
	virtual void PreCacheChar(const String& sText) = 0;

	// is the character cached?
	virtual bool IsCharCached(wchar_t charCode) = 0;
};


#endif		// __SGP_FONT_HEADER__