char Shader_vertexcolor_lightmap_alphatest_VS_String[] = 
	"#version 330																			\n"\

	"layout (location = 0) in vec3 inPosition;												\n"\
	"layout (location = 1) in vec4 inColor;													\n"\
	"layout (location = 2) in vec2 inCoord0;												\n"\
	"layout (location = 3) in vec2 inCoord1;												\n"\

	"uniform mat4 modelViewProjMatrix;														\n"\
	"uniform float fFarPlane;																\n"\
	"// TextureAtlas how many parts texture be divided in X and Y							\n"\
	"// TextureAtlas.xy = AtlasNbX, AtlasNbY												\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX, 1.0f/AtlasNbY										\n"\
	"uniform vec4 TextureAtlas;																\n"\
	"uniform vec3 TexIndexUVOffset;		//x is texture index; y,z is uv offset				\n"\

	"out vec4 vVertexColor;																	\n"\
	"out vec2 vTexCoordPass0;																\n"\
	"out vec2 vTexCoordPass1;																\n"\
	"out float fVertexDepth;																\n"\
	"																						\n"\
	" void main()																			\n"\
	" {																						\n"\
	" 	gl_Position = modelViewProjMatrix * vec4(inPosition, 1.0);							\n"\
	"   vVertexColor = inColor;																\n"\

	"	vTexCoordPass0 = vec2(																\n"\
	"		float( int(TexIndexUVOffset.x) % int(TextureAtlas.x) ) / TextureAtlas.x,		\n"\
	"		float( int(TexIndexUVOffset.x) / int(TextureAtlas.x) ) / TextureAtlas.y ) +		\n"\
	"			inCoord0.xy * TextureAtlas.zw + TexIndexUVOffset.yz;						\n"\
	"	vTexCoordPass1 = inCoord1;															\n"\

	"	// store Depth as current w / farplane												\n"\
	"	fVertexDepth = gl_Position.w / fFarPlane;											\n"\
	" }																						\n"\
	"";

char Shader_vertexcolor_lightmap_alphatest_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\

	"in vec4 vVertexColor;													\n"\
	"in vec2 vTexCoordPass0;												\n"\
	"in vec2 vTexCoordPass1;												\n"\
	"in float fVertexDepth;													\n"\

	"layout (location = 0) out vec4 outputColor;							\n"\
	"layout (location = 1) out vec4 outputColor1;							\n"\

	"uniform sampler2D gSampler0;			// Diffuse						\n"\
	"uniform sampler2D gSampler1;			// Lightmap						\n"\
	"uniform vec4 vMaterialColor;											\n"\
	"uniform vec4 SunColor;					// ambient color and Intensity	\n"\

	"void main()															\n"\
	"{																		\n"\
	"	outputColor = texture2D(gSampler0, vTexCoordPass0) * vVertexColor;	\n"\
	"	// Alpha-test														\n"\
	"	if( outputColor.a < 0.5 )											\n"\
	"	{																	\n"\
	"		discard;														\n"\
	"	}																	\n"\

	"	vec4 lightmap = texture2D(gSampler1, vTexCoordPass1);				\n"\
	"	outputColor.rgb *= lightmap.rgb * vMaterialColor.rgb;				\n"\
	"	outputColor.rgb += SunColor.rgb * SunColor.a * lightmap.a; //Ambient\n"\
	"	outputColor.a *= vMaterialColor.a;									\n"\

	"	// Packing a [0-1] float depth value into a 4D vector				\n"\
	"	//	where each component will be 8-bits color value					\n"\
	"	const vec4 bitSh = vec4(16777216.0, 65536.0, 256.0, 1.0);			\n"\
	"	const vec4 bitMsk = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);		\n"\
	"	outputColor1 = fract(fVertexDepth * bitSh);							\n"\
	"	outputColor1 -= outputColor1.xxyz * bitMsk;							\n"\

	"}																		\n"\

	"";