



SGP_OpenGLTTFFont::SGP_OpenGLTTFFont(FT_Library& ftlib, COpenGLRenderDevice *pRenderDevice)
: m_FTLib(ftlib), m_pRenderDevice(pRenderDevice),
m_LastCacheIndex(0),
m_FirstCacheIndex(0),
m_SpaceAdvance(0),
m_Face(0),
m_FontDataInMemory(0),
m_fFaceAscender(0.0f),
m_CharWidth(20),
m_CharHeight(20),
m_MaxCharInRow(0),
m_bHasKerning(false),
m_fXScale(1.0f),
m_fYScale(1.0f),
m_fXHotSpot(0.5f),
m_fYHotSpot(0.5f),
m_bBold(false),
m_bItalic(false),
m_FontPBOID(0),
m_PBOByteSize(0)
{
	m_FontCharCache.Resize(INIT_FONTCACHE_ARRAY_NUM);

	memset( m_CharCacheMap, 0xFFFF, 65536*sizeof(uint16) );

	for( uint16 i=0; i<(uint16)m_FontCharCache.Size(); i++ )
	{
		m_FontCharCache.getReference(i).iPrev = i-1;
		m_FontCharCache.getReference(i).iNext = i+1;
	}
	m_FontCharCache.getReference(m_FontCharCache.Size() - 1).iNext = 0xFFFF;

	m_matItalic.xx = 1 << 16;
	m_matItalic.xy = 0x5800;
	m_matItalic.yx = 0;
	m_matItalic.yy = 1 << 16;
}

SGP_OpenGLTTFFont::~SGP_OpenGLTTFFont(void)
{
	UnloadFont();
}

bool SGP_OpenGLTTFFont::LoadTTFFontFromDisk(	const char* strname, const String& strpath, 
										uint16 iSize, bool bBold, bool bItalic )
{
	UnloadFont();

	ScopedPointer <FileInputStream> FontInputStream( File(strpath).createInputStream() );
	if( FontInputStream == nullptr )
		return false;

	int32 FontFileBytes = (int32)FontInputStream->getTotalLength();
	m_FontDataInMemory = new uint8 [FontFileBytes];
	FontInputStream->read(m_FontDataInMemory, FontFileBytes);

	//create new face
	if( FT_New_Memory_Face(m_FTLib, m_FontDataInMemory, FontFileBytes, 0, &m_Face) != 0 )
		return false;

	if( !m_Face->charmap || !FT_IS_SCALABLE(m_Face) )
	{
		FT_Done_Face(m_Face);
		return false;
	}

	strcpy(m_strFontName, strname);

	if( FT_HAS_KERNING(m_Face) )
		m_bHasKerning = true;

	m_CharWidth	= iSize;
	m_CharHeight = iSize;
	m_MaxCharInRow = (uint16)(SGPFontTextureSize / iSize);

	if( FT_Set_Pixel_Sizes(m_Face, m_CharWidth, m_CharHeight) != 0 )
		return false;

    m_fFaceAscender = (float)m_Face->ascender * (float)m_Face->size->metrics.y_scale * float(1.0f/64.0f) * (1.0f/65536.0f);

	m_bBold = bBold;
	m_bItalic = bItalic;

#if defined(GL_ARB_pixel_buffer_object)
	// create 1 pixel buffer objects, you need to delete them when program exits.
    // glBufferData with NULL pointer reserves only memory space.
	m_PBOByteSize = m_CharWidth * m_CharHeight * sizeof(uint16);
	m_pRenderDevice->extGlGenBuffers(1, &m_FontPBOID);
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_FontPBOID);
	m_pRenderDevice->extGlBufferData(GL_PIXEL_UNPACK_BUFFER, m_PBOByteSize, NULL, GL_STREAM_DRAW);
	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif

	m_SpaceAdvance = _GetSpaceAdvance();

	return true;
}

void SGP_OpenGLTTFFont::ClearAllCache(void)
{
	for( uint16 i=0; i<(uint16)m_FontCharCache.Size(); i++ )
	{
		m_FontCharCache.getReference(i).charCode = 0;

		m_FontCharCache.getReference(i).iPrev = i-1;
		m_FontCharCache.getReference(i).iNext = i+1;
	}
	m_FontCharCache.getReference(m_FontCharCache.Size() - 1).iNext = 0xFFFF;


	memset( m_CharCacheMap, 0xFFFF, 65536*sizeof(uint16) );
}

