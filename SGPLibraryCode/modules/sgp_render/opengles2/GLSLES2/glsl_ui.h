const char *Shader_ui_Attribute_String[] = { "inPosition", "inColor", "inCoord" };

char Shader_ui_VS_String[] = 
	"												\n"\
	"attribute highp vec4 inPosition;				\n"\
	"attribute lowp vec4 inColor;					\n"\
	"attribute mediump vec2 inCoord;				\n"\

	"varying mediump vec2 vTexCoordPass;			\n"\
	"varying lowp vec4 vVertexColor;				\n"\

	" void main()									\n"\
	" {												\n"\
	" 	gl_Position = inPosition;					\n"\
	" 	vTexCoordPass = inCoord;					\n"\
	"   vVertexColor = inColor;						\n"\
	" }												\n"\
	"";

char Shader_ui_PS_String[] = 
	"												\n"\
	"varying mediump vec2 vTexCoordPass;			\n"\
	"varying lowp vec4 vVertexColor;				\n"\

	"uniform sampler2D gSampler0;					\n"\

	"void main()									\n"\
	"{												\n"\
	"	gl_FragColor = texture2D(gSampler0, vTexCoordPass) * vVertexColor;	\n"\
	"}												\n"\

	"";

