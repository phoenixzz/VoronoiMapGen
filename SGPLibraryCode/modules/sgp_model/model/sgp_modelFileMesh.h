#ifndef __SGP_MODELFILEMESH_HEADER__
#define __SGP_MODELFILEMESH_HEADER__

//------------------------------------------------------------------
/*
    SGPMF1Header    |-----------------------------------------------|
                    |           m_iId           uint32       		|
					|         m_iVersion        uint32				|
					|         m_cFilename      char[64]				|
					|												|
					|		   m_iUVAnim		 uint32				|
					|		 m_iBipBoneID		 int32				|
					|         m_iNumSkins        uint32				|
					|		 m_iSkinOffset		 uint32				|
					|         m_iNumLods         uint32				|
					|         m_iNumMeshes       uint32				|
					|      m_iLod0MeshOffset     uint32				|
					|	   m_iLod1MeshOffset     uint32				|
					|	   m_iLod2MeshOffset     uint32				|	
					|												|
					|	  m_iNumBoneAnimFile     uint32				|
					|     m_iBoneAnimFileOffset  uint32			    |
					|      m_iNumActionList      uint32             |
					|	   m_iActionListOffset   uint32				|
					|                                               |
					|												|
					|	   m_iNumAttachment      uint32				|
					|		 m_iAttachOffset     uint32				|
					|	   m_iNumEffectAttach    uint32				|
					| 		 m_iEttachOffset     uint32				| 
					|												|
					|       m_iNumParticles      uint32             |
					|        m_iParticleOffset   uint32				|
					|        m_iNumRibbons       uint32             |
					|		 m_iRibbonOffset	 uint32				|
					|												|
					|		 m_iNumConfigs       uint32				|
					|        m_iConfigsOffset    uint32             |
					|        m_iHeaderSize       uint32				|
					|												|
					|-----------------------------------------------|
	AABBOX			|            AABBox	of MF1						|
					|-----------------------------------------------| 
					|          SGPMF1Skin*        uint32            |
					|          SGPMF1Mesh*        uint32		    |
					|          SGPMF1Mesh*        uint32		    |
					|          SGPMF1Mesh*        uint32		    |
					|		SGPMF1BoneFileName*   uint32			|
					|	    SGPMF1ActionList*     uint32			|
					|	   SGPMF1AttachmentTag*   uint32			|
					|	   SGPMF1AttachmentTag*   uint32			|
					|		   SGPMF1Bone*		  uint32			|
					|		SGPMF1BoneGroup*	  uint32			|
					|			  void*			  uint32			|
					|			  void*			  uint32			|
					|                                               | 
					|		 m_iNumBones		  uint32			|
					|		 m_iNumBoneGroup	  uint32			|
					|												|
					|		 CSGPModelMF1**		  uint32			|
					|-----------------------------------------------| 
	Skin			|            m_cName         char [64]          |
					|			m_iMtlFlag        uint32	    	|
					|             m_UTile         uint16            |
					|             m_VTile         uint16			|
					|          m_iStartFrame      uint16			|
					|			m_iLoopMode		  uint16			|
					|          m_fPlayBackRate     float			|
					|            m_fUSpeed         float			|
					|            m_fVSpeed         float			|
					|												|
					|         m_iNumMatKeyFrame   uint32			|
					|SGPMF1MatKeyFrame|---------------------------| |
					|                 | m_iFrameID		 uint32   | |
					|				  | m_MaterialColor  float * 4| |
					|				  | m_fUOffset		 float    | |
					|				  | m_fVOffset       float    | |
					|				  |---------------------------|	|
					|								...		    	|
					|												|
			        |-----------------------------------------------|
	LOD0Mesh        |												|
	                |					   m_cMeshId      char[4]   |
					|					   m_cName       char[64]   |
					|					   m_iNumVerts	  uint32	|
					|					|-------------------------| |
					|            Vertex |       SGPMF1Vertex      | |
					|                   |           ...           | |
					|                   |-------------------------| |
					|					   m_iNumIndices	uint32	|
					|                   |-------------------------| |
					|            Index  |         uint16          | |
					|                   |           ...           | |
					|                   |-------------------------| |
					| VertexBoneGroupID |         uint16          | |
					|                   |           ...           | |
					|                   |-------------------------| |
					|					   m_iNumUV0		uint32	|
					|                   |-------------------------| |
					|  SGPMF1TexCoord0  |        float * 2        | |
					|                   |           ...           | |
					|                   |-------------------------| |
					|					   m_iNumUV1		uint32	|
					|                   |-------------------------| |
					|  SGPMF1TexCoord1  |        float * 2        | |
					|                   |           ...           | |
					|                   |-------------------------| |
					|					 m_iNumVertexColor uint32   |
					|                   |-------------------------| |
					| SGPMF1VertexColor |        float * 3        | |
					|                   |           ...           | |
					|                   |-------------------------| |
					|         SkinIndex |         uint32          | |
					|                   |-------------------------| |
					|          nType    |         uint32          | |
					|                   |-------------------------| |
					|		boundingbox |        AABBox of Mesh   | |
					|                   |-------------------------| |
			        |-----------------------------------------------|
	LOD1Mesh        |					...							|
			        |-----------------------------------------------|
	LOD2Mesh        |                   ...                         |
			        |-----------------------------------------------|
    BoneFileName    |        BoneFileName          char[64]         |
	                |            ...                                |
			        |-----------------------------------------------|
	ActionList      |        m_cName               char[32]         |
					|        BoneFileIndex         uint8            |
					|        ActionStart	       uint32           |
					|        ActionEnd             uint32           |
	                |                 ......                        |
			        |-----------------------------------------------|
	Attachment		|		AttachBoneID        uint16	        	|
					|	    m_iAttachParts      uint16				|
	                |       m_InitMatTag		Matrix4x4	    	|
					|												|
					|            ...								|
					|												|
			        |-----------------------------------------------|
Effect Attachment	|      AttachBoneID         uint16				|
					|    	 m_szname           char[32]			|
					|      m_InitMatTag		    Matrix4x4			|
					|	 											|
					|    	     ...								|
					|-----------------------------------------------|
	Particle		|		AttachBoneID        uint16				|
					|		 m_szname           char[32]			|
					|		m_AbsoluteMatrix	Matrix4x4			|
					|												|
					|		m_SystemParam	|----------------------||
					|						|		struct		   ||
					|						|  ParticleSystemParam ||
					|						|----------------------||
					|                  ...                          |
			        |-----------------------------------------------|
	Ribbon          |                  ...                          |
			        |-----------------------------------------------|
					|		MeshConfigNum		  uint32			|
 ConfigSetting	    |	ReplaceTextureConfigNum	  uint32			|
					|		ParticleConfigNum	  uint32			|
					|		RibbonConfigNum		  uint32			|
					|												|
					|		MeshConfig		|----------------------||
					|						|       uint32         ||
					|						|		bool		   ||
					|						|----------------------||
					|								......			|
					|	ReplaceTextureConfig|----------------------||
					|						|       uint32         ||
					|						|		char[64]	   ||
					|						|----------------------||
					|								........		|
					|												|
					|		ParticleConfig  |----------------------||
					|						|       uint32         ||
					|						|		bool		   ||
					|						|----------------------||
					|								........		|
					|		RibbonConfig    |----------------------||
					|						|       uint32         ||
					|						|		bool		   ||
					|						|----------------------||
					|								........		|
					|-----------------------------------------------|
*/
//------------------------------------------------------------------






