#ifndef __SGP_OPENGES2LTTFFONT_HEADER__
#define __SGP_OPENGES2LTTFFONT_HEADER__

class SGP_OpenGLES2TTFFont : public SGP_TTFFont
{
public:
	SGP_OpenGLES2TTFFont( FT_Library& ftlib, COpenGLES2RenderDevice *pRenderDevice );
	virtual ~SGP_OpenGLES2TTFFont();

	// load a TTF font from disk file
	virtual bool LoadTTFFontFromDisk( const char* strname, const String& strpath, 
		uint16 iSize, bool bBold, bool bItalic );

	// unload the TTF font and free all variables
	virtual void UnloadFont(void);

	// is the character cached?
	virtual bool IsCharCached(wchar_t charCode);

	// clear all the cache entries
	virtual void ClearAllCache(void);

	// char pre-cache
	virtual void PreCacheChar(const String& sText);

	// flush all font vertex buffer
	virtual void FlushAllVB(void);

	// draw a string
	virtual bool DrawTextInPos(	const String& sText, float x, float y, Colour FontColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl );
	virtual bool DwawTextShadowInPos( const String& sText, float x, float y, Colour FontShadowColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl );

	virtual bool DrawTextAlignWrap(const String& sText, float x, float y, Colour FontColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl, float xmax, float xOffset = 0, float lineSpacing = 0, AlignFont align = AlignFont_Left) {}
	virtual bool DwawTextShadowAlignWrap(const String& sText, float x, float y, Colour FontShadowColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl, float xmax, float xOffset = 0, float lineSpacing = 0, AlignFont align = AlignFont_Left) {}
	
	// draw a horizon line
	virtual bool DrawHorizonLineInPos(float fXStart, float fXEnd, float fY, Colour color);

	virtual int32 GetFitHeightTextIndex(const String& sText, float xmin, float xmax, float xOffset, 
		float totalHeight, float fFontSize, bool bKerning, float lineSpacing, AlignFont align) {}

	virtual uint32 GetTextTotalHeight(const String& sText, float xmin, float xmax, float xOffset,
		float fFontSize, bool bKerning, float lineSpacing, AlignFont align) {}

	// get the font name 
	virtual const char* GetFontName(void) { return m_strFontName; }

	// In typography, kerning is the process of adjusting the spacing 
	// between characters in a proportional font.
	// Kerning adjusts the space between individual letter forms
	// does font face has kerning
	virtual bool IsKerningSupportedByFace() { return m_bHasKerning; }

	// set x scale when rendering
	virtual void SetXScale(float Scale) { m_fXScale = Scale; }

	// set y scale when rendering
	virtual void SetYScale(float Scale) { m_fYScale = Scale; }

	// set x/y scale when rendering
	virtual void SetScale(float Scale) { m_fXScale = m_fYScale = Scale; }

	// set x hotspot when rendering
	virtual void SetXHotspot(float spot) { m_fXHotSpot = spot; }

	// set y hotspot when rendering
	virtual void SetYHotspot(float spot) { m_fYHotSpot = spot; }

	// set x/y hotspot when rendering
	virtual void SetHotspot(float spot) { m_fXHotSpot = m_fYHotSpot = spot; }

	//
	virtual int GetFontHeight() { return m_CharHeight; }

private:
	//Number of spaces in a tab
	static const unsigned int nTabSize = 4;						// "TAB" size
	static const unsigned int nFontNameStrLen = 256;			// Font file name max length
	static const unsigned int SGPFontTextureSize = 256;			// 256*256 pixel texture size
	static const unsigned int INIT_FONTCACHE_ARRAY_NUM = 1024;	// Max 1024 font cache

	char m_strFontName[nFontNameStrLen];			// font name



private:
	// Struct for every cached character
	// Finally character is cached in an IND_Surface for rendering
	struct CharCacheNode
	{
		wchar_t		charCode;		// wide-byte char value
		uint32		charAdvance;	// advance value

		int32		charOffsetX;	// left offset of the glyph in the image
		int32		charOffsetY;	// top offset of the glyph in the image

		uint16		iPrev, iNext;	// prev OR next CacheNode Index (according frequency)
	};

	ArrayStack<CharCacheNode>			m_FontCharCache;		// character cache list
	uint16								m_CharCacheMap[65536];	// Map of wchar_t => uint16 Index in m_FontCharCache List
	Array<uint32>						m_CharCacheTexture;		// Font cache Texture ID
	Array<COpenGLES2FontBuffer*>		m_FontVB;				// Font Render VB & IB
	uint16				m_LastCacheIndex;		// Font will be replaced(frequency is low)
	uint16				m_FirstCacheIndex;		// First Font in Cache(frequency is high)
	int					m_SpaceAdvance;

	COpenGLES2RenderDevice* m_pRenderDevice;		// SGP Render Device

	FT_Library			m_FTLib;				// freetype lib
	FT_Face				m_Face;					// THIS font face
	FT_Byte*			m_FontDataInMemory;		// Font file Data in Memory
	float				m_fFaceAscender;


	
	uint16				m_CharWidth;			// current font width
	uint16				m_CharHeight;			// current font height
	uint16				m_MaxCharInRow;			// max char num of one row in texture 


	bool				m_bHasKerning;			// font face has kerning
	
	bool				m_bBold;				// bold
	bool				m_bItalic;				// italic
	FT_Matrix			m_matItalic;			// transformation matrix for italic

	float				m_fXScale;				// x scale for bliting
	float				m_fYScale;				// y scale for bliting

	float				m_fXHotSpot;			// x hotspot for bliting
	float				m_fYHotSpot;			// y hotspot for bliting

	uint8*				m_pDynamicFontData;		// updated Font Texture data
private:
	SGP_OpenGLES2TTFFont();

	// cache a single char
	bool _BuildCharCache( wchar_t charCode );

	// render a single char
	bool _RenderChar( wchar_t charCode, float x, float y, Colour FontColor, 
		uint32 flag, float fFontSize, bool bKerning, bool bUnderl );

	// render glyph to image
	bool _RenderGlyph(FT_Bitmap* Glyph, uint32 FontTextureID, uint16 x, uint16 y);

	// advance with space 
	int _GetSpaceAdvance();

	// now not use
	void _DoDrawBorder(float fX_s, float fX_e, float fY, Colour color, float fFontSize);

};



#endif		// __SGP_OPENGES2LTTFFONT_HEADER__