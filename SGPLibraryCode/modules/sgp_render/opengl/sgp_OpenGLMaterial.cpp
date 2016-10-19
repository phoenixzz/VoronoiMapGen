OpenGLMaterial::Script::Section::Section()
{
}



OpenGLMaterial::Script::Section::Section( char *MaterialStr )
{
	BuildInfo buildInfo( MaterialStr, true );
	LoadFromMaterialStr( buildInfo );
}

void OpenGLMaterial::Script::Section::CharToUpper( char* str )
{
	while( *str != 0 )
	{
		if( *str >= 'a' && *str <= 'z' )
			*str = _toupper( *str );
		str++;
	}
}

void OpenGLMaterial::Script::Section::CharTrimLeftRight(char* str )
{
	int strLeftIdx = 0;
	int strRightIdx = strlen(str) - 1;
	char* FirstIdx = str;

	// Trim Left
	while( *str != 0 )
	{
		if( (*str != ' ') && (*str != '\n') && (*str != '\t') )
		{
			break;
		}
		strLeftIdx++;
		str++;
	}

	// Trim Right
	while(	(*(FirstIdx + strRightIdx) == ' ') ||
			(*(FirstIdx + strRightIdx) == '\n') ||
			(*(FirstIdx + strRightIdx) == '\t') )
	{
		strRightIdx--;
		if( strRightIdx == 0 )
			break;
	}

	// Get Final string
	int Idx = 0;
	for(int i = strLeftIdx; i <= strRightIdx; i++,Idx++ )
	{
		*(FirstIdx+Idx) = *(FirstIdx+i);
	}
	
	*(FirstIdx+Idx) = 0;
}

// Searches for a substring within this SrcStr, the index from which the search should proceed 
// Return : the index of the first occurrence of this substring, or -1 if it's not found.
int OpenGLMaterial::Script::Section::Char_Find_first_of( const char* SrcStr, const char SubStr, int index )
{
	int StrIdx = 0;
	
	while( *(SrcStr+index+StrIdx) != 0 )
	{
		if( *(SrcStr+index+StrIdx) == SubStr )
			return StrIdx;

		StrIdx++;
	}
	return -1;
}

// Searches for a substring first not within this SrcStr, the index from which the search should proceed  
int OpenGLMaterial::Script::Section::Char_Find_first_not_of( const char* SrcStr, const char* SubStr, int index )
{
	int StrIdx = 0;
	
	while( *(SrcStr+index+StrIdx) != 0 )
	{
		int SubStrIdx = 0;
		while( *(SubStr+SubStrIdx) != 0 )
		{
			if( *(SrcStr+index+StrIdx) == *(SubStr+SubStrIdx) )
				break;
			SubStrIdx++;
		}
		if( *(SubStr+SubStrIdx) == 0 )
			return StrIdx;
		StrIdx++;
	}
	return -1;
}


int OpenGLMaterial::Script::Section::Char_Find_last_not_of( const char* SrcStr, const char* SubStr, int str_len )
{
	str_len--;
	while(str_len >= 0)
	{
		int SubStrIdx = 0;
		while( *(SubStr+SubStrIdx) != 0 )
		{
			if( *(SrcStr+str_len) == *(SubStr+SubStrIdx) )
				break;
			SubStrIdx++;
		}
		if( *(SubStr+SubStrIdx) == 0 )
			return str_len;
		str_len--;
	}
	return -1;
}


void OpenGLMaterial::Script::Section::LoadFromMaterialStr( BuildInfo & buildInfo )
{
	char *line = NULL;
	char *id = NULL;

	while( GetLine( buildInfo, &line ) )
	{
		uint32 index = 0;

		// get the id
		bool stop = false;
		while( !stop && index < strlen(line) )
		{
			switch( line[index] )
			{
			case '=' :
			case ' ' :
			case '\t':
			case '\0':
				stop = true;
				break;

			case '\n' :
				stop = true;
				break;

			default :
				if( !IsAlphaNumeric( line[index] ) )
				{
					jassertfalse;
				}
				++index;
			}
		}


		// check if it's a section
		bool isSection = false;
		if( index == strlen(line) )
		{
			isSection = true;
		}
		else
		{
			if( Char_Find_first_not_of(line, "\t ", index) == -1 )
				isSection = true;
		}

		// This line is comment
		if( line[0] == '/' && line[1] == '/' )
			continue;
		// This line is space 
		if( strlen(line) == 0 )
			continue;

		if( isSection )
		{
			id = line;
			CharToUpper(id);
		}

		// get the section or value
		if( isSection )
			ReadSection( buildInfo, id );
		else
			ReadValue( buildInfo, line, index );
	}
}