void SGP_OpenGLTTFFont::UnloadFont(void)
{
	ClearAllCache();

	if( m_Face )
	{
		FT_Done_Face(m_Face);
		m_Face = 0;
	}

	if( m_FontDataInMemory )
	{
		delete [] m_FontDataInMemory;
		m_FontDataInMemory = 0;
	}
	
	for( int i=0; i<m_CharCacheTexture.size(); i++ )
		m_pRenderDevice->GetTextureManager()->unRegisterTextureByID(m_CharCacheTexture[i]);

	m_CharCacheTexture.clear();

	for( int i=0; i<m_FontVB.size(); i++ )
	{
		if( m_FontVB[i] )
			delete m_FontVB[i];
	}
	m_FontVB.clearQuick();

#if defined(GL_ARB_pixel_buffer_object)
	m_pRenderDevice->extGlDeleteBuffers(1, &m_FontPBOID);
	m_FontPBOID = 0;
	m_PBOByteSize = 0;
#endif
}

bool SGP_OpenGLTTFFont::IsCharCached(wchar_t charCode)
{
	return m_CharCacheMap[charCode] != 0xFFFF;
}

bool SGP_OpenGLTTFFont::DrawHorizonLineInPos(float fXStart, float fXEnd, float fY, Colour color)
{
	_DoDrawBorder(fXStart, fXEnd, fY, color, GetFontHeight(), m_fFaceAscender);

	return true;
}

bool SGP_OpenGLTTFFont::DrawTextInPos(const String& sText, float x, float y, Colour FontColor, 
	uint32 flag, float fFontSize, bool bKerning, bool bUnderl)
{
	bool Ret = true;
	float penX = x, penY = y;
	uint32 previousGlyph = 0;
	FT_Vector Delta;
	CharCacheNode* pNode = NULL;
	float scale = fFontSize / m_CharWidth;
	float maxcharOffsetY = 0;


	int StrLength = sText.length();
	float original_Pen_x = x;

	for(int i = 0; i < StrLength; ++i)
	{
		wchar_t singleTex = *(sText.toWideCharPointer() + i);
		//Special cases
		switch( singleTex )
		{
		case L' ':
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		case L'\t': 
			penX += m_SpaceAdvance * nTabSize * scale;	
			previousGlyph = 0; 
			continue;
		case L'\n':	
			// Draw underline
			if( bUnderl )
			{
				_DoDrawBorder(original_Pen_x, penX, penY + m_CharHeight*scale, FontColor, fFontSize, maxcharOffsetY);
			}
			penY += m_CharHeight * scale; 
			penX = x;	
			original_Pen_x = x;
			previousGlyph = 0;
			maxcharOffsetY = 0;
			continue;
		}

		bool bCacheOK = _BuildCharCache( singleTex );
		if (!bCacheOK)
		{
			// not support font Glyph, use L' ' instead
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		}

		pNode = &m_FontCharCache.getReference(m_CharCacheMap[singleTex]);
		if( !pNode )
		{
			Ret = false;
			previousGlyph = 0;
			continue;
		}
		maxcharOffsetY = jmax(maxcharOffsetY, (float)pNode->charOffsetY);

		// glyph index in the font face
		uint32 charGlyphIndex = FT_Get_Char_Index(m_Face, singleTex);

		//Kerning
		if( previousGlyph != 0 && m_bHasKerning && bKerning )
		{			
			FT_Get_Kerning(m_Face, previousGlyph, charGlyphIndex, FT_KERNING_DEFAULT, &Delta);
			penX += Delta.x >> 6;
			penY += Delta.y >> 6;
		}

		if( !_RenderChar(singleTex, penX, penY, FontColor, flag, fFontSize, bKerning, bUnderl) )
			Ret = false;

		penX += pNode->charAdvance * scale;
		previousGlyph = charGlyphIndex;
	}

	// Draw underline
	if( bUnderl && ((penX - original_Pen_x) > 0.1f) )
	{
		_DoDrawBorder(original_Pen_x, penX, penY + m_CharHeight*scale, FontColor, fFontSize, maxcharOffsetY);
	}
	return Ret;
}

bool SGP_OpenGLTTFFont::DwawTextShadowInPos( const String& sText, float x, float y, Colour FontShadowColor, uint32 flag,
	float fFontSize, bool bKerning, bool bUnderl )
{
	float scale = fFontSize / m_CharWidth;
	return DrawTextInPos( sText, x+m_CharWidth*scale*0.2f, y+m_CharHeight*scale*0.2f, FontShadowColor, flag, fFontSize, bKerning, bUnderl );
}

float SGP_OpenGLTTFFont::_GetDrawTextStartPos(const String& sText, float startPosx, float maxPosx,
	float fFontSize, bool bKerning, AlignFont align)
{
	if (align == AlignFont_Left)
	{
		return startPosx;
	}
	else if (align == AlignFont_Right)
	{
		float TextSpace = _CalcTextWidth(sText, fFontSize, bKerning);
		return (TextSpace > maxPosx - startPosx) ? startPosx : maxPosx - TextSpace;
	}
	else if (align == AlignFont_Center)
	{
		float TextSpace = _CalcTextWidth(sText, fFontSize, bKerning);
		return (TextSpace > maxPosx - startPosx) ? startPosx : ((maxPosx - TextSpace) * 0.5f);
	}

	return startPosx;
}

