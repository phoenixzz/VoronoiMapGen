
char Shader_particleQuadTexAtlas_VS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) in vec3 inPosition;								\n"\
	"layout (location = 1) in vec4 inColor;									\n"\
	"layout (location = 2) in vec2 inUV;									\n"\
	"layout (location = 3) in float inTexIndex;								\n"\

	"uniform mat4 modelViewProjMatrix;										\n"\

	"// TextureAtlas define how many parts texture be divided in X and Y	\n"\
	"// TextureAtlas.xy = AtlasNbX,AtlasNbY									\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX,1.0f/AtlasNbY						\n"\
	"uniform vec4 TextureAtlas;												\n"\
	"																		\n"\
	"out vec4 vVertexColorPass;												\n"\
	"out vec2 vVertexOutUV;													\n"\
	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	"	vVertexColorPass = inColor;											\n"\

	"	vVertexOutUV = vec2(														\n"\
	"		float( int(inTexIndex) % int(TextureAtlas.x) ) / TextureAtlas.x,		\n"\
	"		float( int(inTexIndex) / int(TextureAtlas.x) ) / TextureAtlas.y ) +		\n"\
	"			inUV.xy * TextureAtlas.zw;											\n"\

	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);			\n"\
	" }																		\n"\
	"";


// Add-alpha pixel shader. To be used in conjunction
// with the blend factors {One, InverseSourceAlpha}
char Shader_particleQuadTexAtlas_PS_String[] = 
	"#version 330								\n"\
	"											\n"\
	"in vec4 vVertexColorPass;					\n"\
	"in vec2 vVertexOutUV;						\n"\

	"out vec4 outputColor;						\n"\

	"uniform sampler2D gSampler0;				\n"\
	"uniform float mode;						\n"\

	"void main()																\n"\
	"{																			\n"\
	"   vec3 colourOut = vVertexColorPass.rgb * vVertexColorPass.a;				\n"\
	"	float alphaOut = mode;													\n"\
	"																			\n"\
	"	vec4 TexColor = texture2D(gSampler0, vVertexOutUV);						\n"\
	"	colourOut.rgb *= TexColor.rgb * TexColor.a;								\n"\
	"	alphaOut *= vVertexColorPass.a * TexColor.a;							\n"\
	
	"	// The mode controls whether the particle is							\n"\
	"	// alpha blended or additive 0 = additive, 1 = alpha blended,			\n"\
	"	// or an intermediate value for a mix of both							\n"\

	"	outputColor = vec4( colourOut, alphaOut );								\n"\
	"}																			\n"\

	"";
