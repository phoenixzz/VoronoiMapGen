
GLenum getGLBlend(SGP_BLEND_FACTOR factor)
{
	GLenum r = 0;
	switch (factor)
	{
		case SGPBF_ZERO:			r = GL_ZERO; break;
		case SGPBF_ONE:				r = GL_ONE; break;
		case SGPBF_DST_COLOR:		r = GL_DST_COLOR; break;
		case SGPBF_ONE_MINUS_DST_COLOR:	r = GL_ONE_MINUS_DST_COLOR; break;
		case SGPBF_SRC_COLOR:		r = GL_SRC_COLOR; break;
		case SGPBF_ONE_MINUS_SRC_COLOR:	r = GL_ONE_MINUS_SRC_COLOR; break;
		case SGPBF_SRC_ALPHA:		r = GL_SRC_ALPHA; break;
		case SGPBF_ONE_MINUS_SRC_ALPHA:	r = GL_ONE_MINUS_SRC_ALPHA; break;
		case SGPBF_DST_ALPHA:		r = GL_DST_ALPHA; break;
		case SGPBF_ONE_MINUS_DST_ALPHA:	r = GL_ONE_MINUS_DST_ALPHA; break;
		case SGPBF_SRC_ALPHA_SATURATE:	r = GL_SRC_ALPHA_SATURATE; break;
	}
	return r;
}

///////////////////////////////////////////////////////////////////////////
// DepthWriteEnableProperty
void DepthWriteEnableProperty::Begin() const
{
	glDepthMask(flag_);
}

void DepthWriteEnableProperty::End() const
{
	// Initially, depth buffer writing is enabled.
	glDepthMask(GL_TRUE);
}

// DepthFuncProperty
void DepthFuncProperty::Begin() const
{
	switch(func_)
	{
		case SGPCFN_NEVER:
			glDisable(GL_DEPTH_TEST);
			break;
		case SGPCFN_LESSEQUAL:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			break;
		case SGPCFN_EQUAL:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_EQUAL);
			break;
		case SGPCFN_LESS:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			break;
		case SGPCFN_NOTEQUAL:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_NOTEQUAL);
			break;
		case SGPCFN_GREATEREQUAL:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_GEQUAL);
			break;
		case SGPCFN_GREATER:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_GREATER);
			break;
		case SGPCFN_ALWAYS:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
			break;
	}
}
void DepthFuncProperty::End() const
{
	if( func_ == SGPCFN_NEVER )
		glEnable(GL_DEPTH_TEST);
	// The initial value is SGPCFN_LESS	
	glDepthFunc(GL_LESS);
}

// TextureProperty
void TextureProperty::Begin() const
{
	COpenGLRenderDevice *RI = static_cast<COpenGLRenderDevice*>(m_pRenderDevice);

	ISGPTexture* pTex = RI->GetTextureManager()->getTextureByID(texture_ID)->pSGPTexture;
	if( pTex )
		pTex->BindTexture2D( unit_ );
}

// TextureActiveProperty
void TextureActiveProperty::Begin() const
{
	COpenGLRenderDevice *RI = static_cast<COpenGLRenderDevice*>(m_pRenderDevice);
	if( RI->MultiTextureExtension )
		RI->extGlActiveTexture(GL_TEXTURE0_ARB + unit_);
}

