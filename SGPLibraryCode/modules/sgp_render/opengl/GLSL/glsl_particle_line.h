
char Shader_particleLine_VS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) in vec3 inPosition;								\n"\
	"layout (location = 1) in vec4 inColor;									\n"\
	"																		\n"\
	"uniform mat4 modelViewProjMatrix;										\n"\
	"																		\n"\
	"out vec4 vVertexColorPass;												\n"\
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
	"#version 330																\n"\
	"																			\n"\
	"in vec4 vVertexColorPass;													\n"\
	"out vec4 outputColor;														\n"\

	"uniform float mode;														\n"\

	"void main()																\n"\
	"{																			\n"\
	"																			\n"\
	"	// The mode controls whether the particle is							\n"\
	"	// alpha blended or additive 0 = additive, 1 = alpha blended,			\n"\
	"	// or an intermediate value for a mix of both							\n"\

	"	outputColor = vec4( vVertexColorPass.rgb * vVertexColorPass.a, mode );	\n"\
	"}																			\n"\

	"";