bool SGP_OpenGLTTFFont::DrawTextAlignWrap(const String& sText, float x, float y, Colour FontColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl, float xmax, float xOffset, float lineSpacing, AlignFont align)
{
	bool Ret = true;
	float penX = x, penY = y;
	uint32 previousGlyph = 0;
	FT_Vector Delta;
	CharCacheNode* pNode = NULL;
	float scale = fFontSize / m_CharWidth;
	float maxcharOffsetY = 0;

	if (x + m_SpaceAdvance * nTabSize * scale > xmax)
	{
		Logger::getCurrentLogger()->writeToLog(String("Draw Text Range is too small! Even a \\t character cannot be displayed!"), ELL_ERROR);
		return false;
	}
	if (align == AlignFont_Left && x + xOffset > xmax)
	{
		Logger::getCurrentLogger()->writeToLog(String("Draw Text Range is too small! Text offset will beyond display area!"), ELL_ERROR);
		return false;
	}

	float startPosx = _GetDrawTextStartPos(sText, x, xmax, fFontSize, bKerning, align);
	if (align == AlignFont_Left)
		startPosx += xOffset;

	int StrLength = sText.length();
	float original_Pen_x = startPosx;
	penX = original_Pen_x;

	for(int i = 0; i < StrLength; ++i)
	{
		wchar_t singleTex = *(sText.toWideCharPointer() + i);
		float currentPenX = penX;
		switch( singleTex )
		{
		case L' ':
			currentPenX += m_SpaceAdvance*scale;
			break;
		case L'\t': 
			currentPenX += m_SpaceAdvance * nTabSize * scale;
			break;
		}
		// Need Wrap??
		if (currentPenX > xmax)
		{
			i--;
			singleTex = L'\n';		
		}

		//Special cases
		switch( singleTex )
		{
		case L' ':
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		case L'\t': 
			penX += m_SpaceAdvance * nTabSize * scale;	
			previousGlyph = 0; 
			continue;
		case L'\n':	
			// Draw underline
			if( bUnderl )
			{
				_DoDrawBorder(original_Pen_x, penX, penY + m_CharHeight*scale, FontColor, fFontSize, maxcharOffsetY);
			}

			startPosx = _GetDrawTextStartPos(sText.substring(i+1), x, xmax, fFontSize, bKerning, align);
			penY += m_CharHeight * scale + lineSpacing; 
			penX = startPosx;	
			original_Pen_x = startPosx;
			previousGlyph = 0; 
			maxcharOffsetY = 0;
			continue;
		}

		bool bCacheOK = _BuildCharCache( singleTex );
		if (!bCacheOK)
		{
			// not support font Glyph, use L' ' instead
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		}

		pNode = &m_FontCharCache.getReference(m_CharCacheMap[singleTex]);
		if( !pNode )
		{
			Ret = false;
			previousGlyph = 0;
			continue;
		}

		maxcharOffsetY = jmax(maxcharOffsetY, (float)pNode->charOffsetY);

		// Wrap??
		if (penX + pNode->charAdvance * scale > xmax)
		{
			i--;
			// Draw underline
			if( bUnderl )
			{
				_DoDrawBorder(original_Pen_x, penX, penY + m_CharHeight*scale, FontColor, fFontSize, maxcharOffsetY);
			}

			startPosx = _GetDrawTextStartPos(sText.substring(i+1), x, xmax, fFontSize, bKerning, align);

			penY += m_CharHeight * scale + lineSpacing; 
			penX = startPosx;	
			original_Pen_x = startPosx;
			previousGlyph = 0; 
			maxcharOffsetY = 0;
				
			continue;
		}

		// glyph index in the font face
		uint32 charGlyphIndex = FT_Get_Char_Index(m_Face, singleTex);

		//Kerning
		if( previousGlyph != 0 && m_bHasKerning && bKerning )
		{			
			FT_Get_Kerning(m_Face, previousGlyph, charGlyphIndex, FT_KERNING_DEFAULT, &Delta);
			penX += Delta.x >> 6;
			penY += Delta.y >> 6;
		}

		if( !_RenderChar(singleTex, penX, penY, FontColor, flag, fFontSize, bKerning, bUnderl) )
			Ret = false;

		penX += pNode->charAdvance * scale;
		previousGlyph = charGlyphIndex;
	}

	// Draw underline
	if( bUnderl && ((penX - original_Pen_x) > 0.1f) )
	{
		_DoDrawBorder(original_Pen_x, penX, penY + m_CharHeight*scale, FontColor, fFontSize, maxcharOffsetY);
	}
	return Ret;
}

