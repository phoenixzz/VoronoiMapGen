#ifndef __SGP_MODELFILEBONE_HEADER__
#define __SGP_MODELFILEBONE_HEADER__

//------------------------------------------------------------------
/*
SGPMF1BoneHeader    |-----------------------------------------------|
                    |           m_iId          uint32       		|
					|         m_iVersion       uint32				|
					|         m_cFilename      char[64]				|
					|         m_iNumBones      uint32				|
					|      m_iNumBoneGroup     uint32				|
					|												|
					|        m_iHeaderSize     uint32				|
					|												|
					|     	m_iBonesOffset	   uint32				|
					|	  m_iBoneGroupOffset   uint32				|
					|          										|
					|-----------------------------------------------|
	SGPMF1Bone0     |           									|
					|          m_cName        char[32]				|
					|												|
					|		 										|
					|     	  m_matFrame0Inv	matrix4x4			|
					|												|
					|         m_sParentID       uint16				|
					|		 m_bUpperBone		uint16				|
					|        m_iNumChildId      uint32				|
					|         m_ChildIds        uint16*				|
					|            ...								|
					|												|
					|KeyFrameBlock(Position)						|
					|    	KeyFrames			SGPMF1KeyFrame*     |
					|    	nextBlock			KeyFrameBlock*		|
					|		m_iNumber			uint16 				|
					|		m_BoneFileID		uint16 				|
					|					    ...						|
					|												|
					|												|
					|KeyFrameBlock(Rotation)						|
					|    	KeyFrames			SGPMF1KeyFrame*     |
					|    	nextBlock			KeyFrameBlock*		|
					|		m_iNumber			uint16 				|
					|		m_BoneFileID		uint16 				|
					|						...						|
					|												|
					|												|
					|ScaleKeyFrameBlock								|
					|    	KeyFrames		SGPMF1ScaleKeyFrame*    |
					|    	nextBlock		ScaleKeyFrameBlock*		|
					|		m_iNumber			uint16 				|
					|		m_BoneFileID		uint16 				|
					|						...						|
					|           									|
					|VisibleKeyFrameBlock							|
					|    	KeyFrames		SGPMF1VisibleKeyFrame*  |
					|    	nextBlock		VisibleKeyFrameBlock* 	|
					|		m_iNumber			uint16 				|
					|		m_BoneFileID		uint16 				|
					|						...						|
					|-----------------------------------------------|
    SGPMF1Bone1     |												|
					|           ...									|
					|           									|
					|-----------------------------------------------|
					            ...									
					|-----------------------------------------------|
    BoneGroup       |                               				|
	                |      Bone_Index			uint8 * 4			|
					|      Bone_Weight          float * 4           |
					|           ...									|
					|-----------------------------------------------|

*/
//------------------------------------------------------------------



#pragma pack(push, packing)
#pragma pack(1)




//-------------------------------------------------------------
//- SGPMF1BoneHeader
//- File header for the MF1 bone info file
struct SGPMF1BoneHeader
{
	uint32 m_iId;           //Must be 0xCAFEBBEE (magic number)
	uint32 m_iVersion;      //Must be 1
	char m_cFilename[64];	//Full filename (Relative Path to application's executable file, usually in Bin Floder)


	uint32 m_iNumBones;			//Number of bones
	uint32 m_iNumBoneGroup;		//Number of BoneGroup

	uint32 m_iHeaderSize;		//Size of this header

	uint32 m_iBonesOffset;		//File offset of bones data
	uint32 m_iBoneGroupOffset;	//File offset of BoneGroup data


	SGPMF1BoneHeader() : m_iId(0xCAFEBBEE), m_iVersion(1), m_iNumBones(0), m_iNumBoneGroup(0)
	{
		memset(m_cFilename, 0, 64);
	}
};

//-------------------------------------------------------------
//- SGPBF1BoneGroup
//- A Bone Group info 
struct SGPMF1BoneGroup
{
	// Bone index of this vertex (0 - 254)	(255 for Error)
	struct Bone_Index
	{
		uint8 x, y, z, w;
	} BoneIndex;

	// Bone weight of this vertex (0 - 255)
	struct Bone_Weight
	{
		uint8 x, y, z, w;
	} BoneWeight;
};

//-------------------------------------------------------------
//- SGPMF1KeyFrame
//- Rotation/Translation information for bones
struct SGPMF1KeyFrame
{
	uint32		m_iFrameID;		//at which keyframe is
    float		m_fParam[4];	//Translation or Rotation values
	bool operator == (const SGPMF1KeyFrame& KF) const noexcept
	{
		return (m_fParam[0] == KF.m_fParam[0]) &&
			(m_fParam[1] == KF.m_fParam[1]) &&
			(m_fParam[2] == KF.m_fParam[2]) &&
			(m_fParam[3] == KF.m_fParam[3]);
	}
};

