#ifndef __SGP_VERTEXTYPE_HEADER__
#define __SGP_VERTEXTYPE_HEADER__

	//! An enum for all types of vertex Struct in SGP Engine supports.
	enum SGP_VERTEX_TYPE
	{
		// untransformed position + vertex color
		SGPVT_UPOS_VERTEXCOLOR = 0,
		// untransformed position + one texture coord
		SGPVT_UPOS_TEXTURE,
		// untransformed position + one texture coord + vertex color
		SGPVT_UPOS_TEXTURE_VERTEXCOLOR,
		// untransformed position + two texture coord , used for Lightmap etc.
		SGPVT_UPOS_TEXTURETWO,
		// untransformed position + two texture coord + vertex color, used for Lightmap etc.
		SGPVT_UPOS_TEXTURETWO_VERTEXCOLOR,
		// untransformed pos + normal + VertexColor + one texture coord
		SGPVT_UPOS_NORMAL_TEXTURE_VERTEXCOLOR,
		// untransformed position + one texture coord + vertex color + Atlas index 
		SGPVT_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX,
		// used for character skeleton animation
		// untransformed pos + normal + one texture coord + bone weights
		SGPVT_ANIM,
		// used for terrain chunk render
		// untransformed pos + tangent + binormal + normal + two texture coord
		SGPVT_TERRAIN,
		// used for grass instancing render
		// only untransformed local pos + one texture coord
		SGPVT_GRASS,
		// transformed position + vertex color + one texcoord
		SGPVT_FONT,
	};


//! standard vertex used by the SGP engine.
// SGPVT_UPOS_VERTEXCOLOR Type
struct SGPVertex_UPOS_VERTEXCOLOR
{
	float  x, y, z;
	float  VertexColor[4];			// In RGBA
};

// SGPVT_UPOS_TEXTURE
struct SGPVertex_UPOS_TEXTURE
{
	float  x, y, z;
	float  tu, tv;
};

// SGPVT_UPOS_TEXTURE_VERTEXCOLOR
struct SGPVertex_UPOS_TEXTURE_VERTEXCOLOR
{
	float  x, y, z;
	float  VertexColor[4];			// In RGBA
	float  tu, tv;
};

// SGPVT_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX
struct SGPVertex_UPOS_TEXTURE_VERTEXCOLOR_TEXINDEX
{
	float  x, y, z;
	float  VertexColor[4];			// In RGBA
	float  tu, tv;
	float  textureIndex;
};

// SGPVT_UPOS_TEXTURETWO
struct SGPVertex_UPOS_TEXTURETWO
{
	float  x, y, z;
	float  tu, tv;
	float  tu1, tv1;
};

// SGPVT_UPOS_TEXTURETWO_VERTEXCOLOR
struct SGPVertex_UPOS_TEXTURETWO_VC
{
	float  x, y, z;
	float  VertexColor[4];			// In RGBA
	float  tu, tv;
	float  tu1, tv1;
};

// SGPVT_UPOS_NORMAL_TEXTURE_VERTEXCOLOR
struct SGPVertex_UPOS_NOR_TEX_VC
{
	float  x, y, z;
	float  vcNormal[3];
	float  VertexColor[4];			// In RGBA
	float  tu, tv;
};

// used for character animation (one vertex with max four bone weights)
struct SGPVertex_ANIM
{
	float  x, y, z, alpha;
	float  vcNormal[3];
	float  tu, tv;
	uint8 BlendIndices[4];		// Bone Index(0-255) Max 4 bones
	uint8 BlendWeights[4];		// Bone Weight(0-255) Max 4 bones
};

// SGPVT_TERRAIN (used for terrain render)
#define SGPVertex_TERRAIN SGPTerrainVertex


// SGPVT_GRASS ( used for grass instancing render )
struct SGPVertex_GRASS
{
	float x, y, z;
	float  tu, tv;
};

// grass Cluster Params (instancing rendering)
struct SGPVertex_GRASS_Cluster
{
	float vPosition[4];			// [3] is tiled texture index
	uint8 vPackedNormal[4];		// [3] is grass scale
	float vColor[4];			// [3] is grass Cluster alpha
	float vWindParams[4];		// Wind Offset for X and Z direction [0] [2]
};

// SGPVT_GRASS ( used for OpenGL ES 2.0 grass render )
struct SGPVertex_GRASS_GLES2
{
	float vPosition[4];			// [3] is tiled texture index
	float tu, tv, LMtu, LMtv;
	float vNormal[4];			// [3] is grass scale
	float vColor[4];			// [3] is grass Cluster alpha
	//float vWindParams[4];		// Wind Offset for X and Z direction [0] [2]
};


// used for font render in Screen
struct SGPVertex_FONT
{
	float  x, y, z, w;
	float  VertexColor[4];			// In RGBA
	float  tu, tv;
};




#endif		// __SGP_VERTEXTYPE_HEADER__