bool SGP_OpenGLTTFFont::DwawTextShadowAlignWrap(const String& sText, float x, float y, Colour FontShadowColor, uint32 flag,
		float fFontSize, bool bKerning, bool bUnderl, float xmax, float xOffset, float lineSpacing, AlignFont align)
{
	float scale = fFontSize / m_CharWidth;
	return DrawTextAlignWrap( sText, x+m_CharWidth*scale*0.2f, y+m_CharHeight*scale*0.2f, FontShadowColor, flag, fFontSize, bKerning, bUnderl, xmax+m_CharWidth*scale*0.2f, xOffset, lineSpacing, align );
}

// get the last index of "UTF8" texts of fitting text render rect range.
// if all characters fit the RECT, return -1
// if none of characters fit the RECT, return 0
int32 SGP_OpenGLTTFFont::GetFitHeightTextIndex(const String& sText, float xmin, float xmax, float xOffset, float _totalHeight,
	float fFontSize, bool bKerning, float lineSpacing, AlignFont align)
{
	bool WillAddHeight = false;

	CharCacheNode* pNode = NULL;
	uint32 previousGlyph = 0;
	FT_Vector Delta;
	float scale = fFontSize / m_CharWidth;
	if (xmin + m_SpaceAdvance * nTabSize * scale > xmax)
	{
		Logger::getCurrentLogger()->writeToLog(String("Draw Text Range is too small! Even a \\t character cannot be displayed!"), ELL_ERROR);
		return 0;
	}
	if (align == AlignFont_Left && xmin + xOffset > xmax)
	{
		Logger::getCurrentLogger()->writeToLog(String("Draw Text Range is too small! Text offset will beyond display area!"), ELL_ERROR);
		return 0;
	}

	float penX = xmin;
	if (align == AlignFont_Left)
		penX += xOffset;
	int StrLength = sText.length();
	float fTotalHeight = (StrLength > 0) ? (m_CharHeight * scale): 0;
	for(int i = 0; i < StrLength; ++i)
	{
		wchar_t singleTex = *(sText.toWideCharPointer() + i);
		float currentPenX = penX;
		switch( singleTex )
		{
		case L' ':
			currentPenX += m_SpaceAdvance*scale;
			break;
		case L'\t': 
			currentPenX += m_SpaceAdvance * nTabSize * scale;
			break;
		}
		// Need Wrap??
		if (currentPenX > xmax)
		{
			i--;
			singleTex = L'\n';		
		}

		//Special cases
		switch( singleTex )
		{
		case L' ':
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		case L'\t': 
			penX += m_SpaceAdvance * nTabSize * scale;	
			previousGlyph = 0; 
			continue;
		case L'\n':	
			WillAddHeight = true;
			penX = xmin;	
			previousGlyph = 0;
			continue;
		}

		bool bCacheOK = _BuildCharCache( singleTex );
		if (!bCacheOK)
		{
			// not support font Glyph, use L' ' instead
			penX += m_SpaceAdvance*scale;
			previousGlyph = 0; 
			continue;
		}

		pNode = &m_FontCharCache.getReference(m_CharCacheMap[singleTex]);
		if ( !pNode )
		{
			previousGlyph = 0;
			continue;
		}
		if (penX + pNode->charAdvance * scale > xmax)
		{
			i--;
			WillAddHeight = true;
			penX = xmin;

			previousGlyph = 0;
			continue;
		}

		if (WillAddHeight)
		{
			fTotalHeight += m_CharHeight * scale + lineSpacing;
			WillAddHeight = false;
		}

		// glyph index in the font face
		uint32 charGlyphIndex = FT_Get_Char_Index(m_Face, singleTex);

		//Kerning
		if( previousGlyph != 0 && m_bHasKerning && bKerning )
		{			
			FT_Get_Kerning(m_Face, previousGlyph, charGlyphIndex, FT_KERNING_DEFAULT, &Delta);
			penX += Delta.x >> 6;
			fTotalHeight += Delta.y >> 6;
		}

		if (fTotalHeight > _totalHeight)
		{
			return sText.substring(0, i-1).getNumBytesAsUTF8();
		}

		penX += pNode->charAdvance * scale;
		previousGlyph = charGlyphIndex;
	}
	
	return -1;
}