#pragma pack(push, packing)
#pragma pack(1)

//-------------------------------------------------------------
//- SGPMF1Header
//- File header for the MF1 file, Mesh Info File
struct SGPMF1Header
{
	uint32 m_iId;				//Must be 0xCAFE2BEE (magic number)
	uint32 m_iVersion;			//Must be 1
	char m_cFilename[64];		//Full filename (Relative Path to application's executable file, usually in Bin Floder)

	uint32 m_iUVAnim;			//Have texture coord anim in this MF1?
	int32  m_iBipBoneID;		//Bip Bone ID (if not have bip bone, it is -1)

	uint32 m_iNumSkins;			//Number of skins for model
	uint32 m_iSkinOffset;		//File offset for skins

	uint32 m_iNumLods;			//Number of LODs
	uint32 m_iNumMeshes;		//Number of sub-meshes for LOD0
	uint32 m_iLod0MeshOffset;	//File offset for Lod0 meshes
	uint32 m_iLod1MeshOffset;	//File offset for Lod1 meshes
	uint32 m_iLod2MeshOffset;	//File offset for Lod2 meshes


	uint32 m_iNumBoneAnimFile;		//Number of Bone Skeleton Anim File
	uint32 m_iBoneAnimFileOffset;	//File offset for Bone skeleton Anim File name

