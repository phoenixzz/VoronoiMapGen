const char *Shader_waterRefraction_Attribute_String[] = { "inPosition" };


char Shader_waterRefraction_VS_String[] = 
	"																\n"\
	"attribute highp vec4 inPosition;								\n"\

	"uniform highp mat4 worldViewProjMatrix;						\n"\
	"uniform highp float waterHeight;								\n"\

	" void main()																					\n"\
	" {																								\n"\
	" 	gl_Position = worldViewProjMatrix * vec4(inPosition.x, waterHeight, inPosition.z, 1.0);		\n"\
	" }																								\n"\
	"";


char Shader_waterRefraction_PS_String[] = 
	"																		\n"\
	"void main()															\n"\
	"{																		\n"\
    "	gl_FragColor = vec4(0, 0, 0, 0);									\n"\
	"}																		\n"\

	"";