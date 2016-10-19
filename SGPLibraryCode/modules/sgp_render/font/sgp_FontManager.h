#ifndef __SGP_FONTMANAGER_HEADER__
#define __SGP_FONTMANAGER_HEADER__

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ttunpat.h>
#include <freetype/ftoutln.h>


class ISGPFontManager
{
public:

	ISGPFontManager(ISGPRenderDevice* pDevice);
	~ISGPFontManager();

	void Release(void);

	bool AddFont( const char* strName, const String& strPath, 
		uint16 iSize = 20,	bool bBold = false, bool bItalic = false );

	bool DoDrawTextInPos( const String& sText, float x, float y,
						  Colour FontColor, uint32 flag, float fFontSize, 
						  bool bKerning, bool bUnderl );
	bool DoDrawTextAlignWrap( const String& sText, float x, float y,
						  Colour FontColor, uint32 flag, float fFontSize, 
						  bool bKerning, bool bUnderl, float xPosMax, float xOffset, float lineSpacing, SGP_TTFFont::AlignFont align );
	
	bool DoDwawHorizonLine(float x, float xmax, float y, Colour LineColor);

	int32 GetFitHeightTextIndex(const String& sText, float startPosx, float endPosx, float xOffset,
		float totalHeight, float fFontSize, bool bKerning, float lineSpacing, SGP_TTFFont::AlignFont align);

	uint32 GetTextTotalHeight(const String& sText, float xmin, float xmax, float xOffset,
		float fFontSize, bool bKerning, float lineSpacing, SGP_TTFFont::AlignFont align);

	void PreCacheChar(const String& sText);

	void FlushAllFonts();

	bool IsFontLoaded(const char* strName);
	void UnloadFont(const char* strName);

	void SetActiveFont( const char* strName );
	SGP_TTFFont* GetFontByName(const char* strName);

	int GetActiveFontHeight();
	
	void SetActiveFontHotSpot(float HotSpotx, float HotSpoty);
	void SetActiveFontScale(float sx, float sy);


private:

    ISGPRenderDevice*					m_pDevice;

	Array<SGP_TTFFont*>					m_FontList;

	SGP_TTFFont*						m_ActiveFont;

	FT_Library							m_FTLib;

};

#endif		// __SGP_FONTMANAGER_HEADER__