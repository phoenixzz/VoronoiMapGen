#ifndef __SGP_VERTEXBUFFERSKIN_HEADER__
#define __SGP_VERTEXBUFFERSKIN_HEADER__

struct SGPSkin
{
	bool				bAlpha;			// transparent material?
	bool				bAlphaTest;		// alpha-test material?
	bool				bLightMap;		// with lightmap texture?
	SGP_SHADER_TYPE		nShaderType;	// render shader types
	SGP_PRIMITIVE_TYPE	nPrimitiveType;	// render primitive types	
	uint32	nTextureID[SGP_MATERIAL_MAX_TEXTURES];	// OpenGL texture ID
	uint8	nTextureNum;				// number of used OpenGL texture
	
	float	vUVSpeed[2];				// [0]USpeed [1]VSpeed
	uint16	vUVTile[5];					// [0]UTile [1]VTile [2]StartFrameID [3]LoopMode [4]PlayBackRate
	
	uint32  nNumMatKeyFrame;			// color key frame num
	SGPMF1MatKeyFrame * pMatKeyFrame;	// color key frame data

	SGPSkin() 
	{ 
		bAlpha = false;
		bAlphaTest = false;
		bLightMap = false;
		nShaderType = SGPST_VERTEXCOLOR;
		nPrimitiveType = SGPPT_TRIANGLES;
		nTextureNum = 0;
		memset( nTextureID, 0, sizeof(uint32)*SGP_MATERIAL_MAX_TEXTURES );

		vUVSpeed[0] = vUVSpeed[1] = 0;
		vUVTile[0] = vUVTile[1] = 1;
		vUVTile[2] = vUVTile[3] = 0;
		vUVTile[4] = 60;				// (60 ms)
		
		nNumMatKeyFrame = 0;
		pMatKeyFrame = NULL;
	}

	inline bool operator == (const SGPSkin &other)
	{
		bool bSame = ( bAlpha == other.bAlpha ) && 
			( bAlphaTest == other.bAlphaTest ) &&
			( bLightMap == other.bLightMap ) &&
			( nTextureNum == other.nTextureNum ) &&
			( nShaderType == other.nShaderType ) &&
			( nPrimitiveType == other.nPrimitiveType );
		for( int i=0; i<SGP_MATERIAL_MAX_TEXTURES; ++i )
			bSame = bSame && (nTextureID[i] == other.nTextureID[i]);
		return bSame;
	}

	inline Vector2D getMatKeyFrameUVOffset(float fTimePassedFromCreated) 
	{
		if( (nNumMatKeyFrame == 0) || (pMatKeyFrame == NULL) )
			return Vector2D(0, 0);

		if( nNumMatKeyFrame == 1 )
			return Vector2D(pMatKeyFrame[0].m_fUOffset, pMatKeyFrame[0].m_fVOffset);
		
		float fFrameID = fTimePassedFromCreated * 30.0f;
		fFrameID -= int32(fFrameID / (float)pMatKeyFrame[nNumMatKeyFrame-1].m_iFrameID) * (float)pMatKeyFrame[nNumMatKeyFrame-1].m_iFrameID;

		for( uint32 i=0; i<nNumMatKeyFrame-1; i++ )
		{
			if( (fFrameID >= (float)pMatKeyFrame[i].m_iFrameID) &&
				(fFrameID <= (float)pMatKeyFrame[i+1].m_iFrameID) )
			{
				float fInterp = float(pMatKeyFrame[i+1].m_iFrameID - fFrameID) / float(pMatKeyFrame[i+1].m_iFrameID - pMatKeyFrame[i].m_iFrameID);
				return Vector2D(pMatKeyFrame[i].m_fUOffset*fInterp+pMatKeyFrame[i+1].m_fUOffset*(1.0f-fInterp), pMatKeyFrame[i].m_fVOffset*fInterp+pMatKeyFrame[i+1].m_fVOffset*(1.0f-fInterp));
			}
		}
		return Vector2D(0, 0);
	}

	inline Vector4D getMatKeyFrameColor(float fTimePassedFromCreated)
	{
		if( (nNumMatKeyFrame == 0) || (pMatKeyFrame == NULL) )
			return Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
		if( nNumMatKeyFrame == 1 )
			return Vector4D(1.0f, 1.0f, 1.0f, 1.0f/*pMatKeyFrame->m_MaterialColor[0], pMatKeyFrame->m_MaterialColor[1], pMatKeyFrame->m_MaterialColor[2], pMatKeyFrame->m_MaterialColor[3]*/);
		
		float fFrameID = fTimePassedFromCreated * 30.0f;
		fFrameID -= int32(fFrameID / (float)pMatKeyFrame[nNumMatKeyFrame-1].m_iFrameID) * (float)pMatKeyFrame[nNumMatKeyFrame-1].m_iFrameID;

		for( uint32 i=0; i<nNumMatKeyFrame-1; i++ )
		{
			if( (fFrameID >= (float)pMatKeyFrame[i].m_iFrameID) &&
				(fFrameID <= (float)pMatKeyFrame[i+1].m_iFrameID) )
			{
				float fInterp = float(pMatKeyFrame[i+1].m_iFrameID - fFrameID) / float(pMatKeyFrame[i+1].m_iFrameID - pMatKeyFrame[i].m_iFrameID);
				return Vector4D(
					pMatKeyFrame[i].m_MaterialColor[0] * fInterp + pMatKeyFrame[i+1].m_MaterialColor[0] * (1.0f-fInterp),
					pMatKeyFrame[i].m_MaterialColor[1] * fInterp + pMatKeyFrame[i+1].m_MaterialColor[1] * (1.0f-fInterp),
					pMatKeyFrame[i].m_MaterialColor[2] * fInterp + pMatKeyFrame[i+1].m_MaterialColor[2] * (1.0f-fInterp),
					1.0f/*pMatKeyFrame[i].m_MaterialColor[3] * fInterp + pMatKeyFrame[i+1].m_MaterialColor[3] * (1.0f-fInterp)*/ );
			}
		}
		return Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
	}
};

// This struct is not per SGP skin, It is setted for every Render Batch
struct RenderBatchConfig
{
	uint32					m_nLightMapTextureID;			// Lightmap Engine texture ID
	uint32					m_nReplacedTextureID;			// Engine texture ID for Replace Diffuse texture
	float					m_fBatchAlpha;					// Render Batch alpha
	float					m_fTimePassedFromCreated;		// Time Passed From Instance created

	RenderBatchConfig() : m_nLightMapTextureID(1), m_nReplacedTextureID(0), m_fBatchAlpha(1.0f), m_fTimePassedFromCreated(0.0f)
	{}
	void Reset() 
	{ m_nLightMapTextureID = 1; m_nReplacedTextureID = 0; m_fBatchAlpha = 1.0f; m_fTimePassedFromCreated = 0.0f; }
};

#endif		// __SGP_VERTEXBUFFERSKIN_HEADER__