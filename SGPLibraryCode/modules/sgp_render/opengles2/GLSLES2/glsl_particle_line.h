const char *Shader_particleLine_Attribute_String[] = { "inPosition", "inColor" };

char Shader_particleLine_VS_String[] = 
	"																		\n"\
	"attribute highp vec3 inPosition;										\n"\
	"attribute lowp vec4 inColor;											\n"\
	"																		\n"\
	"uniform highp mat4 modelViewProjMatrix;								\n"\
	"																		\n"\
	"varying lowp vec4 vVertexColorPass;									\n"\
	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	"	vVertexColorPass = inColor;											\n"\
	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);			\n"\
	" }																		\n"\
	"";


// Add-alpha pixel shader. To be used in conjunction
// with the blend factors {One, InverseSourceAlpha}
char Shader_particleLine_PS_String[] = 
	"																			\n"\
	"varying lowp vec4 vVertexColorPass;										\n"\

	"uniform lowp float mode;													\n"\

	"void main()																\n"\
	"{																			\n"\
	"																			\n"\
	"	// The mode controls whether the particle is							\n"\
	"	// alpha blended or additive 0 = additive, 1 = alpha blended,			\n"\
	"	// or an intermediate value for a mix of both							\n"\

	"	gl_FragColor = vec4( vVertexColorPass.rgb * vVertexColorPass.a, mode );	\n"\
	"}																			\n"\

	"";
