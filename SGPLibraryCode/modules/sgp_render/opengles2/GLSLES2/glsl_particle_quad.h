const char *Shader_particleQuadTexAtlas_Attribute_String[] = { "inPosition", "inColor", "inUV", "inTexIndex" };

char Shader_particleQuadTexAtlas_VS_String[] = 
	"																		\n"\
	"attribute highp vec3 inPosition;										\n"\
	"attribute lowp vec4 inColor;											\n"\
	"attribute mediump vec2 inUV;											\n"\
	"attribute mediump float inTexIndex;									\n"\

	"uniform highp mat4 modelViewProjMatrix;								\n"\

	"// TextureAtlas define how many parts texture be divided in X and Y	\n"\
	"// TextureAtlas.xy = AtlasNbX, AtlasNbY								\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX,1.0f/AtlasNbY						\n"\
	"uniform mediump vec4 TextureAtlas;										\n"\
	"																		\n"\
	"varying lowp vec4 vVertexColorPass;									\n"\
	"varying mediump vec2 vVertexOutUV;										\n"\
	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	"	vVertexColorPass = inColor;											\n"\

	"	vVertexOutUV = vec2(														\n"\
	"		float( mod(inTexIndex, TextureAtlas.x) ) / TextureAtlas.x,				\n"\
	"		float( int(inTexIndex) / int(TextureAtlas.x) ) / TextureAtlas.y ) +		\n"\
	"			inUV.xy * TextureAtlas.zw;											\n"\

	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);			\n"\
	" }																		\n"\
	"";


// Add-alpha pixel shader. To be used in conjunction
// with the blend factors {One, InverseSourceAlpha}
char Shader_particleQuadTexAtlas_PS_String[] = 
	"											\n"\
	"varying lowp vec4 vVertexColorPass;		\n"\
	"varying mediump vec2 vVertexOutUV;			\n"\

	"uniform sampler2D gSampler0;				\n"\
	"uniform lowp float mode;					\n"\

	"void main()																\n"\
	"{																			\n"\
	"   lowp vec3 colourOut = vVertexColorPass.rgb * vVertexColorPass.a;		\n"\
	"	lowp float alphaOut = mode;												\n"\
	"																			\n"\
	"	lowp vec4 TexColor = texture2D(gSampler0, vVertexOutUV);				\n"\
	"	colourOut.rgb *= TexColor.rgb * TexColor.a;								\n"\
	"	alphaOut *= vVertexColorPass.a * TexColor.a;							\n"\
	
	"	// The mode controls whether the particle is							\n"\
	"	// alpha blended or additive 0 = additive, 1 = alpha blended,			\n"\
	"	// or an intermediate value for a mix of both							\n"\

	"	gl_FragColor = vec4( colourOut, alphaOut );								\n"\
	"}																			\n"\

	"";