uint32 SGP_OpenGLTTFFont::GetTextTotalHeight(const String& sText, float xmin, float xmax, float xOffset,
	float fFontSize, bool bKerning, float lineSpacing, AlignFont align)
{
	bool WillAddHeight = false;

	CharCacheNode* pNode = NULL;
	uint32 previousGlyph = 0;
	FT_Vector Delta;
	float scale = fFontSize / m_CharWidth;
	if (xmin + m_SpaceAdvance * nTabSize * scale > xmax)
	{
		Logger::getCurrentLogger()->writeToLog(String("Draw Text Range is too small! Even a \\t character cannot be displayed!"), ELL_ERROR);
		return 0;
	}
	if (align == AlignFont_Left && xmin + xOffset > xmax)
	{
		Logger::getCurrentLogger()->writeToLog(String("Draw Text Range is too small! Text offset will beyond display area!"), ELL_ERROR);
		return 0;
	}

	float penX = xmin;
	if (align == AlignFont_Left)
		penX += xOffset;
	int StrLength = sText.length();
	float TotalHeight = (StrLength > 0) ? (m_CharHeight * scale): 0;
	for(int i = 0; i < StrLength; ++i)
	{
		wchar_t singleTex = *(sText.toWideCharPointer() + i);
		float currentPenX = penX;
		switch( singleTex )
		{
		case L' ':
			currentPenX += m_SpaceAdvance*scale;
			break;
		case L'\t': 
			currentPenX += m_SpaceAdvance * nTabSize * scale;
			break;
		}
		// Need Wrap??
		if (currentPenX > xmax)
		{
			i--;
			singleTex = L'\n';		
		}

		//Special cases
		switch( singleTex )
		{
		case L' ':
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		case L'\t': 
			penX += m_SpaceAdvance * nTabSize * scale;	
			previousGlyph = 0; 
			continue;
		case L'\n':	
			WillAddHeight = true;
			penX = xmin;	
			previousGlyph = 0;
			continue;
		}

		bool bCacheOK = _BuildCharCache( singleTex );
		if (!bCacheOK)
		{
			// not support font Glyph, use L' ' instead
			penX += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		}

		pNode = &m_FontCharCache.getReference(m_CharCacheMap[singleTex]);
		if ( !pNode )
		{
			previousGlyph = 0;
			continue;
		}
		if (penX + pNode->charAdvance * scale > xmax)
		{
			i--;
			WillAddHeight = true;
			penX = xmin;

			previousGlyph = 0;
			continue;
		}

		if (WillAddHeight)
		{
			TotalHeight += m_CharHeight * scale + lineSpacing;
			WillAddHeight = false;
		}

		// glyph index in the font face
		uint32 charGlyphIndex = FT_Get_Char_Index(m_Face, singleTex);

		//Kerning
		if( previousGlyph != 0 && m_bHasKerning && bKerning )
		{			
			FT_Get_Kerning(m_Face, previousGlyph, charGlyphIndex, FT_KERNING_DEFAULT, &Delta);
			penX += Delta.x >> 6;
			TotalHeight += Delta.y >> 6;
		}

		penX += pNode->charAdvance * scale;
		previousGlyph = charGlyphIndex;
	}
	
	return (uint32)TotalHeight;
}

void SGP_OpenGLTTFFont::PreCacheChar(const String& sText)
{
	int StrLength = sText.length();
	for(int i = 0; i < StrLength; ++i)
	{
		wchar_t singleTex = *(sText.toWideCharPointer() + i);
		if( (singleTex != L' ') && (singleTex != L'\t') && (singleTex != L'\n') )
			_BuildCharCache(singleTex);
	}
}



