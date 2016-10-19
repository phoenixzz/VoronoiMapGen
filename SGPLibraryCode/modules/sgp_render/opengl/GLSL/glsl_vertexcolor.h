char Shader_vertexcolor_VS_String[] = 
	"#version 330														\n"\

	"layout (location = 0) in vec3 inPosition;							\n"\
	"layout (location = 1) in vec4 inColor;								\n"\

	"uniform mat4 modelViewProjMatrix;									\n"\
	"uniform float fFarPlane;											\n"\

	"out vec4 vVertexColor;												\n"\
	"out float fVertexDepth;											\n"\
	"																	\n"\
	" void main()														\n"\
	" {																	\n"\
	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);		\n"\
	"   vVertexColor = inColor;											\n"\
	"	// store Depth as current w / farplane							\n"\
	"	fVertexDepth = gl_Position.w / fFarPlane;						\n"\
	" }																	\n"\
	"";

char Shader_vertexcolor_PS_String[] = 
	"#version 330														\n"\
	"																	\n"\

	"in vec4 vVertexColor;												\n"\
	"in float fVertexDepth;												\n"\

	"uniform vec4 vMaterialColor;										\n"\

	"layout (location = 0) out vec4 outputColor;						\n"\
	"layout (location = 1) out vec4 outputColor1;						\n"\


	"void main()														\n"\
	"{																	\n"\
	"	outputColor = vVertexColor;										\n"\
	"	outputColor.rgba *= vMaterialColor.rgba;						\n"\

	"	// Packing a [0-1] float depth value into a 4D vector			\n"\
	"	//	where each component will be 8-bits color value				\n"\
	"	const vec4 bitSh = vec4(16777216.0, 65536.0, 256.0, 1.0);		\n"\
	"	const vec4 bitMsk = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);	\n"\
	"	outputColor1 = fract(fVertexDepth * bitSh);						\n"\
	"	outputColor1 -= outputColor1.xxyz * bitMsk;						\n"\

	"}																	\n"\

	"";