void OpenGLMaterial::Script::Section::ReadSection( BuildInfo & buildInfo, char* id )
{
	char *line = NULL;
	GetLine( buildInfo, &line );
	if( strcmp( line, "{" ) != 0 )
	{
		jassertfalse;
	}

	// create the sub section
	BuildInfo subInfo( buildInfo.Material_Str, false );
	Section * newSection = new Section;
	try
	{
		newSection->LoadFromMaterialStr( subInfo );
	}
	catch ( ... )
	{
		delete newSection;
		newSection = 0;
		throw;
	}

	// Update String Pos
	buildInfo.Material_Str = subInfo.Material_Str;

	sections_.add( newSection );

	names_.add(id);
}


// ReadValue()
void OpenGLMaterial::Script::Section::ReadValue( BuildInfo & , char* line, int index )
{
	char *id = line;
	
	// get the value
	index = Char_Find_first_of( line, '=' );
	if( index == -1 )
	{
		jassertfalse;
		return;
	}

	line[index] = 0;
	char *value = line + index + 1;

	int first = Char_Find_first_not_of(value, "= \t");
	int last = Char_Find_last_not_of(value, " \t", strlen(value));
	*(value+last+1) = 0;
	value = value + first;
	LineValue_.add(value);


	last = Char_Find_last_not_of(id, " \t", strlen(id));
	*(id+last+1) = 0;
	CharToUpper(id);
	LineID_.add(id);
}


// IsAlphaNumeric()
bool OpenGLMaterial::Script::Section::IsAlphaNumeric( char c )
{
	return ( c >= 'a' && c <= 'z' ) ||
		   ( c >= 'A' && c <= 'Z' ) ||
		   ( c >= '0' && c <= '9' ) ||
		   c == '_' || c == '/';
}


// GetLine()
bool OpenGLMaterial::Script::Section::GetLine( BuildInfo & buildInfo, char** line )
{
	int StrOffset = 0;
	if( *(buildInfo.Material_Str + StrOffset) == 0 )
		return false;

	*(line) = buildInfo.Material_Str + StrOffset;

	while( *(buildInfo.Material_Str + StrOffset) != '\n' )
	{
		StrOffset++;
	}

	*(buildInfo.Material_Str + StrOffset) = 0;
	StrOffset++;

	buildInfo.Material_Str += StrOffset;
	CharTrimLeftRight( *line );

	if( buildInfo.global && strcmp( *line, "}" )==0 )
	{
		jassertfalse;
	}
	return (strcmp( *line, "}" )!=0);
}



OpenGLMaterial::Script::Section::~Section()
{
	for( int i=0; i<sections_.size(); i++ )
	{
		if( sections_[i] != NULL )
			delete sections_[i];
		sections_.set(i, NULL);
	}
}




int OpenGLMaterial::Script::Section::ContainSection( const char* id )
{
	int Found = 0;
	for( int i=0; i<names_.size(); ++i )
	{
		if( STRNICMP(id, names_[i], strlen(id)) == 0 )
			Found++;
	}
	return Found;
}

OpenGLMaterial::Script::Section* OpenGLMaterial::Script::Section::GetSection( const char* name, int ID )
{
	int FoundIdx = 0;

	for(int SectionIdx=0; SectionIdx<names_.size(); SectionIdx++)
	{
		if( STRNICMP(name, names_[SectionIdx], strlen(name) ) == 0 )
		{
			if( FoundIdx == ID )
				return sections_[SectionIdx];
			FoundIdx++;
		}
	}

	return NULL;
}

// ContainString()
bool OpenGLMaterial::Script::Section::ContainString( const char* id )
{
	for(int i=0; i<LineID_.size(); i++)
	{
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
			return true;
	}

	return false;
}