/*
// TextureBorderColorProperty
void TextureBorderColorProperty::Begin() const
{
	float fBorderColor[4] = {color_.getFloatRed(), color_.getFloatGreen(), color_.getFloatBlue(), color_.getFloatAlpha()};
	glTexParameterfv(target_, GL_TEXTURE_BORDER_COLOR, fBorderColor);
}

void TextureBorderColorProperty::End() const
{
	float fBorderColor[4] = {0.0f};
	glTexParameterfv(target_, GL_TEXTURE_BORDER_COLOR, fBorderColor);
}

// TextureMaxMipLevelProperty
void TextureMaxMipLevelProperty::Begin() const
{
	glTexParameteri(target_, GL_TEXTURE_MAX_LEVEL, level_);
}

void TextureMaxMipLevelProperty::End() const
{
	glTexParameteri(target_, GL_TEXTURE_MAX_LEVEL, 1000);
}

// TextureFilteringProperty 
void TextureFilteringProperty::Begin() const
{
	glTexParameteri(target_, name_, value_);
}
void TextureFilteringProperty::End() const
{
	if( name_ == GL_TEXTURE_MIN_FILTER )
		glTexParameteri(target_, name_, GL_LINEAR);
	else if( name_ == GL_TEXTURE_MAG_FILTER )
	    glTexParameteri(target_, name_, GL_LINEAR);                    
}

// TextureWrapProperty
void TextureWrapProperty::Begin() const
{
	glTexParameteri(target_, name_, value_);
}

void TextureWrapProperty::End() const
{
	if( name_ == GL_TEXTURE_WRAP_S )
		glTexParameteri(target_, name_, GL_REPEAT);
	else if( name_ == GL_TEXTURE_WRAP_T )
	    glTexParameteri(target_, name_, GL_REPEAT);
	else if( name_ == GL_TEXTURE_WRAP_R )
	    glTexParameteri(target_, name_, GL_REPEAT);
}

// TextureAnisotropicProperty
void TextureAnisotropicProperty::Begin() const
{
#ifdef GL_EXT_texture_filter_anisotropic
	if(GLEE_EXT_texture_filter_anisotropic)
	{        
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
			(AnisotropicFilter_>1) ? jmin((GLint)COpenGLConfig::getInstance()->MaxAnisotropy, AnisotropicFilter_) : 1);
	}
#endif
}

void TextureAnisotropicProperty::End() const
{
#ifdef GL_EXT_texture_filter_anisotropic
	if(GLEE_EXT_texture_filter_anisotropic)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	}
#endif
}

// TextureLODBiasProperty
void TextureLODBiasProperty::Begin() const
{
#ifdef GL_EXT_texture_lod_bias
	if(GLEE_EXT_texture_lod_bias)
	{
		const float tmp = jlimit(
			-COpenGLConfig::getInstance()->MaxTextureLODBias, 
			COpenGLConfig::getInstance()->MaxTextureLODBias,
			lodbias_ * 0.125f );
		glTexParameterf(target_, GL_TEXTURE_LOD_BIAS, tmp);
	}
#endif
}

void TextureLODBiasProperty::End() const
{
#ifdef GL_EXT_texture_lod_bias
	if(GLEE_EXT_texture_lod_bias)
	{
		glTexParameterf(target_, GL_TEXTURE_LOD_BIAS, 0.0f);
	}
#endif
}
*/


// AlphaBlendOpProperty
void AlphaBlendOpProperty::Begin() const
{
	COpenGLRenderDevice *RI = static_cast<COpenGLRenderDevice*>(m_pRenderDevice);
	if(RI->queryDriverFeature(SGPVDF_BLEND_OPERATIONS))
	{
		glEnable(GL_BLEND);
		switch(op_)
		{
		case SGPBO_SUBTRACT:
			glBlendEquation(GL_FUNC_SUBTRACT);
			break;
		case SGPBO_REVSUBTRACT:
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			break;
		case SGPBO_MIN:
			glBlendEquation(GL_MIN);
			break;
		case SGPBO_MAX:
			glBlendEquation(GL_MAX);
			break;

		default:
			glBlendEquation(GL_FUNC_ADD);
			break;
		}
	}
}

void AlphaBlendOpProperty::End() const
{
	glDisable(GL_BLEND);
	if( op_ != SGPBO_ADD )
		glBlendEquation(GL_FUNC_ADD);
}

// AlphaBlendProperty
void AlphaBlendProperty::Begin() const
{
	glBlendFunc( getGLBlend(src_), getGLBlend(dst_) );
}

void AlphaBlendProperty::End() const
{
	glBlendFunc(GL_ONE, GL_ZERO);
}


// AlphaBlendFuncSeparateProperty
void AlphaBlendFuncSeparateProperty::Begin() const
{
	glBlendFuncSeparate( getGLBlend(srcRGB_), getGLBlend(dstRGB_), getGLBlend(srcAlpha_), getGLBlend(dstAlpha_) );
}

void AlphaBlendFuncSeparateProperty::End() const
{
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
}




