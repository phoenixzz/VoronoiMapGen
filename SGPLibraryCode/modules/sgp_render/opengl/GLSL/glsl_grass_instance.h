

char Shader_grassRender_VS_String [] =
	"#version 330															\n"\
	"																		\n"\
	"// position are regular vertex attributes								\n"\
	"layout (location = 0) in vec3 inLocalPosition;							\n"\
	"layout (location = 1) in vec2 inLocalUV;								\n"\

	"// inModelPosition will be used as a per-instance transformation		\n"\
	"//	inModelPosition.w is tiled texture index							\n"\
	"layout (location = 2) in vec4 inModelPosition;							\n"\
	"// Normal is a per-instance attribute									\n"\
	"// inNormal.w is grass scale											\n"\
	"layout (location = 3) in vec4 inNormal;								\n"\
	"// Grass Color is a per-instance attribute								\n"\
	"layout (location = 4) in vec4 inColor;									\n"\
	"// WindParams is a per-instance attribute								\n"\
	"layout (location = 5) in vec4 inWindParams;							\n"\



	"// TextureAtlas define how many parts texture be divided in X and Y	\n"\
	"// TextureAtlas.xy = AtlasNbX,AtlasNbY									\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX,1.0f/AtlasNbY						\n"\
	"uniform vec4 TextureAtlas;												\n"\
	"uniform mat4 ViewProjMatrix;											\n"\
	"uniform vec4 vWindDirAndStrength;										\n"\
	"// vTimeParams.x is game time											\n"\
	"// vTimeParams.y is grass movement period								\n"\
	"uniform vec2 vTimeParams;												\n"\
	"// vLightMapTextureDimision.x is 1.0 / LightmapTex width				\n"\
	"// vLightMapTextureDimision.y is 1.0 / LightmapTex Height				\n"\
	"uniform vec2 vLightMapTextureDimision;									\n"\

	"out vec2 vOutTexCoord;													\n"\
	"out vec2 vOutTexCoordLM;												\n"\
	"out vec4 vOutColor;													\n"\
	"out vec4 vOutNormal;													\n"\
	"flat out int InstanceID;												\n"\

	" void main()																			\n"\
	" {																						\n"\
	"	vec3 vTerrainNormal = normalize((inNormal.xyz - 0.5) * 2.0);						\n"\
	"	mat3 RotMatrix = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);									\n"\
	"	if( vTerrainNormal.y < 0.95 )														\n"\
	"	{																					\n"\
	"		RotMatrix[0] = normalize(cross(vTerrainNormal, vec3(0, 1, 0)));					\n"\
	"		RotMatrix[1] = vTerrainNormal;													\n"\
	"		RotMatrix[2] = normalize(cross(RotMatrix[0], vTerrainNormal));					\n"\
	"	}																					\n"\
	
	"	vec3 modelPos = (RotMatrix * inLocalPosition) * inNormal.w + inModelPosition.xyz;	\n"\

	"	float windAngle = vTimeParams.x * vTimeParams.y * length(inModelPosition.xyz);		\n"\
	"	vec3 vCosSin = vec3( cos(windAngle), 0, sin(windAngle) );							\n"\

	"	vec3 vOffset = inWindParams.xyz * vWindDirAndStrength.xyz * vWindDirAndStrength.w;	\n"\
	"	vOffset += vCosSin * inWindParams.xyz * vWindDirAndStrength.w;						\n"\
	"	modelPos += normalize(vTerrainNormal + vOffset) * inNormal.w * (1.0-inLocalUV.y);	\n"\
	

	" 	gl_Position = ViewProjMatrix * vec4(modelPos, 1.0);									\n"\

	"	vOutTexCoord = vec2(																\n"\
	"		float( int(inModelPosition.w) % int(TextureAtlas.x) ) / TextureAtlas.x,			\n"\
	"		float( int(inModelPosition.w) / int(TextureAtlas.x) ) / TextureAtlas.y ) +		\n"\
	"			inLocalUV.xy * TextureAtlas.zw;												\n"\
	"	vOutTexCoordLM.x = inModelPosition.x * vLightMapTextureDimision.x;					\n"\
	"	vOutTexCoordLM.y = 1.0 - inModelPosition.z * vLightMapTextureDimision.y;				\n"\
	
	"	vOutColor = inColor;																\n"\
	"	InstanceID = gl_InstanceID;															\n"\
	"	vOutNormal.xyz = vTerrainNormal;													\n"\
	"	const vec3 vlightingWaveScale = vec3(0.55, 0.0, 0.55);								\n"\
	"	vOutNormal.w = dot(vOffset, vlightingWaveScale);									\n"\
	" }																						\n"\
	"";


char Shader_grassRender_PS_String [] =
	"#version 330																	\n"\
	"																				\n"\
	"layout (location = 0) out vec4 outputColor;									\n"\

	"in vec2 vOutTexCoord;															\n"\
	"in vec2 vOutTexCoordLM;														\n"\
	"in vec4 vOutColor;																\n"\
	"in vec4 vOutNormal;															\n"\
	"flat in int InstanceID;														\n"\

	"uniform sampler2D gSampler0;			// Grass Texture						\n"\
	"uniform sampler2D gSamplerLightmap;	// Lightmap	Texture						\n"\

	"uniform vec4 SunColor;															\n"\
    "uniform vec3 SunDirection;														\n"\
	
	"void main()																	\n"\
	"{																				\n"\
	"	vec4 grassColor = texture2D(gSampler0, vOutTexCoord);						\n"\
	"	vec4 DiffuseColor = grassColor * vOutColor;									\n"\

	"	vec4 LightmapColor = texture2D(gSamplerLightmap, vOutTexCoordLM);			\n"\

	"	// Invert the light direction for calculations.								\n"\
    "	// Calculate the light based on the grass normal value.						\n"\
    "	float lightIntensity = clamp(dot(vOutNormal.xyz, -SunDirection), 0.0, 1.0);	\n"\

	"	const vec3 vlightingScaleBias = vec3(0.5, 0.3, 0.5);						\n"\
    
	"	// Determine the final color based on  the amount of light intensity.		\n"\
	"	outputColor.rgb = DiffuseColor.rgb * lightIntensity * SunColor.rgb *		\n"\
	"		LightmapColor.a * (SunColor.rgb-vOutNormal.w*vlightingScaleBias) * 2.0;	\n"\
	"	outputColor.rgb += DiffuseColor.rgb * LightmapColor.rgb;					\n"\

	"	outputColor.rgb = clamp( outputColor.rgb, 0.0, 1.0);						\n"\
	"	outputColor.a = DiffuseColor.a;												\n"\
	"}																				\n"\

	"";