struct SGPMF1ScaleKeyFrame
{
	uint32		m_iFrameID;		//at which keyframe is
	float		m_scale;		//scale value
};

struct SGPMF1VisibleKeyFrame
{
	uint32		m_iFrameID;		//at which keyframe is
	bool		m_Visible;		//bone visible
};

struct KeyFrameBlock
{
	SGPMF1KeyFrame * m_KeyFrames;			//keyframes data
	KeyFrameBlock * m_nextBlock;			//Pointer to next Block
	uint16 m_iNumber;						//Number of key frames
	uint16 m_BoneFileID;					//Bone Anim File NO 

	KeyFrameBlock() : m_iNumber(0), m_BoneFileID(0), m_KeyFrames(NULL), m_nextBlock(NULL) {}
};

struct ScaleKeyFrameBlock
{
	SGPMF1ScaleKeyFrame * m_KeyFrames;		//Scale keyframes data
	ScaleKeyFrameBlock * m_nextBlock;		//Pointer to next Block
	uint16 m_iNumber;						//Number of scale key frames
	uint16 m_BoneFileID;					//Bone Anim File NO

	ScaleKeyFrameBlock() : m_iNumber(0), m_BoneFileID(0), m_KeyFrames(NULL), m_nextBlock(NULL) {}
};

struct VisibleKeyFrameBlock
{
	SGPMF1VisibleKeyFrame * m_KeyFrames;	//Visible keyframes data
	VisibleKeyFrameBlock * m_nextBlock;		//Pointer to next Block
	uint16 m_iNumber;						//Number of Visible key frames
	uint16 m_BoneFileID;					//Bone Anim File NO 

	VisibleKeyFrameBlock() : m_iNumber(0), m_BoneFileID(0), m_KeyFrames(NULL), m_nextBlock(NULL) {}
};


//-------------------------------------------------------------
//- SGPMF1Bone
//- bone structure for MF1 
struct SGPMF1Bone
{
	char		m_cName[32];			//Name of bone

	Matrix4x4   m_matFrame0Inv;			//Frame 0 Inverse Matrix 

	uint16		m_sParentID;			//Parent bone index
	uint16		m_bUpperBone;			//The upper part of the body skeleton 
	uint32		m_iNumChildId;			//Number of Child bones
	uint16*		m_ChildIds;				//Child Bones ID

	KeyFrameBlock*			m_TransKeyFrames;	//Transform keyframes data
	KeyFrameBlock*			m_RotsKeyFrames;	//Rotation keyframes data
	ScaleKeyFrameBlock*		m_ScaleKeyFrames;	//Scale keyframes data
	VisibleKeyFrameBlock*	m_VisibleKeyFrames; //Visible keyframes data


	SGPMF1Bone()
	{
		m_TransKeyFrames = NULL;
		m_RotsKeyFrames = NULL;
		m_ScaleKeyFrames = NULL;
		m_VisibleKeyFrames = NULL;
		m_ChildIds = NULL;
	}
	~SGPMF1Bone()
	{
		while( m_TransKeyFrames != NULL )
		{
			delete [] m_TransKeyFrames->m_KeyFrames;
			m_TransKeyFrames->m_KeyFrames = NULL;
			KeyFrameBlock *pNext = m_TransKeyFrames->m_nextBlock;
			delete m_TransKeyFrames;
			m_TransKeyFrames = pNext;
		}

		while( m_RotsKeyFrames != NULL )
		{
			delete [] m_RotsKeyFrames->m_KeyFrames;
			m_RotsKeyFrames->m_KeyFrames = NULL;
			KeyFrameBlock *pNext = m_RotsKeyFrames->m_nextBlock;
			delete m_RotsKeyFrames;
			m_RotsKeyFrames = pNext;
		}
		while( m_ScaleKeyFrames != NULL )
		{
			delete [] m_ScaleKeyFrames->m_KeyFrames;
			m_ScaleKeyFrames->m_KeyFrames = NULL;
			ScaleKeyFrameBlock *pNext = m_ScaleKeyFrames->m_nextBlock;
			delete m_ScaleKeyFrames;
			m_ScaleKeyFrames = pNext;
		}
		while( m_VisibleKeyFrames != NULL )
		{
			delete [] m_VisibleKeyFrames->m_KeyFrames;
			m_VisibleKeyFrames->m_KeyFrames = NULL;
			VisibleKeyFrameBlock *pNext = m_VisibleKeyFrames->m_nextBlock;
			delete m_VisibleKeyFrames;
			m_VisibleKeyFrames = pNext;
		}
		if( m_ChildIds )
		{
			delete [] m_ChildIds;
			m_ChildIds = NULL;
		}
	}
};


#pragma pack(pop, packing)

#endif		// __SGP_MODELFILEBONE_HEADER__