#ifndef __SGP_OBJECT_HEADER__
#define __SGP_OBJECT_HEADER__


//! An enum for all types of objects in SGP Engine supports.
enum SGP_OBJECT_TYPE
{
	SGPOT_Building = 0,
	SGPOT_Light,

	SGPOT_Proxy,
};

#pragma pack(push, packing)
#pragma pack(1)

struct ISGPObject
{
public:
	ISGPObject() : m_iSceneID(0), m_iConfigIndex(0), m_iObjectInChunkIndexNum(0),
		m_iMeshTriangleCount(0), m_pObjectInChunkIndex(NULL), m_bRefreshed(false)
	{
		m_MF1FileName[0] = 0;
		m_SceneObjectName[0] = 0;

		m_ObjectType = SGPOT_Building;

		m_fPosition[0] = m_fPosition[1] = m_fPosition[2] = 0;		
		m_fRotationXYZ[0] = m_fRotationXYZ[1] = m_fRotationXYZ[2] = 0;	
		m_fScale = 1.0f;			
		m_fAlpha = 1.0f;			
	}
	~ISGPObject()
	{
		if( m_pObjectInChunkIndex )
			delete [] m_pObjectInChunkIndex;
		m_pObjectInChunkIndex = NULL;
	}

	inline void Clone(const ISGPObject* pObjSrc)
	{
		memcpy( m_MF1FileName, pObjSrc->m_MF1FileName, sizeof(char)*128 );			
		memcpy( m_SceneObjectName, pObjSrc->m_SceneObjectName, sizeof(char)*128 );

		m_iSceneID = pObjSrc->m_iSceneID;
		m_iConfigIndex = pObjSrc->m_iConfigIndex;
		m_ObjectType = pObjSrc->m_ObjectType;			

		memcpy( m_fPosition, pObjSrc->m_fPosition, sizeof(float)*3 );
		memcpy( m_fRotationXYZ, pObjSrc->m_fRotationXYZ, sizeof(float)*3 );
	
		m_fScale = pObjSrc->m_fScale;				
		m_fAlpha = pObjSrc->m_fAlpha;				
		m_bRefreshed = pObjSrc->m_bRefreshed;
	

		m_iMeshTriangleCount = pObjSrc->m_iMeshTriangleCount;	
		m_ObjectOBBox = pObjSrc->m_ObjectOBBox;			


		m_iObjectInChunkIndexNum = pObjSrc->m_iObjectInChunkIndexNum;
		if( m_iObjectInChunkIndexNum > 0 )
		{
			m_pObjectInChunkIndex = new int32 [m_iObjectInChunkIndexNum];
			memcpy(m_pObjectInChunkIndex, pObjSrc->m_pObjectInChunkIndex, sizeof(int32) * m_iObjectInChunkIndexNum);
		}
	}


	inline const OBBox& getBoundingBox() const { return m_ObjectOBBox; }
	inline const uint32 getTriangleCount() const { return m_iMeshTriangleCount; }

	inline void setBoundingBox(const OBBox& boundingbox) { m_ObjectOBBox = boundingbox; }
	inline void setTriangleCount(uint32 nTriangleCount) { m_iMeshTriangleCount = nTriangleCount; }


	//! Get/Set name of object (in most cases this is the unique name in this world map)
	inline const char* getSceneObjectName() const { return m_SceneObjectName; }
	inline void setSceneObjectName(const char* pNameStr) { strcpy(m_SceneObjectName, pNameStr); }

	//! Get/Set name of object MF1 FileName
	inline const char* getMF1FileName() const { return m_MF1FileName; }
	inline void setMF1FileName(const char* pNameStr) { strcpy(m_MF1FileName, pNameStr); }


	inline uint32 getObjectInChunkNum() const { return m_iObjectInChunkIndexNum; }
	inline uint32 getObjectInChunkIndex( int idx ) const { return m_pObjectInChunkIndex[idx]; }

	//! Get type of object
	inline SGP_OBJECT_TYPE getSceneObjectType() const { return m_ObjectType; }
	//! Get SceneObject ID of object
	inline uint32 getSceneObjectID() const { return m_iSceneID; }



	//! Interface function
	inline bool isLightObject() const 
	{ 
		return (m_ObjectType == SGPOT_Light); 
	}
	inline bool isEditorObject() const 
	{ 
		return (m_ObjectType == SGPOT_Light) || (m_ObjectType == SGPOT_Proxy);
	}

public:
	char			m_MF1FileName[128];			// MF1 file name
	char			m_SceneObjectName[128];		// Scene Object Name

	uint32			m_iSceneID;					// scene object ID in one world map
	uint32			m_iConfigIndex;				// scene object MF1 config setting index

	SGP_OBJECT_TYPE	m_ObjectType;				// scene object type

	float			m_fPosition[3];				// object position
	float			m_fRotationXYZ[3];			// object rotation ( X Y Z Axis ) Angle of rotation in radians
	float			m_fScale;					// object scale
	float			m_fAlpha;					// object alpha value

	bool			m_bRefreshed;				// object data is up to date

	uint32			m_iMeshTriangleCount;		// Mesh Triangle Count of this object
	OBBox			m_ObjectOBBox;				// object bounding box
	bool			m_bReceiveLight;
	bool			m_bCastShadow;

	// one object may be in many terrain chunks in the same time
	uint32			m_iObjectInChunkIndexNum;	// terrain chunks index number
	int32*			m_pObjectInChunkIndex;		// terrain chunks index array
};

#pragma pack(pop, packing)
#endif		// __SGP_OBJECT_HEADER__