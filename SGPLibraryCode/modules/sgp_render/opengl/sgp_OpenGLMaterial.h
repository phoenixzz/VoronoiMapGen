#ifndef __SGP_OPENGLMATERIAL_HEADER__
#define __SGP_OPENGLMATERIAL_HEADER__

// Material
class OpenGLMaterial : public AbstractMaterial
{
public :
	OpenGLMaterial();			// creates a default material containing 1 empty pass
	OpenGLMaterial( char* MaterialStr );
	virtual ~OpenGLMaterial();

	// public interface
	virtual void Clear();
	virtual void LoadFromMaterialStr( char* str );
	virtual void Update( float elapsedTime );
	virtual void Clone( AbstractMaterial *pSrcMaterial );

public:
	class Pass : public RenderPass
	{
	public :
		// ctor(s) & dtor
		Pass();
		virtual ~Pass();
		Pass * Clone();

		// public interface
		//virtual void UpdateTexture(uint32 texID, uint8 unit);
		virtual void AddProperty( ISGPMaterialProperty * property );
		virtual void PreRender();
		virtual void PostRender();

		// public members
		Array<ISGPMaterialProperty *> begin_;			// begin_ contains all the material properties
		Array<ISGPMaterialProperty *> end_;				// end_ contains a subset of begin_
		Array<ISGPMaterialProperty *> update_;			// update_ contains a subset of begin_
	};

private :
	// enumerates actions to take when a feature is not supported on the user's hardware
	enum EMaterialResult
	{
		MR_AllRight,	// everything's all right
		MR_SkipPass,	// skip the current render pass
		MR_Fallback		// fallback to the specified material
	};

	
    SGP_DECLARE_NON_COPYABLE (OpenGLMaterial)

	class Script
	{
	public :
		// public structures
		class Section
		{
		public :
			// ctor(s) & dtor
			~Section();

			// public types
			typedef Array<Section*>		SectionList;
			typedef Array<const char*>	StackName;

			// public interface

			int				ContainSection( const char* id );
			Section*		GetSection( const char* name, int ID = 0 );
			bool			ContainBool   ( const char* id );
			bool			GetBool( const char* id, bool def = false );
			bool			ContainString ( const char* id );
			const char*		GetString( const char* id, const char* def = "" );
			int				GetInt( const char* id, int def = 0 );
			Vector4D		GetVec4( const char* id, const Vector4D & def = Vector4D(0, 0, 0) );


			float GetFloat( const char* id, float def = 0.f );

		private:
			void CharToUpper( char* str );
			static void CharTrimLeftRight( char* str );
			// Searches for a substring first not within this SrcStr, the index from which the search should proceed  
			int Char_Find_first_not_of( const char* SrcStr, const char* SubStr, int index=0 );
			// Searches for a substring within this SrcStr, the index from which the search should proceed 
			// Return : the index of the first occurrence of this substring, or -1 if it's not found.
			int Char_Find_first_of( const char* SrcStr, const char SubStr, int index=0 );
			// Searches for a substring first not within this SrcStr, the index from which the search should proceed  
			// (working backwards from the end of the string)
			int Char_Find_last_not_of( const char* SrcStr, const char* SubStr, int str_len );
		
		private :
			// ctor(s) & dtor
			Section();
			Section( char* MaterialStr );

			// private types
			StackName		LineID_;
			StackName		LineValue_;

			SectionList		sections_;
			StackName		names_;

			// private helper structures
			struct BuildInfo
			{
				inline BuildInfo( char* MaterialStr, bool global )
					: Material_Str( MaterialStr ), global( global )
				{
				}

				char* Material_Str;
				bool global;
			};

			// private helper functions
			void LoadFromMaterialStr( BuildInfo & buildInfo );
			void ReadSection( BuildInfo & buildInfo, char* id );
			void ReadValue( BuildInfo & buildInfo, char* line, int index );
			static bool IsAlphaNumeric( char c );
			static bool GetLine( BuildInfo & buildInfo, char** line );

			// friendship
			friend class Script;
		};


		// ctor(s) & dtor
		Script( char* MaterialStr );
		~Script();

		// public interface


		inline int GetInt( const char* id, int def = 0 ) { return global_->GetInt( id, def ); }
		inline float GetFloat( const char* id, float def = 0.f ) { return global_->GetFloat( id, def ); }
		inline Vector4D GetVec4( const  char* id, const Vector4D & def = Vector4D(0, 0, 0) ) { return global_->GetVec4( id, def ); }

		inline int			ContainSection( const char* id ) { return global_->ContainSection( id ); }
		inline Script::Section* GetSection( const char* name, int ID = 0  ) { return global_->GetSection( name, ID ); }
		inline bool			ContainBool   ( const char* id ) { return global_->ContainBool( id ); }
		inline bool			GetBool( const char* id, bool def = false ) { return global_->GetBool( id, def ); }
		inline bool			ContainString ( const char* id ) { return global_->ContainString( id ); }
		inline const char*	GetString( const char* id, const char* def = "" ) { return global_->GetString( id, def ); }

	private :
		Script::Section * global_;
	};
private:
	//EMaterialResult SetupTexturing( const OpenGLMaterial::Script & script, OpenGLMaterial::Script::Section & scriptPass, Pass & pass );
	EMaterialResult SetupAlphaBlending( const OpenGLMaterial::Script & script, OpenGLMaterial::Script::Section & scriptPass, Pass & pass );
	EMaterialResult SetupStencil( const OpenGLMaterial::Script & script, OpenGLMaterial::Script::Section & scriptPass, Pass & pass );
	EMaterialResult SetupVarious( const OpenGLMaterial::Script & script, OpenGLMaterial::Script::Section & scriptPass, Pass & pass );


public:
	static SGP_BLEND_FACTOR AlphaBlendFunc( const char* func );
	static SGP_BLEND_OPERATION AlphaBlendOp( const char* op );
	static GLenum StencilOp( const char* op );
	static SGP_COMPARISON_FUNC StencilFunc( const char* func );
	//static GLint TextureAddressingModes( const char* mode );
	//static GLint TextureFilteringModes( const char* mode );

};

#endif		// __SGP_OPENGLMATERIAL_HEADER__