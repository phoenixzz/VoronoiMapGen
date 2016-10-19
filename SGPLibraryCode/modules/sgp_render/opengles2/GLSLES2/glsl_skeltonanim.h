/*
	If the current vertex is affected by bones then the vertex position and
	normal will be transformed by the bone matrices. Each vertex wil have up 
	to 4 bone indices (inBoneIndex) and bone weights (inBoneWeight).
	
	The indices are used to index into the array of bone matrices 
	(BoneMatrixArray) to get the required bone matrix for transformation. The 
	amount of influence a particular bone has on a vertex is determined by the
	weights which should always total 1. So if a vertex is affected by 2 bones 
	the vertex position in world space is given by the following equation:

	position = (BoneMatrixArray[Index0] * inVertex) * Weight0 + 
	           (BoneMatrixArray[Index1] * inVertex) * Weight1

	The same proceedure is applied to the normals but the translation part of 
	the transformation is ignored.
*/
const char *Shader_anim_Attribute_String[] = { "inPosition", "inNormal", "inCoord", "inBoneIndex", "inBoneWeight" };

char Shader_anim_VS_String[] = 
	"														\n"\
	"attribute highp vec4 inPosition;						\n"\
	"attribute mediump vec3 inNormal;						\n"\
	"attribute mediump vec2 inCoord;						\n"\
	"attribute mediump vec4 inBoneIndex;					\n"\
	"attribute mediump vec4 inBoneWeight;					\n"\

	"uniform highp mat4 modelViewProjMatrix;				\n"\
	"uniform highp mat4 worldMatrix;						\n"\
	"uniform highp vec4 BoneMatrixArray[120];				\n"\

	"// TextureAtlas how many parts texture be divided in X and Y								\n"\
	"// TextureAtlas.xy = AtlasNbX, AtlasNbY													\n"\
	"// TextureAtlas.zw = 1.0f/AtlasNbX, 1.0f/AtlasNbY											\n"\
	"uniform mediump vec4 TextureAtlas;															\n"\
	"uniform mediump vec3 TexIndexUVOffset;		// x is texture index; y,z is uv offset			\n"\
	"uniform mediump vec3 SunDirection;			// Sun Direction 								\n"\

	"varying lowp float fVertexAlphaPass;														\n"\
	"varying mediump vec2 vTexCoordPass;														\n"\
	"varying lowp vec3 vLightPass;																\n"\

	" void main()																				\n"\
	" {																							\n"\
	"	 mediump ivec4 boneIndex = ivec4(inBoneIndex);											\n"\
	"	 mediump vec4 boneWeights = inBoneWeight;												\n"\

	"	 highp vec4 matBone0 = BoneMatrixArray[ boneIndex.x*3   ] * boneWeights.x;				\n"\
    "	 highp vec4 matBone1 = BoneMatrixArray[ boneIndex.x*3+1 ] * boneWeights.x;				\n"\
    "	 highp vec4 matBone2 = BoneMatrixArray[ boneIndex.x*3+2 ] * boneWeights.x;				\n"\


	"    for(lowp int i = 1; i < 4; ++i)																\n"\
    "    {																						\n"\
	"		 boneIndex = boneIndex.yzwx;														\n"\
	"		 boneWeights = boneWeights.yzwx;													\n"\
	"		 matBone0 += BoneMatrixArray[ boneIndex.x*3   ] * boneWeights.x;					\n"\
    "		 matBone1 += BoneMatrixArray[ boneIndex.x*3+1 ] * boneWeights.x;					\n"\
    "		 matBone2 += BoneMatrixArray[ boneIndex.x*3+2 ] * boneWeights.x;					\n"\
	"    }																						\n"\
	
	"	 highp mat4 mtInvBone;																	\n"\
	"	 mtInvBone[0] = vec4(matBone0.x, matBone1.x, matBone2.x, 0);							\n"\
	"	 mtInvBone[1] = vec4(matBone0.y, matBone1.y, matBone2.y, 0);							\n"\
	"	 mtInvBone[2] = vec4(matBone0.z, matBone1.z, matBone2.z, 0);							\n"\
	"	 mtInvBone[3] = vec4(matBone0.w, matBone1.w, matBone2.w, 1.0);							\n"\


	"    highp vec4 vPosition = mtInvBone * vec4(inPosition.xyz, 1.0);							\n"\
	"	 vPosition = vPosition / vPosition.w;													\n"\
	"	 highp vec4 vNormal = mtInvBone * vec4(inNormal, 0.0);									\n"\
																								
	" 	 gl_Position = modelViewProjMatrix * vPosition;											\n"\
	"	 highp vec4 worldNormal = worldMatrix * vNormal;										\n"\
	"	 worldNormal = normalize(worldNormal);													\n"\
	
	"	 // Invert the light direction for calculations.										\n"\
    "	 // Calculate the light based on the normal value.										\n"\
    "	 lowp float lightIntensity = clamp(dot(worldNormal.xyz, -SunDirection), 0.0, 1.0);		\n"\
	"	 vLightPass = vec3(lightIntensity, lightIntensity, lightIntensity);						\n"\

	"	 vTexCoordPass = vec2(																	\n"\
	"		 float( mod(TexIndexUVOffset.x, TextureAtlas.x) ) / TextureAtlas.x,					\n"\
	"		 float( int(TexIndexUVOffset.x) / int(TextureAtlas.x) ) / TextureAtlas.y ) +		\n"\
	"			 inCoord.xy * TextureAtlas.zw + TexIndexUVOffset.yz;							\n"\

	"	  fVertexAlphaPass = inPosition.w;														\n"\
	" }																							\n"\
	"";

char Shader_anim_PS_String[] = 
	"																							\n"\
	"varying lowp float fVertexAlphaPass;														\n"\
	"varying mediump vec2 vTexCoordPass;														\n"\
	"varying lowp vec3 vLightPass;																\n"\

	"uniform sampler2D gSampler0;																\n"\
	"uniform sampler2D gSamplerLightmap;	// Lightmap	Texture									\n"\
	
	"uniform lowp vec4 vMaterialColor;															\n"\
	"uniform mediump vec2 vTexCoordLightMap;													\n"\
	"uniform lowp vec4 SunColor;																\n"\
	
	"void main()																				\n"\
	"{																							\n"\
	"	lowp vec4 DiffuseColor = texture2D(gSampler0, vTexCoordPass) * vMaterialColor;			\n"\
	"	lowp vec4 LightmapColor = texture2D(gSamplerLightmap, vTexCoordLightMap);				\n"\

	"	gl_FragColor.rgb = DiffuseColor.rgb * vLightPass.rgb * SunColor.rgb;					\n"\
	"	gl_FragColor.rgb += DiffuseColor.rgb * SunColor.a * LightmapColor.a;					\n"\
	"	gl_FragColor.rgb += DiffuseColor.rgb * LightmapColor.rgb;								\n"\
	"	gl_FragColor.a = DiffuseColor.a * fVertexAlphaPass;										\n"\

	"}																							\n"\

	"";