	uint32 m_iNumActionList;		//Number of Action List
	uint32 m_iActionListOffset;		//File offset for action list

	uint32 m_iNumAttc;			//Number of Attachment Point
	uint32 m_iAttachOffset;		//File offset for Attachment Point
	uint32 m_iNumEttc;			//Number of Effect Attachment Point
	uint32 m_iEttachOffset;		//File offset for Effect Attachment Point

	uint32 m_iNumParticles;		//Number of Particle Emitter
	uint32 m_iParticleOffset;	//File offset for Particle
	uint32 m_iNumRibbons;		//Number of Ribbon Emitter
	uint32 m_iRibbonOffset;		//File offset for Ribbon

	uint32 m_iNumConfigs;		//Number of config setting
	uint32 m_iConfigsOffset;	//File offset for config setting

	uint32 m_iHeaderSize;		//Size of this header


	SGPMF1Header() : m_iId(0xCAFE2BEE), m_iVersion(1), m_iNumLods(1), m_iUVAnim(0),
		m_iBipBoneID(-1), m_iNumSkins(0), m_iNumBoneAnimFile(0), m_iNumActionList(0),
		m_iNumMeshes(0), m_iNumParticles(0), m_iNumRibbons(0), m_iNumConfigs(0),
		m_iNumAttc(0), m_iNumEttc(0), m_iLod1MeshOffset(0), m_iLod2MeshOffset(0)
	{
		memset(m_cFilename, 0, 64);
	}
};


//-------------------------------------------------------------
//- SGPMF1Vertex
//- A single vertex in the MF1 file
struct SGPMF1Vertex
{
	float vPos[3];
	float vNormal[3];
};



//-------------------------------------------------------------
//- SGPMF1TexCoord
//- A U/F pair of texture coordinates for a vertex
struct SGPMF1TexCoord
{
	float m_fTexCoord[2];
};

//-------------------------------------------------------------
//- SGPMF1VertexColor
//- A vertex color for a vertex
struct SGPMF1VertexColor 
{
	float m_fVertColor[3];		// RGB, If AVMESH, Using R Channel as alpha
};

//-------------------------------------------------------------
//- SGPMF1MatKeyFrame
//- A diffuse color value for material Key Frame
struct SGPMF1MatKeyFrame
{
	uint32 m_iFrameID;				// at which keyframe is
	float m_MaterialColor[4];		// Diffuse Color RGBA
	float m_fUOffset;				// Texture UV offset
	float m_fVOffset;
	bool operator == (const SGPMF1MatKeyFrame& KF) const noexcept
	{
		return (FloatCmp(m_MaterialColor[0], KF.m_MaterialColor[0]) == 0) &&
			(FloatCmp(m_MaterialColor[1], KF.m_MaterialColor[1]) == 0) &&
			(FloatCmp(m_MaterialColor[2], KF.m_MaterialColor[2]) == 0) &&
			(FloatCmp(m_MaterialColor[3], KF.m_MaterialColor[3]) == 0) &&
			(FloatCmp(m_fUOffset, KF.m_fUOffset) == 0) &&
			(FloatCmp(m_fVOffset, KF.m_fVOffset) == 0);
	}
};

//-------------------------------------------------------------
//- SGPMF1Skin
//- A Mesh Skin struct
struct SGPMF1Skin
{
	char m_cName[64];			// Texture name
	uint32 m_iMtlFlag;			// Material render flag
	uint16 m_UTile;				// Texture Tile U
	uint16 m_VTile;				// Texture Tile V
	uint16 m_iStartFrame;		// Material key Frame start Frame
	uint16 m_iLoopMode;			// Texture Tile loop mode
	float  m_fPlayBackRate;		// Texture Tile Play speed
	float  m_fUSpeed;
	float  m_fVSpeed;			// texture anim play speed u/v