// GetString()
const char* OpenGLMaterial::Script::Section::GetString( const char* id, const char* def )
{
	for(int i=0; i<LineID_.size(); i++)
	{
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
			return LineValue_[i];
	}
	return def;
}


// GetInt()
int OpenGLMaterial::Script::Section::GetInt( const char* id, int def )
{
	for(int i=0; i<LineID_.size(); i++)
	{
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
			return atoi(LineValue_[i]);
	}

	return def;
}


// GetFloat()
float OpenGLMaterial::Script::Section::GetFloat( const char* id, float def )
{
	for(int i=0; i<LineID_.size(); i++)
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
			return (float)atof(LineValue_[i]);
	return def;
}


// ContainBool()
bool OpenGLMaterial::Script::Section::ContainBool( const char* id )
{
	for(int i=0; i<LineID_.size(); i++)
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
			return true;
	return false;
}

// GetBool()
bool OpenGLMaterial::Script::Section::GetBool( const char* id, bool def )
{
	for(int i=0; i<LineID_.size(); i++)
	{
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
		{
			if( STRNICMP( "true", LineValue_[i], 4 ) == 0 )
				return true;
			else if( STRNICMP( "false", LineValue_[i], 5 ) == 0 )
				return false;
		}
	}
	return def;
}

// GetVec4()
Vector4D OpenGLMaterial::Script::Section::GetVec4( const char* id, const Vector4D & def )
{
	static char NumStr[128];
	Vector4D Result(0, 0, 0);

	for(int i=0; i<LineID_.size(); i++)
	{
		if( STRNICMP(id, LineID_[i], strlen(id) ) == 0 )
		{
			float vec[4] = { 0 } ;
			const char *Vec4Str = LineValue_[i];
		
			if( (Vec4Str[0] == '(') && (Vec4Str[strlen(Vec4Str)-1] == ')') )
			{
				strcpy( NumStr, Vec4Str+1 );
				NumStr[strlen(Vec4Str)-2] = 0;


				char* pStr = NumStr;
				for(int i = 0; i < 3; ++i )
				{
					int pos = Char_Find_first_of( pStr, ',' );
					if( pos == -1 )
						return def;

					*(pStr+pos) = 0;
					vec[i] = (float)atof(pStr);

					pStr += pos + 1;
				}
				vec[3] = (float)atof(pStr);

				Result.Set(vec[0], vec[1], vec[2], vec[3]);
				return Result;
			}
			return def;
		}
	}
	return def;
}







//     _____           _       _   
//    / ____|         (_)     | |  
//   | (___   ___ _ __ _ _ __ | |_ 
//    \___ \ / __| '__| | '_ \| __|
//    ____) | (__| |  | | |_) | |_ 
//   |_____/ \___|_|  |_| .__/ \__|
//                      | |        
//                      |_|        


// ctor
OpenGLMaterial::Script::Script( char* MaterialStr ) :
	global_( NULL )
{
	global_ = new Section( MaterialStr );
}


// dtor
OpenGLMaterial::Script::~Script()
{
	delete global_;
	global_ = NULL;
}


/////////////////////////////////////////////////////////////////////////////


//    _____              
//   |  __ \             
//   | |__) |_ _ ___ ___ 
//   |  ___/ _` / __/ __|
//   | |  | (_| \__ \__ \
//   |_|   \__,_|___/___/
//                       

OpenGLMaterial::Pass::Pass()
{
}


OpenGLMaterial::Pass::~Pass()
{
	for( int i=0; i<begin_.size(); i++ )
	{
		if( begin_[i] )
			delete begin_[i];
		begin_.set(i, NULL);
	}
}


// Clone()
OpenGLMaterial::Pass * OpenGLMaterial::Pass::Clone()
{
	Pass * newPass = new Pass();

	newPass->begin_.ensureStorageAllocated( begin_.size() );
	newPass->end_.ensureStorageAllocated( end_.size() );
	newPass->update_.ensureStorageAllocated( update_.size() );

	for( int i = 0; i < begin_.size(); ++i )
	{
		ISGPMaterialProperty * property = begin_[i]->Clone();
		newPass->begin_.add( property );
		if( property->RegisterEnd() )
			newPass->end_.add( property );
		if( property->IsDynamic() )
			newPass->update_.add( property );
	}

	return newPass;
}



