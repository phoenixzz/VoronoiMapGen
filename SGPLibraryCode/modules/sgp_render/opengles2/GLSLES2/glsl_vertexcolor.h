const char *Shader_vertexcolor_Attribute_String[] = { "inPosition", "inColor" };

char Shader_vertexcolor_VS_String[] = 
	"																	\n"\
	"attribute highp vec3 inPosition;									\n"\
	"attribute lowp vec4 inColor;										\n"\

	"uniform highp mat4 modelViewProjMatrix;							\n"\

	"varying lowp vec4 vVertexColor;									\n"\
	"																	\n"\
	" void main()														\n"\
	" {																	\n"\
	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);		\n"\
	"   vVertexColor = inColor;											\n"\
	" }																	\n"\
	"";

char Shader_vertexcolor_PS_String[] = 
	"																	\n"\
	"varying lowp vec4 vVertexColor;									\n"\

	"uniform lowp vec4 vMaterialColor;									\n"\

	"void main()														\n"\
	"{																	\n"\
	"	gl_FragColor = vVertexColor;									\n"\
	"	gl_FragColor.rgba *= vMaterialColor.rgba;						\n"\

	"}																	\n"\

	"";