	uint32 m_iNumMatKeyFrame;	// Material Key Frame count
	SGPMF1MatKeyFrame * m_pMatKeyFrame;

	SGPMF1Skin()
	{
		memset(m_cName, 0, 64);
		m_iMtlFlag = 0;
		m_UTile = m_UTile = 1;
		m_iStartFrame = 0;
		m_iLoopMode = 0;
		m_fPlayBackRate = 1.0f;
		m_fUSpeed = m_fVSpeed = 0;
		m_iNumMatKeyFrame = 0;
		m_pMatKeyFrame = NULL;
	}
	~SGPMF1Skin()
	{
		if( m_iNumMatKeyFrame > 0 && m_pMatKeyFrame )
		{
			delete [] m_pMatKeyFrame;
			m_pMatKeyFrame = NULL;
		}
	}
};

//-------------------------------------------------------------
//- SGPBF1BoneFileName
//- A bone file name struct
struct SGPMF1BoneFileName
{
	char m_cBoneFileName[64];	// Bone & skeleton anim file name
};

//-------------------------------------------------------------
//- SGPMF1ActionList
//- A Action list struct
struct SGPMF1ActionList
{
	char m_cName[32];			// Action Name
	uint8 m_iBoneFileIndex;		// Bone % skeleton anim file Index
	uint32 m_iActionStart;		// Action Start Frame ID
	uint32 m_iActionEnd;		// Action End Frame ID
};


//-------------------------------------------------------------
//- SGPMF1Mesh
//- A single mesh in the MF1 file
struct SGPMF1Mesh
{
	char m_cMeshId[4];					//Mesh ID
	char m_cName[64];					//Mesh name

	uint32 m_iNumVerts;					//Number of vertices
	SGPMF1Vertex * m_pVertex;			//vertices data
	uint32 m_iNumIndices;				//Number of indices
	uint16 * m_pIndices;				//index data
	uint16 * m_pVertexBoneGroupID;		//Vertex Bone Group ID (num is m_iNumVerts)
	uint32 m_iNumUV0;					//Number of UV0
	SGPMF1TexCoord * m_pTexCoords0;		//Texture coordinate set 0
	uint32 m_iNumUV1;					//Number of UV1
	SGPMF1TexCoord * m_pTexCoords1;		//Texture coordinate set 1
	uint32 m_iNumVertexColor;			//Number of Vertex color
	SGPMF1VertexColor * m_pVertexColor; //Data for vertex color

	uint32 m_SkinIndex;
	uint32 m_nType;
	AABBox m_bbox;


	SGPMF1Mesh() : m_iNumVerts(0), m_iNumIndices(0), m_iNumUV0(0), m_iNumUV1(0),
		m_iNumVertexColor(0), m_SkinIndex(0), m_nType(0)
	{
		memset(m_cMeshId, 0, 4);
		memset(m_cName, 0, 64);

		m_pVertex = NULL;
		m_pIndices = NULL;
		m_pVertexBoneGroupID = NULL;
		m_pTexCoords0 = NULL;
		m_pTexCoords1 = NULL;
		m_pVertexColor = NULL;

	}
	~SGPMF1Mesh()
	{
		if( m_pVertex )
		{
			delete [] m_pVertex;
			m_pVertex = NULL;
		}
		if( m_pIndices )
		{
			delete [] m_pIndices;
			m_pIndices = NULL;
		}
		if( m_pVertexBoneGroupID )
		{
			delete [] m_pVertexBoneGroupID;
			m_pVertexBoneGroupID = NULL;
		}
		if( m_pTexCoords0 )
		{
			delete [] m_pTexCoords0;
			m_pTexCoords0 = NULL;
		}
		if( m_pTexCoords1 )
		{
			delete [] m_pTexCoords1;
			m_pTexCoords1 = NULL;
		}
		if( m_pVertexColor )
		{
			delete [] m_pVertexColor;
			m_pVertexColor = NULL;
		}	
	}
};