bool SGP_OpenGLTTFFont::_BuildCharCache(wchar_t charCode)
{
	static uint16 TexX = 0;
	static uint16 TexY = 0;

	if( IsCharCached(charCode) )
	{
		uint16 CacheIdx = m_CharCacheMap[charCode];

		// if cache index is Head, do none
		if (m_FirstCacheIndex == CacheIdx)
			return true;

		// if cache index is Last one
		if (m_LastCacheIndex == CacheIdx)
		{
			m_LastCacheIndex = m_FontCharCache.getReference(CacheIdx).iPrev;

			m_FontCharCache.getReference(m_LastCacheIndex).iNext = 0xFFFF;
			m_FontCharCache.getReference(CacheIdx).iPrev = 0xFFFF;
			m_FontCharCache.getReference(CacheIdx).iNext = m_FirstCacheIndex;
			m_FontCharCache.getReference(m_FirstCacheIndex).iPrev = CacheIdx;
			m_FirstCacheIndex = CacheIdx;

			return true;
		}

		// move found cache index to Head
		m_FontCharCache.getReference(m_FontCharCache.getReference(CacheIdx).iPrev).iNext = m_FontCharCache.getReference(CacheIdx).iNext;
		m_FontCharCache.getReference(m_FontCharCache.getReference(CacheIdx).iNext).iPrev = m_FontCharCache.getReference(CacheIdx).iPrev;
		m_FontCharCache.getReference(CacheIdx).iPrev = 0xFFFF;
		m_FontCharCache.getReference(CacheIdx).iNext = m_FirstCacheIndex;
		m_FontCharCache.getReference(m_FirstCacheIndex).iPrev = CacheIdx;

		m_FirstCacheIndex = CacheIdx;

		return true;
	}

	CharCacheNode *pNode = &m_FontCharCache.getReference(m_LastCacheIndex);
	//////////
	//init the struct pointers
	m_CharCacheMap[pNode->charCode] = 0xFFFF;
	pNode->charCode = 0;
	//////////
	
	uint32 charGlyphIndex = FT_Get_Char_Index(m_Face, charCode);
	if( charGlyphIndex == 0 )
		return false;

	FT_Load_Char(m_Face, charCode, FT_LOAD_DEFAULT/* | FT_LOAD_NO_BITMAP*/);

	if( m_bBold )
	{
		int strength = 1 << 6;
		FT_Outline_Embolden(&m_Face->glyph->outline, strength);
	}

	if(m_bItalic)
	{
		// set transformation 
		FT_Outline_Transform(&m_Face->glyph->outline, &m_matItalic);
	}


	if( FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL) )
	{
		return false;
	}

	// Cache NOT full, texture has space for new font
	bool bCacheFull = false;
	if( m_FontCharCache.Count() < INIT_FONTCACHE_ARRAY_NUM )
	{
		// One texture is full?
		if( (m_FontCharCache.Count() % (m_MaxCharInRow*m_MaxCharInRow) == 0) &&
			m_CharCacheTexture.size() <= m_FontCharCache.Count()/(m_MaxCharInRow*m_MaxCharInRow) )
		{
			uint32 FontTexID = m_pRenderDevice->GetTextureManager()->registerEmptyTexture(
														SDimension2D(SGPFontTextureSize,SGPFontTextureSize),
														String(m_strFontName) + String(" FontTex ") + String(m_CharCacheTexture.size()),
														SGPPF_A4R4G4B4);
			m_pRenderDevice->GetTextureManager()->getTextureByID(FontTexID)->pSGPTexture->setWrapMode(TEXTURE_ADDRESS_CLAMP_TO_EDGE, TEXTURE_ADDRESS_CLAMP_TO_EDGE);
			m_pRenderDevice->GetTextureManager()->getTextureByID(FontTexID)->pSGPTexture->unBindTexture2D(0);

			m_CharCacheTexture.add(FontTexID);

			COpenGLFontBuffer *pFontVB = new COpenGLFontBuffer(FontTexID, m_pRenderDevice);
			m_FontVB.add(pFontVB);
		}
		// Add to cache
		bool bOK = m_FontCharCache.Push(*pNode);
		jassert(bOK);
		pNode = &m_FontCharCache.getReferenceTop();
	}
	else
	{
		bCacheFull = true;
	}


	// Calculate texture update Offset_X and Offset_Y
	uint16 TexOffset = m_LastCacheIndex - uint16(m_LastCacheIndex/(m_MaxCharInRow*m_MaxCharInRow)) * (m_MaxCharInRow*m_MaxCharInRow);
	// OpenGL coordinates need fliping Offset_Y in Font texture
	uint16 TexOffset_Y = (m_MaxCharInRow-1) * m_CharHeight - (TexOffset / m_MaxCharInRow) * m_CharHeight;
	uint16 TexOffset_X = (TexOffset - uint16(TexOffset / m_MaxCharInRow) * m_MaxCharInRow) * m_CharWidth;
		

	if( !_RenderGlyph(&m_Face->glyph->bitmap, m_CharCacheTexture[m_LastCacheIndex/(m_MaxCharInRow*m_MaxCharInRow)], TexOffset_X, TexOffset_Y) )
	{
		if (!bCacheFull)
			m_FontCharCache.Pop();
		return false;
	}


	pNode->charOffsetX = m_Face->glyph->bitmap_left;
	pNode->charOffsetY = m_Face->glyph->bitmap_top;
	pNode->charAdvance = m_Face->glyph->advance.x / 64;
	pNode->charCode = charCode;


	uint16 BackupIdx = m_LastCacheIndex;

	if( m_FontCharCache.Count() < INIT_FONTCACHE_ARRAY_NUM )
	{
		m_LastCacheIndex++;
	}
	else
	{
		// Move Last cache index to Head
		m_LastCacheIndex = m_FontCharCache.getReference(m_LastCacheIndex).iPrev;
			
		m_FontCharCache.getReference(m_FontCharCache.getReference(BackupIdx).iPrev).iNext = 0xFFFF;
		m_FontCharCache.getReference(BackupIdx).iPrev = 0xFFFF;
		m_FontCharCache.getReference(BackupIdx).iNext = m_FirstCacheIndex;
		m_FontCharCache.getReference(m_FirstCacheIndex).iPrev = BackupIdx;
		m_FirstCacheIndex = BackupIdx;

	}

	m_CharCacheMap[charCode] = BackupIdx;

	return true;
}



