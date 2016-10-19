#ifndef __SGP_OPENGLES2MATERIALPROPERTIES_HEADER__
#define __SGP_OPENGLES2MATERIALPROPERTIES_HEADER__


// defines
#define REGISTER_BEGIN			virtual void Begin() const;
#define REGISTER_END			virtual bool RegisterEnd() const { return true; } virtual void End() const;
#define REGISTER_UPDATE			virtual bool IsDynamic() const { return true; } virtual void Update( float elapsedTime );


// DepthWriteEnableProperty
// Initially, depth buffer writing is enabled.        
class DepthWriteEnableProperty : public ISGPMaterialProperty
{
public :
	inline DepthWriteEnableProperty( GLboolean flag ) : flag_( flag ) {}
	virtual ISGPMaterialProperty * Clone() const { return new DepthWriteEnableProperty(flag_); }
	REGISTER_BEGIN;
	REGISTER_END;
	
private :
	GLboolean flag_;
};

// DepthFuncProperty
// The initial value is SGPCFN_LESS
class DepthFuncProperty : public ISGPMaterialProperty
{
public :
	inline DepthFuncProperty( SGP_COMPARISON_FUNC func ) : func_( func ) {}
	virtual ISGPMaterialProperty * Clone() const { return new DepthFuncProperty( func_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	SGP_COMPARISON_FUNC func_;
};

// TextureProperty
class TextureProperty : public ISGPMaterialProperty
{
public:
	inline TextureProperty( uint32 texID, uint8 unit ) : texture_ID( texID ), unit_( unit ) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureProperty( texture_ID, unit_ ); }
	//inline void UpdateTexture(uint32 texID, uint8 unit) { texture_ID = texID; unit_ = unit; }
	REGISTER_BEGIN;
	//REGISTER_END;

private :
	uint32 texture_ID;
	uint8 unit_;
};

// TextureActiveProperty
class TextureActiveProperty : public ISGPMaterialProperty
{
public:
	inline TextureActiveProperty( uint8 unit ) : unit_( unit ) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureActiveProperty( unit_ ); }
	REGISTER_BEGIN;

private :
	uint8 unit_;
};

/*
// TextureBorderColorProperty
class TextureBorderColorProperty : public ISGPMaterialProperty
{
public:
	inline TextureBorderColorProperty( GLenum target, Colour color ) : target_(target), color_(color) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureBorderColorProperty( target_, color_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	GLenum target_;
	Colour color_;
};

// TextureAnisotropicProperty
class TextureAnisotropicProperty : public ISGPMaterialProperty
{
public:
	inline TextureAnisotropicProperty( GLenum target, GLint AnisotropicFilter ) : target_(target), AnisotropicFilter_(AnisotropicFilter) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureAnisotropicProperty( target_, AnisotropicFilter_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	GLenum target_;
	GLint AnisotropicFilter_;
};

// TextureLODBiasProperty
// The initial value is 0.0
class TextureLODBiasProperty : public ISGPMaterialProperty
{
public:
	inline TextureLODBiasProperty( GLenum target, float lodbias ) : target_(target), lodbias_(lodbias) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureLODBiasProperty( target_, lodbias_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	GLenum target_;
	float lodbias_;
};

// TextureMaxMipLevelProperty
// The initial value is 1000
class TextureMaxMipLevelProperty : public ISGPMaterialProperty
{
public:
	inline TextureMaxMipLevelProperty( GLenum target, GLint level ) : target_(target), level_(level) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureMaxMipLevelProperty( target_, level_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	GLenum target_;
	GLint level_;
};

// TextureFilteringProperty 
class TextureFilteringProperty : public ISGPMaterialProperty
{
public:
	inline TextureFilteringProperty( GLenum target, GLenum name, GLint value ) : target_(target), name_(name), value_(value) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureFilteringProperty( target_, name_, value_ ); }
	REGISTER_BEGIN;
	REGISTER_END;
private :
	GLenum target_;
	GLenum name_;
	GLint value_;
};

// TextureWrapProperty
class TextureWrapProperty : public ISGPMaterialProperty
{
public:
	inline TextureWrapProperty( GLenum target, GLenum name, GLint value ) : target_(target), name_(name), value_(value) {}
	virtual ISGPMaterialProperty * Clone() const { return new TextureWrapProperty( target_, name_, value_ ); }
	REGISTER_BEGIN;
	REGISTER_END;
private :
	GLenum target_;
	GLenum name_;
	GLint value_;
};
*/