//-------------------------------------------------------------
//- SGPMF1AttachmentTag
//- bones attachment info struct
struct SGPMF1AttachmentTag
{
	uint16		m_iAttachBoneID;	// Attach Bone ID
	uint16		m_iAttachParts;		// Attachment parts
	Matrix4x4	m_InitMatTag;		// Attach Bone Init Matrix
};


//-------------------------------------------------------------
//- SGPMF1ParticleTag
//- particle Transform info struct
struct SGPMF1ParticleTag
{
	uint16				m_iAttachBoneID;	// Attach Bone ID
	char				m_szname[32];		// Particle Name
	Matrix4x4			m_AbsoluteMatrix;	// Particle Absolute world space matrix
	ParticleSystemParam m_SystemParam;		// Particle setting Param
};

#pragma pack(pop, packing)


// the struct PtrOffset node array for collecting pointers
struct PtrOffset
{
	void*  pDataPointer;
	uint32 DataSize;
	uint32 RawOffset;

	PtrOffset() : pDataPointer(NULL), DataSize(0), RawOffset(0) {}
};

class CSGPModelMF1
{
public:
	CSGPModelMF1();
	~CSGPModelMF1();

	//Load an MF1 mesh model
	//Return the raw memory allocated from file
	static uint8* LoadMF1(CSGPModelMF1* &pOutModelMF1, const String& WorkingDir, const String& Filename);
	//Load an MF1 bone animation file
	//Return the raw memory allocated from file
	static uint8* LoadBone(CSGPModelMF1* &pOutModelMF1, const String& WorkingDir, const String& BoneFilename, uint16 BoneFileIndex = 0);

	//Save an MF1 mesh model
	bool SaveMF1(const String& WorkingDir, const String& szFilename);
	//Save an MF1 bone animation file
	bool SaveBone(const String& WorkingDir, const String& BoneFilename, uint16 BoneFileIndex = 0);

	//Get real vertex position and normal from mesh according to TexCoord1
	bool GetMeshPointFromSecondTexCoord( Vector3D& position, Vector3D& normal, const Vector2D& uv, const Matrix4x4& modelMatrix );


	//This function is used when saving. 
	//In this function you transfer the pDataBuffer pointers to file offset according to DataSize.
	//the pointers are adjusted to become an offset from the beginning of the block.
	uint32 CollectPointers( void* pDataBuffer, uint32 DataSize, uint32 RawOffset = 0 );
	
	//Return recorded original pointer from Array
	//Because CollectPointers() function will adjust pointer to offset
	//Alloced memory in this pointer will miss when deleted
	//After save function, those pointers should be relocated 
	void * RelocateFirstPointers();

public:
	//File header
	SGPMF1Header m_Header;

	// Bounding box (AABB)
	AABBox m_MeshAABBox;

	//Skins
	SGPMF1Skin * m_pSkins;

	//Meshes
	SGPMF1Mesh * m_pLOD0Meshes;
	SGPMF1Mesh * m_pLOD1Meshes;
	SGPMF1Mesh * m_pLOD2Meshes;


	//Bone & Skeleton Anim filename
	SGPMF1BoneFileName * m_pBoneFileNames;

	//Action List
	SGPMF1ActionList * m_pActionLists;

	//Attachment helper Tags
	SGPMF1AttachmentTag * m_pAttachTags;
	SGPMF1AttachmentTag * m_pEffectTags;	



	//Bone Info
	SGPMF1Bone * m_pBones;					// every bone data

	//Bone Group Info
	SGPMF1BoneGroup * m_pBoneGroup;			// BoneGroup data

	//Particle Info
	SGPMF1ParticleTag * m_pParticleEmitter;	// Particles data

	//Ribbon Reserved
	void * m_pRibbonEmitter;

	//Config Setting
	SGPMF1ConfigSetting * m_pConfigSetting;	// Config data


	uint32		 m_iNumBones;
	uint32		 m_iNumBoneGroup;


};


#endif		// __SGP_MODELFILEMESH_HEADER__