char Normal_VS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) in vec4 inPosition;								\n"\
	"layout (location = 1) in vec4 inColor;									\n"\
	"layout (location = 2) in vec2 inCoord;									\n"\
	"																		\n"\
	"out vec2 vTexCoordPass;												\n"\
	"out vec4 vVertexColor;													\n"\
	"uniform mat4 modelMatrix;												\n"\
	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	" 	gl_Position = modelMatrix * inPosition;								\n"\
	" 	vTexCoordPass = inCoord;											\n"\
	"   vVertexColor = inColor;												\n"\
	" }																		\n"\
	"";

char Normal_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\

	"in vec2 vTexCoordPass;													\n"\
	"in vec4 vVertexColor;													\n"\
	"out vec4 outputColor;													\n"\

	"uniform sampler2D gSampler0;											\n"\

	"void main()															\n"\
	"{																		\n"\
	"	outputColor = texture2D(gSampler0, vTexCoordPass) * vVertexColor;	\n"\
	"	outputColor.a = 1.0;												\n"\
	"}																		\n"\

	"";