bool SGP_OpenGLTTFFont::_RenderGlyph(FT_Bitmap* ftBMP, uint32 FontTextureID, uint16 x, uint16 y)
{
	uint32 glyphWidth = ftBMP->width;
	uint32 glyphHeight = ftBMP->rows;

	if( glyphWidth == 0 || glyphHeight == 0 || FontTextureID == 0 )
		return false;

#if defined(GL_ARB_pixel_buffer_object)
    // bind the Font texture and PBO
	m_pRenderDevice->GetTextureManager()->getTextureByID(FontTextureID)->pSGPTexture->BindTexture2D(0);

    // bind PBO to update pixel values
    m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_FontPBOID);
    // map the buffer object into client's memory
    // Note that glMapBuffer() causes sync issue.
    // If GPU is working with this buffer, glMapBuffer() will wait(stall)
    // for GPU to finish its job. To avoid waiting (stall), you can call
    // first glBufferData() with NULL pointer before glMapBuffer().
    // If you do that, the previous data in PBO will be discarded and
    // glMapBuffer() returns a new allocated pointer immediately
    // even if GPU is still working with the previous data.
    m_pRenderDevice->extGlBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, m_PBOByteSize, NULL, GL_STREAM_DRAW);
    GLubyte* pDst = (GLubyte*)m_pRenderDevice->extGlMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
    if(pDst)
    {
		GLubyte* pSrc = 0;
		GLubyte alpha = 0;

        // update data directly on the mapped buffer
/*	IMPORTANT NOTE:
		The way OpenGL loads pixel data is upside-down 
		OpenGL considers the first row of pixels to be the bottom one, and they go up from there
		So Font Glyph in this texture is also upside-down
		We render texts fliply when commiting vertex
*/
		for(uint32 j = 0; j < m_CharHeight; j++)
		{
			for(uint32 i = 0; i < m_CharWidth; i++)
			{
				if( j >= glyphHeight || i >= glyphWidth )
				{
					*(uint16*)(pDst) = 0;
					pDst += 2;
					continue;
				}
				switch(ftBMP->pixel_mode)
				{
					case FT_PIXEL_MODE_GRAY:
						pSrc = ftBMP->buffer + j * ftBMP->width + i;
						// ARGB
						alpha = (*(pSrc)) >> 4;
						*(uint16*)(pDst) = 
							uint16(alpha << 12) | uint16(alpha << 8) | uint16(alpha << 4) | uint16(alpha);
						pDst += 2;
						break;
					case FT_PIXEL_MODE_MONO:
						if((ftBMP->buffer[j * ftBMP->pitch + i/8] & (0x80 >> (i & 7))))
						{
							*(uint16*)(pDst) = 0xFFFF;
						}
						else
						{
							*(uint16*)(pDst) = 0;
						}
						pDst += 2;
						break;
					default:
						break;
				}
			}
		}

        m_pRenderDevice->extGlUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
    }

	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, m_CharWidth, m_CharHeight, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV, 0);

	m_pRenderDevice->extGlBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);


#endif
	return true;
}


int SGP_OpenGLTTFFont::_GetSpaceAdvance()
{
	//We use the advance value of 'A' as the space value
	_BuildCharCache(L'A');
	CharCacheNode* pNode = &m_FontCharCache.getReference(m_CharCacheMap[L'A']);


	if(pNode->charCode)
		return pNode->charAdvance;
	else// no 'A' in this face
		return m_CharWidth / 2;
}

void SGP_OpenGLTTFFont::_DoDrawBorder(float fX_s, float fX_e, float fY, Colour color, float fFontSize, float maxcharOffsetY)
{
	float scale = fFontSize / m_CharWidth;
	float startP[3] = {fX_s, fY + (m_fFaceAscender - maxcharOffsetY) * scale, 0};
	float endP[3] = {fX_e, fY + (m_fFaceAscender - maxcharOffsetY) * scale, 0};
	m_pRenderDevice->GetVertexCacheManager()->RenderLine(startP, endP, &color);
	return;
}

