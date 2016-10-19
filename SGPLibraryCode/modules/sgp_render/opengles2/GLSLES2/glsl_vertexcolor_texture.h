const char *Shader_vertexcolor_texture_Attribute_String[] = { "inPosition", "inColor", "inCoord" };

char Shader_vertexcolor_texture_VS_String[] = 
	"																						\n"\
	"attribute highp vec3 inPosition;														\n"\
	"attribute lowp vec4 inColor;															\n"\
	"attribute mediump vec2 inCoord;														\n"\

	"uniform highp mat4 modelViewProjMatrix;												\n"\

	"// TextureAtlas how many parts texture be divided in X and Y							\n"\
	"// TextureAtlas.xy = AtlasNbX, AtlasNbY												\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX, 1.0f/AtlasNbY										\n"\
	"uniform mediump vec4 TextureAtlas;														\n"\
	"uniform mediump vec3 TexIndexUVOffset;		//x is texture index; y,z is uv offset		\n"\

	"varying lowp vec4 vVertexColor;														\n"\
	"varying mediump vec2 vTexCoordPass;													\n"\
	"																						\n"\
	" void main()																			\n"\
	" {																						\n"\
	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);							\n"\
	"   vVertexColor = inColor;																\n"\

	"	vTexCoordPass = vec2(																\n"\
	"		float( mod(TexIndexUVOffset.x, TextureAtlas.x) ) / TextureAtlas.x,				\n"\
	"		float( int(TexIndexUVOffset.x) / int(TextureAtlas.x) ) / TextureAtlas.y ) +		\n"\
	"			inCoord.xy * TextureAtlas.zw + TexIndexUVOffset.yz;							\n"\

	" }																						\n"\
	"";

char Shader_vertexcolor_texture_PS_String[] = 
	"																		\n"\
	"varying lowp vec4 vVertexColor;										\n"\
	"varying mediump vec2 vTexCoordPass;									\n"\

	"uniform sampler2D gSampler0;											\n"\
	"uniform lowp vec4 vMaterialColor;										\n"\

	"void main()															\n"\
	"{																		\n"\
	"	gl_FragColor = texture2D(gSampler0, vTexCoordPass) * vVertexColor;	\n"\
	
	"	gl_FragColor.rgb *= vMaterialColor.rgb;								\n"\
	"	gl_FragColor.a *= vMaterialColor.a;									\n"\
	
	"}																		\n"\

	"";