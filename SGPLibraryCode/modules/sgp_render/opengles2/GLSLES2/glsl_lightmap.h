const char *Shader_lightmap_Attribute_String[] = { "inPosition", "inCoord0", "inCoord1" };


char Shader_lightmap_VS_String[] = 
	"																					\n"\
	"attribute highp vec3 inPosition;													\n"\
	"attribute mediump vec2 inCoord0;													\n"\
	"attribute mediump vec2 inCoord1;													\n"\
	"																					\n"\
	"uniform highp mat4 modelViewProjMatrix;											\n"\

	"// TextureAtlas how many parts texture be divided in X and Y						\n"\
	"// TextureAtlas.xy = AtlasNbX, AtlasNbY											\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX, 1.0f/AtlasNbY									\n"\
	"uniform mediump vec4 TextureAtlas;													\n"\
	"uniform mediump vec3 TexIndexUVOffset;		// x is texture index; y,z is uv offset	\n"\
	"																					\n"\
	"varying mediump vec2 vTexCoordPass0;												\n"\
	"varying mediump vec2 vTexCoordPass1;												\n"\
	"																					\n"\
	" void main()																		\n"\
	" {																					\n"\
	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);						\n"\

	"	vTexCoordPass0 = vec2(															\n"\
	"		float( mod(TexIndexUVOffset.x, TextureAtlas.x) ) / TextureAtlas.x,			\n"\
	"		float( int(TexIndexUVOffset.x) / int(TextureAtlas.x) ) / TextureAtlas.y ) +	\n"\
	"			inCoord0.xy * TextureAtlas.zw + TexIndexUVOffset.yz;					\n"\

	"	vTexCoordPass1 = inCoord1;														\n"\
	" }																					\n"\
	"";

char Shader_lightmap_PS_String[] = 
	"																					\n"\
	"varying mediump vec2 vTexCoordPass0;												\n"\
	"varying mediump vec2 vTexCoordPass1;												\n"\
																				
																				
	"uniform sampler2D gSampler0;			// Diffuse									\n"\
	"uniform sampler2D gSampler1;			// Lightmap									\n"\
	"uniform lowp vec4 vMaterialColor;			 										\n"\
	"uniform lowp vec4 SunColor;			// ambient color and Intensity				\n"\
																				
	"void main()																		\n"\
	"{																					\n"\
	"	gl_FragColor = texture2D(gSampler0, vTexCoordPass0);							\n"\

	"	lowp vec4 lightmap = texture2D(gSampler1, vTexCoordPass1);						\n"\
	"	gl_FragColor.rgb *= lightmap.rgb * vMaterialColor.rgb;							\n"\
	"	gl_FragColor.rgb += SunColor.rgb * SunColor.a * lightmap.a;			//Ambient	\n"\
	"	gl_FragColor.a *= vMaterialColor.a;												\n"\

	"}																					\n"\

	"";

