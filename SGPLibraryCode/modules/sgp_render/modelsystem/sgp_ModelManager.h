#ifndef __SGP_MODELMANAGER_HEADER__
#define __SGP_MODELMANAGER_HEADER__


class SGP_API ISGPModelManager
{
public:
	ISGPModelManager(ISGPRenderDevice *pdevice, Logger* logger);
	~ISGPModelManager();


	//! Returns a loaded MF1 Model index by name
	/** Loads the MF1 file from disk if it is not already loaded and loading BF1 files if desired.
	    If mesh in MF1 file have been loaded, and bLoadBoneAnim is true,
		Try to load one bone anim file
	\param modelfilename Filename of the MF1 to be loaded.
	\param bLoadBoneAnim setting if loading BF1 bone Anim file 1
	\return ID of the CMF1FileResource, or 0xFFFFFFFF if the model could not be loaded.  */
	uint32 registerModel(const String& modelfilename, bool bLoadBoneAnim);

	//! Returns the MF1 Model index
	/** Loads the BF1 Bone Anim file from disk if it is not already loaded and attaching to one existed MF1 Mesh.
	    When mesh in MF1 file have not been loaded, nothing will happen
		This function usually be used to load addition Bone Anim file(index >0)
	\param modelname Filename of the MF1 to attach.
	\param boneAnimFileIndex  Filename index of BF1 bone Anim file to load
	\return ID of the CMF1FileResource  */
	uint32 attachActionInfoByName(const String& modelfilename, uint16 boneAnimFileIndex);
	
	/** Loads the BF1 Bone Anim file from disk if it is not already loaded and attaching to one existed MF1 Mesh.
	    When mesh in MF1 file have not been loaded, nothing will happen
	//\param id Index of the model to find.
	//\param boneAnimFileIndex  Filename index of BF1 bone Anim file to load
	//\return ID of the CMF1FileResource  */
	uint32 attachActionInfoByID(uint32 id, uint16 boneAnimFileIndex);

	//! Returns a MF1 model by name String
	/** Return the model from Model manager if it is be found else return 0xFFFFFFFF
	\param modelfilename Filename of the model to find.
	\return ID of the CMF1FileResource, or 0xFFFFFFFF if it could not be found. */
	uint32 getModelIDByName(const String& modelfilename);

	//! Returns a model by ID
	/** Return the model from Model manager if it is be found else NULL
	\param id Index of the model to find.
	\return Pointer the model Resource, or NULL if the model could not be found. */
	CMF1FileResource* getModelByID(uint32 id);

	//! Removes a model from the model manager and deletes it.
	/** This method can free a lot of memory!
	Please note that after calling this, the pointer to the	CMF1FileResource may no longer be valid.
	\param modelfilename Model file name will be deleted from the manager. */
	void unRegisterModelByName( const String& modelfilename );
	// \param id Model id will be deleted from the manager. */
	void unRegisterModelByID(uint32 id);



	//! Removes all models from the Model manager and deletes them. 
	// This function is without considering the reference count
	/** This method can free a lot of memory!
	Please note that after calling this, the pointer to the	CMF1FileResource may no longer be valid. */
	void removeAllModels();

	//! Returns amount of models currently loaded
	/** \return Amount of models currently loaded */
	uint32 getModelCount() const;

	//! Find first empty ID in Model Array
	uint32 getFirstEmptyID();

	//! Setting Working Direction
	inline void setWorkingDirection(const String& workingDir)
	{ m_WorkingDir = workingDir; }
	inline String& getWorkingDirection() { return m_WorkingDir; }

	//! Create Static Mesh / Skeleton Mesh and Register used textures from loaded CMF1FileResource data  
	//\param pMF1FileRes Pointer to CMF1FileResource.
	void createRenderResource(CMF1FileResource* pMF1FileRes);

	//! Release Static Mesh / Skeleton Mesh and unRegister used textures from loaded CMF1FileResource data  
	//\param pMF1FileRes Pointer to CMF1FileResource.
	void releaseRenderResource(CMF1FileResource* pMF1FileRes);






	//! Create used textures in Skins, multi-thread version, called by resource Loading Thread
	// immediately called after Load Raw MF1 Model file and before Render Resource be created
	//\param pMF1FileRes Pointer to CMF1FileResource.
	void registerSkinTexturesMT(CMF1FileResource* pMF1FileRes);
	void unRegisterSkinTexturesMT(CMF1FileResource* pMF1FileRes);

	// Multi-Thread version of Function createRenderResource and releaseRenderResource
	void createRenderResourceMT(SGPModelRecord Record);
	void releaseRenderResourceMT(SGPModelRecord Record);

	// Multi-Thread version of Function registerModel
	uint32 registerModelMT(const String& modelfilename, bool bLoadBoneAnim);

	// Multi-Thread version of unRegister
	void unRegisterModelByNameMT( const String& modelfilename );
	void unRegisterModelByIDMT(uint32 id);

	// Return default MF1 Skin
	// Only used for no-material mesh
	const SGPMF1Skin& getDefaultMF1Skin();

private:

	// Init m_MF1Models Array size also m_StringToModelIDMap hash map slot number
	// (so that the array won't have to keep dynamically resizing itself as the elements are added, 
	// and it'll therefore be more efficient. )
	static const int			INIT_MODELARRAYSIZE = 2048;

	ISGPRenderDevice*			m_pRenderDevice;
	Logger*						m_pLogger;
	String						m_WorkingDir;

	// Created Model Array
	OwnedArray<CMF1FileResource> m_MF1Models;


	// Hashmap of model file path string to Index of Model Array
	HashMap<uint64, uint32>		m_StringToModelIDMap;
};

#endif		// __SGP_MODELMANAGER_HEADER__