void OpenGLMaterial::Pass::AddProperty( ISGPMaterialProperty * property )
{
	begin_.add( property );

	if( property->RegisterEnd() )
	{
		end_.add( property );
	}
	if( property->IsDynamic() )
	{
		update_.add( property );
	}
}

//void OpenGLMaterial::Pass::UpdateTexture(uint32 texID, uint8 unit)
//{
//	for( int i = 0; i < begin_.size(); ++i )
//		begin_[i]->UpdateTexture(texID, unit);
//}


// PreRender()
void OpenGLMaterial::Pass::PreRender()
{
	for( int i = 0; i < begin_.size(); ++i )
		begin_[i]->Begin();
}


// PostRender()
void OpenGLMaterial::Pass::PostRender()
{
	for( int i = 0; i < end_.size(); ++i )
		end_[i]->End();
}

//    __  __       _            _       _ 
//   |  \/  |     | |          (_)     | |
//   | \  / | __ _| |_ ___ _ __ _  __ _| |
//   | |\/| |/ _` | __/ _ \ '__| | / _` | |
//   | |  | | (_| | ||  __/ |  | | (_| | |
//   |_|  |_|\__,_|\__\___|_|  |_|\__,_|_|
//                                        


// defines
#define CHECK_RESULT( op )							\
{													\
	EMaterialResult result = op;					\
	if( result == MR_SkipPass )						\
	{												\
		delete pass;								\
		pass = NULL;								\
		continue;									\
	}												\
}


// ctor
OpenGLMaterial::OpenGLMaterial() :
	AbstractMaterial()
{
	passes_.add( new Pass );
}


// ctor
OpenGLMaterial::OpenGLMaterial( char* MaterialStr ) :
	AbstractMaterial()
{
	LoadFromMaterialStr( MaterialStr );
}


// dtor
OpenGLMaterial::~OpenGLMaterial()
{
	Clear();
}


// Clear()
void OpenGLMaterial::Clear()
{
	for( int i = 0; i < passes_.size(); ++i )
	{
		if( passes_[i] )
			delete passes_[i];
		passes_.set(i, NULL);
	}

	passes_.clear();
	SetDynamic( false );
	transparent_ = false;
}

void OpenGLMaterial::Clone( AbstractMaterial *pSrcMaterial )
{
	if( !pSrcMaterial )
		return;

	Clear();

	transparent_ = pSrcMaterial->IsTransparent();
	mergeMode_ = pSrcMaterial->GetMergeMode();

	passes_.ensureStorageAllocated( pSrcMaterial->GetPassCount() );

	for( int j=0; j<pSrcMaterial->GetPassCount(); ++j )
		passes_.add( ( static_cast<Pass*>(pSrcMaterial->GetRenderPass(j)) )->Clone() );
}


// LoadFromMaterialStr()
void OpenGLMaterial::LoadFromMaterialStr( char* str )
{
	Clear();

	Script script( str );

	// modifier merge mode
	const char * mergeMode = script.GetString( "merge_mode", "add" );
	if( strcmp( mergeMode, "add" ) == 0 )
		mergeMode_ = MM_Add;
	else if( strcmp( mergeMode, "blend" ) == 0 )
		mergeMode_ = MM_Blend;
	else if( strcmp( mergeMode, "replace" ) == 0 )
		mergeMode_ = MM_Replace;

	// loop through material passes
	int numPasses = script.ContainSection("pass");
	for( int i = 0; i < numPasses; ++i )
	{
		Script::Section *pScriptPass = script.GetSection("pass", i);

		// create the pass
		Pass * pass = new Pass;

		// setup the pass
//		CHECK_RESULT( SetupTexturing( script, *pScriptPass, *pass ) );
//		CHECK_RESULT( SetupTexturStageState( script, *pScriptPass, *pass ) );
		//CHECK_RESULT( SetupColoring( script, *pScriptPass, *pass ) );
		CHECK_RESULT( SetupAlphaBlending( script, *pScriptPass, *pass ) );
//		CHECK_RESULT( SetupAlphaTesting( script, *pScriptPass, *pass ) );
		CHECK_RESULT( SetupStencil( script, *pScriptPass, *pass ) );
		CHECK_RESULT( SetupVarious( script, *pScriptPass, *pass ) );

		// push the pass
		passes_.add( pass );

		// the material is dynamic if at least one of its pass contains at least one dynamic property
		SetDynamic( IsDynamic() || !(pass->update_.size()==0) );
	}

	// transparent flag
	transparent_ = script.GetBool( "transparent", transparent_ );
}