// AlphaBlendOpProperty
class AlphaBlendOpProperty : public ISGPMaterialProperty
{
public :
	inline AlphaBlendOpProperty( SGP_BLEND_OPERATION op ) : op_( op ) {}
	virtual ISGPMaterialProperty * Clone() const { return new AlphaBlendOpProperty( op_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	SGP_BLEND_OPERATION op_;
};

// AlphaBlendProperty
class AlphaBlendProperty : public ISGPMaterialProperty
{
public :
	inline AlphaBlendProperty( SGP_BLEND_FACTOR src, SGP_BLEND_FACTOR dst ) : src_( src ), dst_( dst ) {}
	virtual ISGPMaterialProperty * Clone() const { return new AlphaBlendProperty( src_, dst_ ); }
	REGISTER_BEGIN;
	REGISTER_END;

private :
	SGP_BLEND_FACTOR src_;
	SGP_BLEND_FACTOR dst_;
};

// AlphaBlendFuncSeparateProperty
class AlphaBlendFuncSeparateProperty : public ISGPMaterialProperty
{
public :
	inline AlphaBlendFuncSeparateProperty( SGP_BLEND_FACTOR srcRGB, SGP_BLEND_FACTOR dstRGB, SGP_BLEND_FACTOR srcAlpha, SGP_BLEND_FACTOR dstAlpha ) 
		: srcRGB_( srcRGB ), dstRGB_( dstRGB ), srcAlpha_( srcAlpha ), dstAlpha_( dstAlpha ) {}
	virtual ISGPMaterialProperty * Clone() const { return new AlphaBlendFuncSeparateProperty( srcRGB_, dstRGB_, srcAlpha_, dstAlpha_ ); }
	REGISTER_BEGIN;
	REGISTER_END;
private :
	SGP_BLEND_FACTOR srcRGB_;
	SGP_BLEND_FACTOR dstRGB_;
	SGP_BLEND_FACTOR srcAlpha_;
	SGP_BLEND_FACTOR dstAlpha_;
};


// StencilProperty
class StencilProperty : public ISGPMaterialProperty
{
public :
	inline StencilProperty( SGP_COMPARISON_FUNC func, GLint ref, GLuint mask,
		GLenum fail, GLenum zfail, GLenum pass, GLuint writemask ) 
		: func_(func), ref_(ref), mask_(mask), fail_(fail),
		zfail_(zfail), pass_(pass), writemask_(writemask) {}
	virtual ISGPMaterialProperty * Clone() const 
	{ return new StencilProperty( func_, ref_, mask_, fail_, zfail_, pass_, writemask_); }

	REGISTER_BEGIN;
	REGISTER_END;

private :
	SGP_COMPARISON_FUNC func_;
	GLint ref_;
	GLuint mask_;

	GLenum fail_;
	GLenum zfail_;
	GLenum pass_;

	GLuint writemask_;
};

// FillModeProperty
// GL_POINT, GL_LINE, and GL_FILL. initial value is GL_FILL
class FillModeProperty : public ISGPMaterialProperty
{
public :
	inline FillModeProperty( GLenum mode ) : mode_( mode ) {}
	virtual ISGPMaterialProperty * Clone() const { return new FillModeProperty(mode_); }
	REGISTER_BEGIN;
	REGISTER_END;
private:
	GLenum mode_;
};



// CullingModeProperty
// GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK initial value is GL_BACK
class CullingModeProperty : public ISGPMaterialProperty
{
public :
	inline CullingModeProperty( bool bculling, GLenum mode, GLenum frontface ) : bculling_(bculling), mode_( mode ), frontface_(frontface) {}
	virtual ISGPMaterialProperty * Clone() const { return new CullingModeProperty(bculling_, mode_, frontface_); }
	REGISTER_BEGIN;
	REGISTER_END;
	
private:
	GLenum mode_;
	GLenum frontface_;
	bool bculling_;
};

// ColorMaskProperty
// The initial values are all GL_TRUE
class ColorMaskProperty : public ISGPMaterialProperty
{
public :
	inline ColorMaskProperty( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) : 
		red_(red), green_(green), blue_(blue), alpha_(alpha) {}
	virtual ISGPMaterialProperty * Clone() const { return new ColorMaskProperty(red_, green_, blue_, alpha_); }
	REGISTER_BEGIN;
	REGISTER_END;
	
private:
	GLboolean  red_;
	GLboolean  green_;
	GLboolean  blue_;
	GLboolean  alpha_;
};


// ThicknessProperty
// The initial value is 1.                
class ThicknessProperty : public ISGPMaterialProperty
{
public :
	inline ThicknessProperty( GLfloat size ) : size_(size) {}
	virtual ISGPMaterialProperty * Clone() const { return new ThicknessProperty(size_); }
	REGISTER_BEGIN;
	REGISTER_END;
	
private:
	GLfloat size_;
};

// Anti aliasing
class AntiAliasingProperty : public ISGPMaterialProperty
{
public :
	inline AntiAliasingProperty( SGP_ANTI_ALIASING_MODE mode ) : mode_(mode) {}
	virtual ISGPMaterialProperty * Clone() const { return new AntiAliasingProperty(mode_); }
	REGISTER_BEGIN;
	REGISTER_END;
	
private:
	SGP_ANTI_ALIASING_MODE mode_;
};

// PolygonOffset (depth bias)
class PolygonOffsetProperty : public ISGPMaterialProperty
{
public :
	inline PolygonOffsetProperty( GLfloat factor, GLfloat units ) : factor_(factor), units_(units) {}
	virtual ISGPMaterialProperty * Clone() const { return new PolygonOffsetProperty(factor_, units_); }
	REGISTER_BEGIN;
	REGISTER_END;
	
private:
	GLfloat  factor_;
	GLfloat  units_;
};

#endif		// __SGP_OPENGLES2MATERIALPROPERTIES_HEADER__