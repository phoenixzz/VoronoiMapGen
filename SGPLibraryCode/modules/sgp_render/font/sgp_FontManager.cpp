#if SGP_WINDOWS && SGP_MSVC
#if SGP_DEBUG
  #pragma comment(lib, "../../OtherLib/FreeType/lib/freetype2411MT_D.lib")
#else
  #pragma comment(lib, "../../OtherLib/FreeType/lib/freetype2411MT.lib")
#endif
#endif



ISGPFontManager::ISGPFontManager(ISGPRenderDevice* pDevice) : m_pDevice(pDevice), m_ActiveFont(NULL)
{
	if( FT_Init_FreeType(&m_FTLib) != 0 )
	{
		jassertfalse;
		Logger::getCurrentLogger()->writeToLog(String("Initialize Free Type Fail"), ELL_ERROR);
	}
}

ISGPFontManager::~ISGPFontManager()
{
	Release();
}

void ISGPFontManager::Release(void)
{
	for( int i=0; i<m_FontList.size(); i++ )
	{
		SGP_TTFFont* pFont = m_FontList[i];
		pFont->UnloadFont();
		delete pFont;
		pFont = NULL;
	}

	FT_Done_FreeType(m_FTLib);
}

void ISGPFontManager::FlushAllFonts()
{
	for( int i=0; i<m_FontList.size(); i++ )
		m_FontList[i]->FlushAllVB();
}

bool ISGPFontManager::AddFont(const char* strName, const String& strPath, uint16 iSize, bool bBold, bool bItalic)
{
	if(iSize < 5)
		return false; // too small

	if(IsFontLoaded(strName))
		return true;

	SGP_TTFFont* ptrNewFont = m_pDevice->CreateTTFFont( m_FTLib );
	if( !ptrNewFont->LoadTTFFontFromDisk(strName, strPath, iSize, bBold, bItalic) )
	{
		Logger::getCurrentLogger()->writeToLog(String("error: Can't Load TTFFont from disk!"), ELL_ERROR);
		delete ptrNewFont;
		ptrNewFont = NULL;
		return false;
	}

	m_FontList.add( ptrNewFont );
	
	return true;
}

bool ISGPFontManager::IsFontLoaded(const char* strName)
{
	return GetFontByName(strName) != NULL;
}

SGP_TTFFont* ISGPFontManager::GetFontByName(const char* strName)
{
	for( int i=0; i<m_FontList.size(); i++ )
	{
		if( strcmp( strName, m_FontList[i]->GetFontName() ) == 0 )
			return m_FontList[i];
	}
	return NULL;
}

void ISGPFontManager::UnloadFont(const char* strName)
{
	int i = 0;
	for( i=0; i<m_FontList.size(); i++ )
	{
		if( strcmp( strName, m_FontList[i]->GetFontName() ) == 0 )
		{
			m_FontList[i]->UnloadFont();
			delete m_FontList[i];
			break;
		}
	}
	if( i != m_FontList.size() )
		m_FontList.remove(i);
}

int ISGPFontManager::GetActiveFontHeight()
{
	if(m_ActiveFont)
		return m_ActiveFont->GetFontHeight();

	return 0;
}

void ISGPFontManager::SetActiveFont( const char* strName )
{
	m_ActiveFont = GetFontByName(strName);
}

void ISGPFontManager::SetActiveFontHotSpot( float HotSpotx, float HotSpoty )
{
	if(m_ActiveFont)
	{
		m_ActiveFont->SetXHotspot(HotSpotx);
		m_ActiveFont->SetYHotspot(HotSpoty);
	}
}

void ISGPFontManager::SetActiveFontScale(float sx, float sy)
{
	if(m_ActiveFont)
	{
		m_ActiveFont->SetXScale(sx);
		m_ActiveFont->SetYScale(sy);
	}
}

int32 ISGPFontManager::GetFitHeightTextIndex(const String& sText, float startPosx, float endPosx, float xOffset, float totalHeight,
	float fFontSize, bool bKerning, float lineSpacing, SGP_TTFFont::AlignFont align)
{
	return m_ActiveFont->GetFitHeightTextIndex(sText, startPosx, endPosx, xOffset, totalHeight, fFontSize, bKerning, lineSpacing, align);
}

uint32 ISGPFontManager::GetTextTotalHeight(const String& sText, float xmin, float xmax, float xOffset,
	float fFontSize, bool bKerning, float lineSpacing, SGP_TTFFont::AlignFont align)
{
	return m_ActiveFont->GetTextTotalHeight(sText, xmin, xmax, xOffset, fFontSize, bKerning, lineSpacing, align);
}

// x,y Rendering text left-upper pos
bool ISGPFontManager::DoDrawTextInPos( const String& sText, float x, float y, Colour FontColor,
	uint32 flag, float fFontSize, bool bKerning, bool bUnderl )
{
	bool ret = false;
	if(m_ActiveFont)
	{
		if( flag & SGPFDL_DRAWTEXT_MASK )
		{
			if( flag & SGPFDL_SHADOW )
				ret |= m_ActiveFont->DwawTextShadowInPos( sText, x, y, Colour(0xFF202020), flag, fFontSize, bKerning, bUnderl );
		}
		ret |= m_ActiveFont->DrawTextInPos( sText, x, y, FontColor, flag, fFontSize, bKerning, bUnderl );
	}

	return ret;
}

bool ISGPFontManager::DoDrawTextAlignWrap( const String& sText, float x, float y, Colour FontColor, uint32 flag,
	float fFontSize, bool bKerning, bool bUnderl, float xPosMax, float xOffset, float lineSpacing, SGP_TTFFont::AlignFont align )
{
	bool ret = false;
	if(m_ActiveFont)
	{
		if( flag & SGPFDL_DRAWTEXT_MASK )
		{
			if( flag & SGPFDL_SHADOW )
				ret |= m_ActiveFont->DwawTextShadowAlignWrap( sText, x, y, Colour(0xFF202020), flag, fFontSize, bKerning, bUnderl, xPosMax, xOffset, lineSpacing, align );
		}
		ret |= m_ActiveFont->DrawTextAlignWrap( sText, x, y, FontColor, flag, fFontSize, bKerning, bUnderl, xPosMax, xOffset, lineSpacing, align );
	}

	return ret;
}

bool ISGPFontManager::DoDwawHorizonLine(float x, float xmax, float y, Colour LineColor)
{
	if (m_ActiveFont)
		return m_ActiveFont->DrawHorizonLineInPos(x, xmax, y, LineColor);
	return false;
}

void ISGPFontManager::PreCacheChar(const String& sText)
{
	if(m_ActiveFont)
		m_ActiveFont->PreCacheChar(sText);
}