bool SGP_OpenGLTTFFont::_RenderChar( wchar_t charCode, float x, float y ,Colour FontColor, 
	uint32 , float fFontSize, bool , bool  )
{	
	float scale = fFontSize / m_CharWidth;
	float txscale = 1.0f / SGPFontTextureSize;
	float tyscale = 1.0f / SGPFontTextureSize;


	CharCacheNode* pNode = &m_FontCharCache.getReference(m_CharCacheMap[charCode]);
	if( !pNode )
		return false;

	uint16 TexOffset = m_CharCacheMap[charCode] - uint16(m_CharCacheMap[charCode]/(m_MaxCharInRow*m_MaxCharInRow)) * (m_MaxCharInRow*m_MaxCharInRow);
	// OpenGL coordinates need fliping y in Font texture
	uint16 V = (m_MaxCharInRow-1)* m_CharHeight - (TexOffset / m_MaxCharInRow) * m_CharHeight;
	uint16 U = (TexOffset - uint16(TexOffset / m_MaxCharInRow) * m_MaxCharInRow) * m_CharWidth;


	float tx = U * txscale /*- 0.5f*txscale*/;
	float ty = V * tyscale /*- 0.5f*tyscale*/;
	float tw = m_CharWidth * txscale;
	float th = m_CharHeight * tyscale;
	float w = m_CharWidth * scale;
	float h = m_CharHeight * scale;
	float offsetx = pNode->charOffsetX * scale;	
	//float offsety = pNode->charOffsetY * scale;	
	//float advancedx = pNode->charAdvance * scale;	

	float cx = x + offsetx;
	float cy = y + (m_fFaceAscender - pNode->charOffsetY) * scale;

	/*
	2 ________3
	  |		 |
	  |		 |
	0 |______|1
	*/
	SGPVertex_FONT vb[4];
	vb[0].x = cx;
	vb[0].y = cy + h;
	vb[0].z = 0;
	vb[0].w = 1;
	vb[0].VertexColor[0] = FontColor.getFloatRed();
	vb[0].VertexColor[1] = FontColor.getFloatGreen();
	vb[0].VertexColor[2] = FontColor.getFloatBlue();
	vb[0].VertexColor[3] = FontColor.getFloatAlpha();
	vb[0].tu = tx; vb[0].tv = ty + th;
	vb[1].x = cx + w;
	vb[1].y = cy + h;
	vb[1].z = 0;
	vb[1].w = 1;
	vb[1].VertexColor[0] = FontColor.getFloatRed();
	vb[1].VertexColor[1] = FontColor.getFloatGreen();
	vb[1].VertexColor[2] = FontColor.getFloatBlue();
	vb[1].VertexColor[3] = FontColor.getFloatAlpha();
	vb[1].tu = tx + tw; vb[1].tv = ty + th;
	vb[2].x = cx;
	vb[2].y = cy;
	vb[2].z = 0;
	vb[2].w = 1;
	vb[2].VertexColor[0] = FontColor.getFloatRed();
	vb[2].VertexColor[1] = FontColor.getFloatGreen();
	vb[2].VertexColor[2] = FontColor.getFloatBlue();
	vb[2].VertexColor[3] = FontColor.getFloatAlpha();
	vb[2].tu = tx; vb[2].tv = ty;
	vb[3].x = cx + w;
	vb[3].y = cy;
	vb[3].z = 0;
	vb[3].w = 1;
	vb[3].VertexColor[0] = FontColor.getFloatRed();
	vb[3].VertexColor[1] = FontColor.getFloatGreen();
	vb[3].VertexColor[2] = FontColor.getFloatBlue();
	vb[3].VertexColor[3] = FontColor.getFloatAlpha();
	vb[3].tu = tx + tw; vb[3].tv = ty;

	COpenGLFontBuffer *pFontBuffer = m_FontVB[ m_CharCacheMap[charCode] / (m_MaxCharInRow*m_MaxCharInRow) ];
	if( pFontBuffer->IsBufferFull() )
		pFontBuffer->Flush();

	pFontBuffer->PushVertex(vb);

	return true;
}

float SGP_OpenGLTTFFont::_CalcTextWidth(const String& sText, float fFontSize, bool bKerning)
{
	float fTotalSpace = 0;

	uint32 previousGlyph = 0;
	FT_Vector Delta;
	CharCacheNode* pNode = NULL;
	float scale = fFontSize / m_CharWidth;


	int StrLength = sText.length();

	for(int i = 0; i < StrLength; ++i)
	{
		wchar_t singleTex = *(sText.toWideCharPointer() + i);
		//Special cases
		switch( singleTex )
		{
		case L' ':
			fTotalSpace += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		case L'\t': 
			fTotalSpace += m_SpaceAdvance * nTabSize * scale;	
			previousGlyph = 0; 
			continue;
		case L'\n':	
			return fTotalSpace;
		}

		bool bCacheOK = _BuildCharCache( singleTex );
		if (!bCacheOK)
		{
			// not support font Glyph, use L' ' instead
			fTotalSpace += m_SpaceAdvance*scale;	
			previousGlyph = 0; 
			continue;
		}

		pNode = &m_FontCharCache.getReference(m_CharCacheMap[singleTex]);
		if( !pNode )
		{
			previousGlyph = 0;
			continue;
		}

		// glyph index in the font face
		uint32 charGlyphIndex = FT_Get_Char_Index(m_Face, singleTex);

		//Kerning
		if( previousGlyph != 0 && m_bHasKerning && bKerning )
		{			
			FT_Get_Kerning(m_Face, previousGlyph, charGlyphIndex, FT_KERNING_DEFAULT, &Delta);
			fTotalSpace += Delta.x >> 6;
		}


		fTotalSpace += pNode->charAdvance * scale;
		previousGlyph = charGlyphIndex;
	}

	return fTotalSpace;
}


void SGP_OpenGLTTFFont::FlushAllVB(void)
{
	for( int i=0; i<m_FontVB.size(); i++ )
		m_FontVB[i]->Flush();
}