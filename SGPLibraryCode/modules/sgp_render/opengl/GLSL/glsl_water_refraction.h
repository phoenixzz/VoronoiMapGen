

char Shader_waterRefraction_VS_String[] = 
	"#version 330													\n"\
	"																\n"\
	"layout (location = 0) in vec4 inPosition;						\n"\
/*
	"layout (location = 1) in vec3 inNormal;						\n"\
	"layout (location = 2) in vec3 inTangent;						\n"\
	"layout (location = 3) in vec3 inBinormal;						\n"\
	"layout (location = 4) in vec2 inCoord0;						\n"\
	"layout (location = 5) in vec2 inCoord1;						\n"\
*/
	"uniform mat4 worldViewProjMatrix;								\n"\
	"uniform float waterHeight;										\n"\

	" void main()																					\n"\
	" {																								\n"\
	" 	gl_Position = worldViewProjMatrix * vec4(inPosition.x, waterHeight, inPosition.z, 1.0);		\n"\
	" }																								\n"\
	"";


char Shader_waterRefraction_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"layout (location = 0) out vec4 outputColor;							\n"\

	"void main()															\n"\
	"{																		\n"\
    "	outputColor = vec4(0, 0, 0, 0);										\n"\
	"}																		\n"\

	"";