// Update()
void OpenGLMaterial::Update( float elapsedTime )
{
	for( int i=0; i<passes_.size(); ++i )
	{
		Pass * pass = static_cast<Pass *>( passes_[i] );
		for( int j=0; j<pass->update_.size(); ++j )
			pass->update_[j]->Update(elapsedTime);
	}
}



/*
GLint OpenGLMaterial::TextureAddressingModes( const char* mode )
{
	if( STRNICMP( "clamptoedge", mode, 11 ) == 0 )
		return  GL_CLAMP_TO_EDGE;
	else if( STRNICMP( "clamptoborder", mode, 13 ) == 0 )
		return GL_CLAMP_TO_BORDER;
	else if( STRNICMP( "mirroredrepeat", mode, 14 ) == 0 )
		return GL_MIRRORED_REPEAT;

	return GL_REPEAT;
}

GLint OpenGLMaterial::TextureFilteringModes( const char* mode )
{
	if( STRNICMP( "nearest", mode, 7 ) == 0 )
		return GL_NEAREST;
	else if( STRNICMP( "linear", mode, 6 ) == 0 )
		return GL_LINEAR;
	else if( STRNICMP( "nearest_nearest", mode, 15 ) == 0 )
		return GL_NEAREST_MIPMAP_NEAREST;
	else if( STRNICMP( "linear_nearest", mode, 14 ) == 0 )
		return GL_LINEAR_MIPMAP_NEAREST;
	else if( STRNICMP( "trilinear", mode, 9 ) == 0 )
		return GL_LINEAR_MIPMAP_LINEAR;

	return GL_LINEAR;
}
*/
/*
OpenGLMaterial::EMaterialResult OpenGLMaterial::SetupTexturing( const Script & , Script::Section & scriptPass, Pass & pass )
{
	int textureSectionNum = scriptPass.ContainSection( "texture" );

	if( textureSectionNum == 0 )
		return MR_AllRight;


	for( uint8 unit = 0; unit < (uint8)textureSectionNum; ++unit )
	{
		// set the texture property	
		Script::Section * scriptTexture = scriptPass.GetSection( "texture", unit );

		GLenum target = GL_TEXTURE_2D;

		if( scriptTexture->ContainString( "type" ) )
		{
			const char* type_name = scriptTexture->GetString( "type" );
			if( STRNICMP( "2d", type_name, 2 ) == 0 )
				target = GL_TEXTURE_2D;
			else if ( STRNICMP( "1d", type_name, 2 ) == 0 )
				target = GL_TEXTURE_1D;
			else if ( STRNICMP( "3d", type_name, 2 ) == 0 )
				target = GL_TEXTURE_3D;
			else if ( STRNICMP( "cubemap", type_name, 7 ) == 0 )
				target = GL_TEXTURE_CUBE_MAP;
			else
				Logger::getCurrentLogger()->writeToLog(String("Material Script Error!"), ELL_WARNING);
		}

		pass.AddProperty( new TextureActiveProperty( unit ) );

		//set the texture Sampler states property
		if( scriptTexture->ContainString( "wraps" ) )
		{
			const char* address_u = scriptTexture->GetString( "wraps" );
			pass.AddProperty( new TextureWrapProperty( target, GL_TEXTURE_WRAP_S, TextureAddressingModes(address_u) ) );
		}
		if( scriptTexture->ContainString( "wrapt" ) )
		{
			const char* address_v = scriptTexture->GetString( "wrapt" );
			pass.AddProperty( new TextureWrapProperty( target, GL_TEXTURE_WRAP_T, TextureAddressingModes(address_v) ) );
		}
		if( scriptTexture->ContainString( "wrapr" ) )
		{
			const char* address_w = scriptTexture->GetString( "wrapr" );
			pass.AddProperty( new TextureWrapProperty( target, GL_TEXTURE_WRAP_R, TextureAddressingModes(address_w) ) );
		}
		if( scriptTexture->ContainString( "bordercolor" ) )
		{
			const Vector4D border_color = scriptTexture->GetVec4( "bordercolor" );
			pass.AddProperty( new TextureBorderColorProperty( target, Colour::fromFloatRGBA(border_color.x, border_color.y, border_color.z, border_color.w) ) );
		}
		if( scriptTexture->ContainString( "magfilter" ) )
		{
			const char* mag_filter = scriptTexture->GetString( "magfilter" );
			pass.AddProperty( new TextureFilteringProperty( target, GL_TEXTURE_MAG_FILTER, TextureFilteringModes(mag_filter) ) );
		}
		if( scriptTexture->ContainString( "minfilter" ) )
		{
			const char* min_filter = scriptTexture->GetString( "minfilter" );
			pass.AddProperty( new TextureFilteringProperty( target, GL_TEXTURE_MIN_FILTER, TextureFilteringModes(min_filter) ) );
		}
		if( scriptTexture->ContainString( "mipmaplodbias" ) )
		{
			const float mipmaplodbias = scriptTexture->GetFloat( "mipmaplodbias" );
			pass.AddProperty( new TextureLODBiasProperty( target, mipmaplodbias ) );
		}
		if( scriptTexture->ContainString( "maxmiplevel" ) )
		{
			const int maxmiplevel = scriptTexture->GetInt( "maxmiplevel" );
			pass.AddProperty( new TextureMaxMipLevelProperty( target, maxmiplevel ) );
		}

	}

	return MR_AllRight;
}
*/

