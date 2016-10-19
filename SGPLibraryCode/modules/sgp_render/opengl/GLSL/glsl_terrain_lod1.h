

char Shader_terrain_LOD1_VS_String[] = 
	"#version 330													\n"\
	"																\n"\
	"layout (location = 0) in vec4 inPosition;						\n"\
	"layout (location = 1) in vec3 inNormal;						\n"\
	"layout (location = 2) in vec2 inCoord0;						\n"\
	"layout (location = 3) in vec2 inCoord1;						\n"\
	"layout (location = 4) in vec3 inTangent;						\n"\
	"layout (location = 5) in vec3 inBinormal;						\n"\
	"																\n"\
	"uniform mat4 worldViewProjMatrix;								\n"\
	"uniform float fFarPlane;										\n"\

	"out vec4 vNormal;				// w save as depth				\n"\
	"out vec2 vTexCoord1;											\n"\

	"																		\n"\
	" void main()															\n"\
	" {																		\n"\
	" 	gl_Position = worldViewProjMatrix * vec4(inPosition.xyz, 1.0);		\n"\

	"	// Calculate the normal vector against the world matrix only and	\n"\
	"	// then normalize the final value.									\n"\
	"	//vNormal = mat3x3(worldMatrix) * inNormal;							\n"\
	"	vNormal.xyz = normalize(inNormal);									\n"\
	"	// store Depth into vNormal w channel as current w / farplane		\n"\
	"	vNormal.w = gl_Position.w / fFarPlane;								\n"\
	
	"	vTexCoord1 = inCoord1;												\n"\

	" }																		\n"\
	"";

char Shader_terrain_LOD1_PS_String[] = 
	"#version 330															\n"\
	"																		\n"\
	"in vec4 vNormal;														\n"\
	"in vec2 vTexCoord1;													\n"\

	"layout (location = 0) out vec4 outputColor;							\n"\
	"layout (location = 1) out vec4 outputColor1;							\n"\
	
	"uniform sampler2D gSamplerMiniMap;										\n"\
	"uniform sampler2D gSamplerLightmap;									\n"\

	"uniform vec4 SunColor;													\n"\
    "uniform vec3 SunDirection;												\n"\


	"void main()															\n"\
	"{																		\n"\
	"	vec4 DiffuseColor = texture2D(gSamplerMiniMap, vTexCoord1);			\n"\

	"	// Invert the light direction for calculations.								\n"\
    "	// Calculate the light based on the bump map normal value.					\n"\
    "	float lightIntensity = clamp(dot(vNormal.xyz, -SunDirection), 0.0, 1.0);	\n"\

	"	// lightmap color															\n"\
	"	vec4 lightmap = texture2D(gSamplerLightmap, vTexCoord1);					\n"\


    "	// Determine the final diffuse color based on the diffuse color and			\n"\
	"	// the amount of light intensity.											\n"\
    "	outputColor = DiffuseColor * lightIntensity * SunColor * lightmap.a;		\n"\
	"	outputColor.rgb += DiffuseColor.rgb * lightmap.rgb;							\n"\
	"	outputColor = clamp( outputColor, 0.0, 1.0);								\n"\
	"	outputColor.w = 1.0;														\n"\

	"	// Packing a [0-1] float depth value into a 4D vector						\n"\
	"	// where each component will be 8-bits color value							\n"\
	"	const vec4 bitSh = vec4(16777216.0, 65536.0, 256.0, 1.0);					\n"\
	"	const vec4 bitMsk = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);				\n"\
	"	outputColor1 = fract(vNormal.w * bitSh);									\n"\
	"	outputColor1 -= outputColor1.xxyz * bitMsk;									\n"\
	
	"																				\n"\
	"																				\n"\
	"}																				\n"\

	"";