const char *Shader_terrain_VeryHigh_Attribute_String[] = { "inPosition", "inNormal", "inCoord0", "inCoord1" };


char Shader_terrain_VeryHigh_VS_String[] = 
	"																\n"\
	"attribute highp vec4 inPosition;								\n"\
	"attribute mediump vec3 inNormal;								\n"\
	"attribute mediump vec2 inCoord0;								\n"\
	"attribute mediump vec2 inCoord1;								\n"\

	"uniform highp mat4 worldViewProjMatrix;						\n"\

	"varying mediump vec3 vNormal;									\n"\
	"varying mediump vec2 vTexCoord0;								\n"\
	"varying mediump vec2 vTexCoord1;								\n"\

	" void main()															\n"\
	" {																		\n"\
	" 	gl_Position = worldViewProjMatrix * vec4(inPosition.xyz, 1.0);		\n"\
	"																		\n"\
	"	// Calculate the normal vector against the world matrix only and	\n"\
	"	// then normalize the final value.									\n"\
	"	vNormal = normalize(inNormal);										\n"\

	" 	vTexCoord0 = inCoord0;												\n"\
	"	vTexCoord1 = inCoord1;												\n"\
	" }																		\n"\
	"";

char Shader_terrain_VeryHigh_PS_String[] = 
	"																		\n"\
	"varying mediump vec3 vNormal;											\n"\
	"varying mediump vec2 vTexCoord0;										\n"\
	"varying mediump vec2 vTexCoord1;										\n"\

	"uniform sampler2D gSamplerDiffuse0;									\n"\
	"uniform sampler2D gSamplerDiffuse1;									\n"\
	"uniform sampler2D gSamplerDiffuse2;									\n"\
	"uniform sampler2D gSamplerDiffuse3;									\n"\
	"uniform sampler2D gSamplerAlphaBlend;									\n"\
	"uniform sampler2D gSamplerDetail;										\n"\
	"uniform sampler2D gSamplerLightmap;									\n"\

	"uniform lowp vec4 SunColor;													\n"\
    "uniform lowp vec3 SunDirection;												\n"\

	"uniform mediump float RenderFlag;												\n"\

	"void main()																	\n"\
	"{																				\n"\
	"	lowp vec4 Diffuse1, Diffuse2, Diffuse3;										\n"\
	"	lowp vec4 blendValue = texture2D(gSamplerAlphaBlend, vTexCoord1);			\n"\
	"	lowp vec4 Diffuse0 = texture2D(gSamplerDiffuse0, vTexCoord0);				\n"\
	"	// Set the base color to the first color texture.							\n"\
	"	lowp vec4 DiffuseColor = Diffuse0;											\n"\
	"																				\n"\
	"	if( mod( RenderFlag, 32.0 ) >= 16.0 )										\n"\
	"	{																			\n"\
	"		Diffuse1 = texture2D(gSamplerDiffuse1, vTexCoord0);						\n"\
	"		// Add the second layer using the red channel of the alpha map.			\n"\
    "		DiffuseColor = mix(DiffuseColor, Diffuse1, blendValue.x);				\n"\
	"	}																			\n"\
	"	if( mod( RenderFlag, 64.0 ) >= 32.0 )										\n"\
	"	{																			\n"\
	"		Diffuse2 = texture2D(gSamplerDiffuse2, vTexCoord0);						\n"\
	"		// Add the third layer using the green channel of the alpha map.		\n"\
    "		DiffuseColor = mix(DiffuseColor, Diffuse2, blendValue.y);				\n"\
	"	}																			\n"\
	"	if( mod( RenderFlag, 128.0 ) >= 64.0 )										\n"\
	"	{																			\n"\
	"		Diffuse3 = texture2D(gSamplerDiffuse3, vTexCoord0);						\n"\
	"		// Add the fourth layer using the blue channel of the alpha map.		\n"\
    "		DiffuseColor = mix(DiffuseColor, Diffuse3, blendValue.z);				\n"\
	"	}																			\n"\
	"																				\n"\
	
	"	// Have Detail map?															\n"\
	"	if( mod( RenderFlag, 2.0 ) == 1.0 )											\n"\
	"	{																			\n"\
	"		// Sample the pixel color from the detail map texture using				\n"\
	"		// the sampler at this texture coordinate location.						\n"\
	"		lowp vec4 detailColor = texture2D(gSamplerDetail, vTexCoord0.xy*4.0);	\n"\
																					
    "		// Combine the ground texture and the detail texture.					\n"\
	"		//Also multiply in the detail brightness.								\n"\
    "		DiffuseColor = DiffuseColor * detailColor * 1.8;						\n"\
	"	}																			\n"\

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