SGP_BLEND_FACTOR OpenGLMaterial::AlphaBlendFunc( const char* func )
{
	if( STRNICMP( "zero", func, 4 ) == 0 )
		return SGPBF_ZERO;
	else if( STRNICMP( "one", func, 3 ) == 0 )
		return SGPBF_ONE;
	else if( STRNICMP( "srccolor", func, 8 ) == 0 )
		return SGPBF_SRC_COLOR;
	else if( STRNICMP( "invsrccolor", func, 11 ) == 0 )
		return SGPBF_ONE_MINUS_SRC_COLOR;
	else if( STRNICMP( "destcolor", func, 9 ) == 0 )
		return SGPBF_DST_COLOR;
	else if( STRNICMP( "invdestcolor", func, 12 ) == 0 )
		return SGPBF_ONE_MINUS_DST_COLOR;
	else if( STRNICMP( "srcalpha", func, 8 ) == 0 )
		return SGPBF_SRC_ALPHA;
	else if( STRNICMP( "invsrcalpha", func, 11 ) == 0 )
		return SGPBF_ONE_MINUS_SRC_ALPHA;
	else if( STRNICMP( "destalpha", func, 9 ) == 0 )
		return SGPBF_DST_ALPHA;
	else if( STRNICMP( "invdestalpha", func, 12 ) == 0 )
		return SGPBF_ONE_MINUS_DST_ALPHA;
	else if( STRNICMP( "srcalphasaturate", func, 16 ) == 0 )
		return SGPBF_SRC_ALPHA_SATURATE;
	else
		return SGPBF_ZERO;
}

SGP_BLEND_OPERATION OpenGLMaterial::AlphaBlendOp( const char* op )
{
	if( STRNICMP( "add", op, 3 ) == 0 )
		return SGPBO_ADD;
	else if( STRNICMP( "subtract", op, 8 ) == 0 )
		return SGPBO_SUBTRACT;
	else if( STRNICMP( "revsubtract", op, 11 ) == 0 )
		return SGPBO_REVSUBTRACT;
	else if( STRNICMP( "min", op, 3 ) == 0 )
		return SGPBO_MIN;
	else if( STRNICMP( "max", op, 3 ) == 0 )
		return SGPBO_MAX;

	return SGPBO_ADD;
}

