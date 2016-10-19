const char *Shader_grassRender_Attribute_String[] = { "inModelPosition", "inLocalUV", "inNormal", "inColor" };


char Shader_grassRender_VS_String [] =
	"																								\n"\
	"attribute highp vec4 inModelPosition;	// inModelPosition.w is tiled texture index				\n"\
	"attribute mediump vec4 inLocalUV;		// inLocalUV.zw is lightmap uv												\n"\
	"attribute mediump vec4 inNormal;		// inNormal.w is grass scale							\n"\
	"attribute lowp vec4 inColor;																	\n"\


	"// TextureAtlas define how many parts texture be divided in X and Y							\n"\
	"// TextureAtlas.xy = AtlasNbX,AtlasNbY															\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX,1.0f/AtlasNbY												\n"\
	"uniform mediump vec4 TextureAtlas;																\n"\

	"uniform highp mat4 ViewProjMatrix;																\n"\

	"varying mediump vec2 vOutTexCoord;																\n"\
	"varying mediump vec2 vOutTexCoordLM;															\n"\
	"varying lowp vec4 vOutColor;																	\n"\
	"varying mediump vec3 vOutNormal;																\n"\

	" void main()																					\n"\
	" {																								\n"\
	"	mediump vec3 vTerrainNormal = normalize(inNormal.xyz);										\n"\

	" 	gl_Position = ViewProjMatrix * vec4(inModelPosition.xyz, 1.0);								\n"\

	"	vOutTexCoord = vec2(																		\n"\
	"		float( mod(inModelPosition.w, TextureAtlas.x) ) / TextureAtlas.x,						\n"\
	"		float( int(inModelPosition.w) / int(TextureAtlas.x) ) / TextureAtlas.y ) +				\n"\
	"			inLocalUV.xy * TextureAtlas.zw;														\n"\
	"	vOutTexCoordLM = inLocalUV.zw;																\n"\
	
	"	vOutColor = inColor;																		\n"\
	"	vOutNormal = vTerrainNormal;														\n"\
	" }																								\n"\
	"";


char Shader_grassRender_PS_String [] =
	"																						\n"\
	"varying mediump vec2 vOutTexCoord;														\n"\
	"varying mediump vec2 vOutTexCoordLM;													\n"\
	"varying lowp vec4 vOutColor;															\n"\
	"varying mediump vec3 vOutNormal;														\n"\

	"uniform sampler2D gSampler0;			// Grass Texture								\n"\
	"uniform sampler2D gSamplerLightmap;	// Lightmap	Texture								\n"\

	"uniform lowp vec4 SunColor;															\n"\
    "uniform lowp vec3 SunDirection;														\n"\
	
	"void main()																			\n"\
	"{																						\n"\
	"	lowp vec4 grassColor = texture2D(gSampler0, vOutTexCoord);							\n"\
	"	lowp vec4 DiffuseColor = grassColor * vOutColor;									\n"\

	"	lowp vec4 LightmapColor = texture2D(gSamplerLightmap, vOutTexCoordLM);				\n"\

	"	// Invert the light direction for calculations.										\n"\
    "	// Calculate the light based on the grass normal value.								\n"\
    "	lowp float lightIntensity = clamp(dot(vOutNormal, -SunDirection), 0.0, 1.0);		\n"\

	"	// Determine the final color based on  the amount of light intensity.				\n"\
	"	gl_FragColor.rgb = DiffuseColor.rgb * lightIntensity * SunColor.rgb *				\n"\
	"		LightmapColor.a * SunColor.rgb * 2.0;											\n"\
	"	gl_FragColor.rgb += DiffuseColor.rgb * LightmapColor.rgb;							\n"\

	"	gl_FragColor = clamp( gl_FragColor, 0.0, 1.0);										\n"\
	"	gl_FragColor.a = DiffuseColor.a;													\n"\
	"}																						\n"\

	"";