// StencilProperty
void StencilProperty::Begin() const
{
	GLenum func = GL_ALWAYS;
	switch(func_)
	{
	case SGPCFN_ALWAYS:
		func = GL_ALWAYS;
		break;
	case SGPCFN_NEVER:
		func = GL_NEVER;
		break;
	case SGPCFN_LESSEQUAL:
		func = GL_LEQUAL;
		break;
	case SGPCFN_EQUAL:
		func = GL_EQUAL;
		break;
	case SGPCFN_LESS:
		func = GL_LESS;
		break;
	case SGPCFN_NOTEQUAL:
		func = GL_NOTEQUAL;
		break;
	case SGPCFN_GREATEREQUAL:
		func = GL_GEQUAL;
		break;
	case SGPCFN_GREATER:
		func = GL_GREATER;
		break;

	default:
		break;
	}
	glEnable(GL_STENCIL_TEST);
	glStencilFunc( func, ref_, mask_ );
	glStencilOp( fail_, zfail_, pass_ );
	glStencilMask( writemask_ );
}

void StencilProperty::End() const
{
	glStencilFunc( GL_ALWAYS, 0, 0xFFFFFFFF );	
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glStencilMask( 0xFFFFFFFF );
	glDisable(GL_STENCIL_TEST);
}

// FillModeProperty
void FillModeProperty::Begin() const
{
	glPolygonMode(GL_FRONT_AND_BACK, mode_);
}

void FillModeProperty::End() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// CullingModeProperty
void CullingModeProperty::Begin() const
{
	if( bculling_ )
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	glCullFace(mode_);
	glFrontFace(frontface_);
}

void CullingModeProperty::End() const
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

// ColorMaskProperty
void ColorMaskProperty::Begin() const
{
	glColorMask(red_, green_, blue_, alpha_);
}

void ColorMaskProperty::End() const
{
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}

// ThicknessProperty
void ThicknessProperty::Begin() const
{
	COpenGLRenderDevice *RI = static_cast<COpenGLRenderDevice*>(m_pRenderDevice);

	if(COpenGLConfig::getInstance()->FullScreenAntiAlias)
	{
		// we don't use point smoothing
		glPointSize(jlimit(RI->DimAliasedPoint[0], RI->DimAliasedPoint[1], size_));
		glLineWidth(jlimit(RI->DimSmoothedLine[0], RI->DimSmoothedLine[1], size_));
	}
	else
	{
		glPointSize(jlimit(RI->DimAliasedPoint[0], RI->DimAliasedPoint[1], size_));
		glLineWidth(jlimit(RI->DimAliasedLine[0], RI->DimAliasedLine[1], size_));
	}
}
void ThicknessProperty::End() const
{
	glPointSize(1);
	glLineWidth(1);
}

// AntiAliasingProperty
void AntiAliasingProperty::Begin() const
{
	if(GLEE_ARB_multisample)
	{
		if( mode_ & SGPAAM_ALPHA_TO_COVERAGE )
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

		if( (COpenGLConfig::getInstance()->FullScreenAntiAlias >= 2) && (mode_ & (SGPAAM_SIMPLE|SGPAAM_QUALITY)) )
		{
			glEnable(GL_MULTISAMPLE_ARB);
#ifdef GL_NV_multisample_filter_hint
			if (GLEE_NV_multisample_filter_hint)
			{
				if ((mode_ & SGPAAM_QUALITY) == SGPAAM_QUALITY)
					glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
			}
#endif
		}
		else
			glDisable(GL_MULTISAMPLE_ARB);
	}
	if( mode_ & SGPAAM_LINE_SMOOTH )
	{
		glEnable(GL_LINE_SMOOTH);
	}
	if( mode_ & SGPAAM_POINT_SMOOTH )
	{
		// often in software, and thus very slow
		glEnable(GL_POINT_SMOOTH);
	}
}

void AntiAliasingProperty::End() const
{
	if( mode_ & SGPAAM_ALPHA_TO_COVERAGE )
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
	if( mode_ & SGPAAM_POINT_SMOOTH )
		glDisable(GL_POINT_SMOOTH);
	if( mode_ & SGPAAM_LINE_SMOOTH )
		glDisable(GL_LINE_SMOOTH);
	if( (COpenGLConfig::getInstance()->FullScreenAntiAlias >= 2) && (mode_ & (SGPAAM_SIMPLE|SGPAAM_QUALITY)) )
		glDisable(GL_MULTISAMPLE_ARB);
}

//PolygonOffsetProperty
void PolygonOffsetProperty::Begin() const
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(factor_, units_);
}

void PolygonOffsetProperty::End() const
{
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0, 0);
}

//PolygonFillLineOffsetProperty
void PolygonFillLineOffsetProperty::Begin() const
{
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(factor_, units_);
}

void PolygonFillLineOffsetProperty::End() const
{
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(0, 0);
}