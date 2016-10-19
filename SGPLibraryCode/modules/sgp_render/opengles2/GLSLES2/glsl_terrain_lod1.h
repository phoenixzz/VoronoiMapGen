const char *Shader_terrain_LOD1_Attribute_String[] = { "inPosition", "inNormal", "inCoord0", "inCoord1" };


char Shader_terrain_LOD1_VS_String[] = 
	"														\n"\
	"attribute highp vec4 inPosition;						\n"\
	"attribute mediump vec3 inNormal;						\n"\
	"attribute mediump vec2 inCoord0;						\n"\
	"attribute mediump vec2 inCoord1;						\n"\

	"uniform highp mat4 worldViewProjMatrix;				\n"\

	"varying mediump vec3 vNormal;							\n"\
	"varying mediump vec2 vTexCoord0;						\n"\
	"varying mediump vec2 vTexCoord1;						\n"\

	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	" 	gl_Position = worldViewProjMatrix * vec4(inPosition.xyz, 1.0);		\n"\

	"	// Calculate the normal vector against the world matrix only and	\n"\
	"	// then normalize the final value.									\n"\
	"	vNormal = normalize(inNormal);										\n"\
	" 	vTexCoord0 = inCoord0;												\n"\
	"	vTexCoord1 = inCoord1;												\n"\

	" }																		\n"\
	"";

char Shader_terrain_LOD1_PS_String[] = 
	"																		\n"\
	"varying mediump vec3 vNormal;											\n"\
	"varying mediump vec2 vTexCoord0;										\n"\
	"varying mediump vec2 vTexCoord1;										\n"\

	"uniform sampler2D gSamplerMiniMap;										\n"\
	"uniform sampler2D gSamplerLightmap;									\n"\

	"uniform lowp vec4 SunColor;											\n"\
    "uniform lowp vec3 SunDirection;										\n"\


	"void main()																	\n"\
	"{																				\n"\
	"	lowp vec4 DiffuseColor = texture2D(gSamplerMiniMap, vTexCoord1);			\n"\

	"	// Invert the light direction for calculations.								\n"\
    "	// Calculate the light based on the bump map normal value.					\n"\
    "	lowp float lightIntensity = clamp(dot(vNormal, -SunDirection), 0.0, 1.0);	\n"\

	"	// lightmap color															\n"\
	"	lowp vec4 lightmap = texture2D(gSamplerLightmap, vTexCoord1);				\n"\


    "	// Determine the final diffuse color based on the diffuse color and			\n"\
	"	// the amount of light intensity.											\n"\
    "	gl_FragColor = DiffuseColor * lightIntensity * SunColor * lightmap.a;		\n"\
	"	gl_FragColor.rgb += DiffuseColor.rgb * lightmap.rgb;						\n"\
	"	gl_FragColor = clamp( gl_FragColor, 0.0, 1.0);								\n"\
	"	gl_FragColor.w = 1.0;														\n"\
	
	"}																				\n"\

	"";