OpenGLMaterial::EMaterialResult OpenGLMaterial::SetupAlphaBlending( const Script & , Script::Section & scriptPass, Pass & pass )
{
	if( scriptPass.ContainSection( "alpha_blendseparate" ) )
	{
		// retrieve the section
		Script::Section * section = scriptPass.GetSection( "alpha_blendseparate" );

		// get the blending functions & add the property
		const char* op = section->GetString( "op", "add" );
		const char* srcrgb = section->GetString( "srcrgb", "one" );
		const char* dstrgb = section->GetString( "dstrgb", "zero" );
		const char* srcalpha = section->GetString( "srcalpha", "one" );
		const char* dstalpha = section->GetString( "dstalpha", "zero" );


		pass.AddProperty( new AlphaBlendOpProperty( AlphaBlendOp( op ) ) );

		pass.AddProperty( new AlphaBlendFuncSeparateProperty( AlphaBlendFunc( srcrgb ), AlphaBlendFunc( dstrgb ), AlphaBlendFunc( srcalpha ), AlphaBlendFunc( dstalpha ) ) );

		// set the material as transparent if alpha blending is enabled on the first pass
		if( passes_.size() == 0 )
			transparent_ = true;
	}
	else if( scriptPass.ContainSection( "alpha_blend" ) > 0 )
	{
		// retrieve the section
		Script::Section * section = scriptPass.GetSection( "alpha_blend" );
		

		// get the blending functions & add the property
		const char* op = section->GetString( "op", "add" );
		const char* src = section->GetString( "src", "srcalpha" );
		const char* dst = section->GetString( "dst", "invsrcalpha" );
		pass.AddProperty( new AlphaBlendProperty( AlphaBlendFunc( src ), AlphaBlendFunc( dst ) ) );


		pass.AddProperty( new AlphaBlendOpProperty( AlphaBlendOp( op ) ) );

		// set the material as transparent if alpha blending is enabled on the first pass
		if( passes_.size() == 0 )
			transparent_ = true;
	}



	return MR_AllRight;
}


SGP_COMPARISON_FUNC OpenGLMaterial::StencilFunc( const char* func )
{
	if( STRNICMP( "always", func, 6 ) == 0 )
		return SGPCFN_ALWAYS;
	else if( STRNICMP( "never", func, 5 ) == 0 )
		return SGPCFN_NEVER;
	else if( STRNICMP( "less", func, 4 ) == 0 )
		return SGPCFN_LESS;
	else if( STRNICMP( "equal", func, 5 ) == 0 )
		return SGPCFN_EQUAL;
	else if( STRNICMP( "lessequal", func, 9 ) == 0 )
		return SGPCFN_LESSEQUAL;
	else if( STRNICMP( "notequal", func, 8 ) == 0 )
		return SGPCFN_NOTEQUAL;
	else if( STRNICMP( "greater", func, 7 ) == 0 )
		return SGPCFN_GREATER;
	else if( STRNICMP( "greaterequal", func, 12 ) == 0 )
		return SGPCFN_GREATEREQUAL;
		
	return SGPCFN_ALWAYS;
}

GLenum OpenGLMaterial::StencilOp( const char* op )
{
	if( STRNICMP( "keep", op, 4 ) == 0 )
		return GL_KEEP;
	else if( STRNICMP( "zero", op, 4 ) == 0 )
		return GL_ZERO;
	else if( STRNICMP( "replace", op, 7 ) == 0 )
		return GL_REPLACE;
	else if( STRNICMP( "incrwrap", op, 8 ) == 0 )
		return GL_INCR_WRAP;
	else if( STRNICMP( "decrwrap", op, 8 ) == 0 )
		return GL_DECR_WRAP;
	else if( STRNICMP( "invert", op, 6 ) == 0 )
		return GL_INVERT;
	else if( STRNICMP( "incr", op, 4 ) == 0 )
		return GL_INCR;
	else if( STRNICMP( "decr", op, 4 ) == 0 )
		return GL_DECR;

	return GL_KEEP;
}

OpenGLMaterial::EMaterialResult OpenGLMaterial::SetupStencil( const Script & , Script::Section & scriptPass, Pass & pass )
{
	if( scriptPass.ContainSection( "stencil_enable" ) > 0 )
	{
		// retrieve the section
		Script::Section * section = scriptPass.GetSection( "stencil_enable" );
		
		const char* func = section->GetString( "func", "always" );
		const char* fail = section->GetString( "fail", "keep" );
		const char* zfail = section->GetString( "zfail", "keep" );
		const char* spass = section->GetString( "pass", "keep" );
		const char* ref = section->GetString( "ref", "0" );
		const char* mask = section->GetString( "mask", "0xFFFFFFFF" );
		const char* writemask = section->GetString( "writemask", "0xFFFFFFFF" );

		int sref = String(ref).getIntValue();
		int smask = String(mask+2).getHexValue32();
		int swritemask = String(writemask+2).getHexValue32();


		pass.AddProperty( 
			new StencilProperty( 
				StencilFunc(func),
				StencilOp(fail),
				StencilOp(zfail),
				StencilOp(spass),
				sref,
				smask,
				swritemask ) );

	}

	return MR_AllRight;
}

OpenGLMaterial::EMaterialResult OpenGLMaterial::SetupVarious( const Script & , Script::Section & scriptPass, Pass & pass )
{
	if( scriptPass.ContainString( "cull_mode" ) )
	{
		const char* cullMode = scriptPass.GetString( "cull_mode" );
		if( STRNICMP( "none", cullMode, 4 ) == 0 )
			pass.AddProperty( new CullingModeProperty( false, GL_BACK, GL_CCW ) );
		else if( STRNICMP( "ccw", cullMode, 3 ) == 0 )
			pass.AddProperty( new CullingModeProperty( true, GL_BACK, GL_CCW ) );
		else if( STRNICMP( "cw", cullMode, 2 ) == 0 )
			pass.AddProperty( new CullingModeProperty( true, GL_BACK, GL_CW ) );
		else
			jassertfalse;
	}
	if( scriptPass.ContainString( "color_write" ) )
	{
		const char* color_writeable = scriptPass.GetString( "color_write" );
		if( STRNICMP( "rgba", color_writeable, 4 ) == 0 )
			pass.AddProperty( new ColorMaskProperty( true, true, true, true ) );
		else if( STRNICMP( "rgb", color_writeable, 3 ) == 0 )
			pass.AddProperty( new ColorMaskProperty( true, true, true, false ) );
		else if( STRNICMP( "rb", color_writeable, 2 ) == 0 )
			pass.AddProperty( new ColorMaskProperty( true, false, true, false ) );
		else if( STRNICMP( "rba", color_writeable, 3 ) == 0 )
			pass.AddProperty( new ColorMaskProperty( true, false, true, true ) );
		else if( *color_writeable == 'a' || *color_writeable == 'A' )
			pass.AddProperty( new ColorMaskProperty( false, false, false, true ) );
		else if( *color_writeable == 'r' || *color_writeable == 'R' )
			pass.AddProperty( new ColorMaskProperty( true, false, false, false ) );	
		else if( *color_writeable == 'g' || *color_writeable == 'G' )
			pass.AddProperty( new ColorMaskProperty( false, true, false, false ) );	
		else if( *color_writeable == 'b' || *color_writeable == 'B' )
			pass.AddProperty( new ColorMaskProperty( false, false, true, false ) );
		else if( STRNICMP( "none", color_writeable, 4 ) == 0 )
			pass.AddProperty( new ColorMaskProperty( false, false, false, false ) );
		else 
			jassertfalse;
	}

	if( scriptPass.ContainString( "PolygonOffset" ) )
	{
		const char* OffsetValue = scriptPass.GetString( "PolygonOffset" );
		int ch = ',';
		char *pdest = (char*)strchr(OffsetValue, ch);
		if( pdest != NULL )
		{
			*pdest = '\0';
			pdest++;

			pass.AddProperty( new PolygonOffsetProperty( (GLfloat)atof(OffsetValue), (GLfloat)atof(pdest) ) );
		}
		else 
			jassertfalse;
	}

	if( !scriptPass.GetBool( "depth_test", true ) )
		pass.AddProperty( new DepthFuncProperty( SGPCFN_NEVER ) );		

	if( !scriptPass.GetBool( "depth_write", true ) )
		pass.AddProperty( new DepthWriteEnableProperty( false ) );

	if( scriptPass.GetBool( "depth_greater", false ) )
		pass.AddProperty( new DepthFuncProperty( SGPCFN_GREATER ) );

	if( scriptPass.GetBool( "line_drawing", false ) )
		pass.AddProperty( new FillModeProperty(GL_LINE) );

	return MR_AllRight;
}