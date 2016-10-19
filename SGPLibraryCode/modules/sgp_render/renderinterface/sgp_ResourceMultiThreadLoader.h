#ifndef __SGP_RESOURCEMTLOADER_HEADER__
#define __SGP_RESOURCEMTLOADER_HEADER__

class CTextureResource;
class CMF1FileResource;
class ISGPRenderDevice;

struct SGPTextureRecord
{
	CTextureResource* pTexResource;		// pointer of the CTextureResource
	String TexFileName;					// File Name of this texture file


	bool bReady;						// Raw data loaded and be ready for creating resource in render thread
										// OR be ready for releasing resource in render thread

	uint32 nRefCount;					// reference count before resource be created in render thread
	
	bool bGenMipMap;

	SGPTextureRecord() : pTexResource(NULL), bReady(false), bGenMipMap(false), nRefCount(0) {}
	~SGPTextureRecord()	{}

	bool operator== (const SGPTextureRecord& other) const noexcept
	{
		return TexFileName.equalsIgnoreCase( other.TexFileName );
	}
};

struct SGPModelRecord
{
	CMF1FileResource* pMF1Resource;		// pointer of the CMF1FileResource
	String MF1AbsoluteFileName;			// Absolute path File Name of this MF1 file
	uint16 BF1FileIndex;				// Filename index of BF1 bone Anim file (0xFFFF will be static mesh)


	bool bReady;						// Raw data be loaded from disk and be ready for creating resource in render thread
										// OR be ready for releasing resource in render thread
	
	uint32 nRefCount;					// reference count before resource be created in render thread


	SGPModelRecord() : pMF1Resource(NULL), BF1FileIndex(0xFFFF), bReady(false), nRefCount(0) {}
	~SGPModelRecord() {}
	bool operator== (const SGPModelRecord& other) const noexcept
	{
		return MF1AbsoluteFileName.equalsIgnoreCase( other.MF1AbsoluteFileName );
	}
};










class CSGPResourceLoaderMuitiThread : public Thread
{
public:
    CSGPResourceLoaderMuitiThread(ISGPRenderDevice* pDevice)
        : Thread ("Resource Loader Multi Thread"), m_pDevice (pDevice)
    {
        interval = 10;

		Logger::getCurrentLogger()->writeToLog(String("Create Resource Loading Thread"), ELL_INFORMATION);

        // give the threads a background priority (lower)
        startThread(3);
    }

    ~CSGPResourceLoaderMuitiThread()
    {
		Logger::getCurrentLogger()->writeToLog(String("Shutdown Resource Loading Thread"), ELL_INFORMATION);

		removeAll();

        // allow the thread 2 seconds to stop cleanly - should be plenty of time.
        stopThread (2000);
    }

    void run();

	void addLoadingTexture(const String& texturename, bool bGenMipMap);
	void addDeletingTexture(CTextureResource *pTextureRes, const String& texturename);
	void addLoadingModel(const String& modelname, uint16 BF1FileIndex);
	void addDeletingModel(CMF1FileResource *pModelRes, const String& modelname);

	void syncRenderResource();
	void removeAll();

private:
    int interval;

	CriticalSection resourceArrayLock;

	ISGPRenderDevice* m_pDevice;

	
	Array<SGPModelRecord> m_LoadingModels;
	Array<SGPTextureRecord> m_LoadingTextures;

	Array<SGPModelRecord> m_DeletingModels;
	Array<SGPTextureRecord> m_DeletingTextures;

	static const int RESOURCE_BONE_TO_FREE_KEEPTIME = 1 * 60 * 1000;	// 1 mins

    SGP_DECLARE_NON_COPYABLE (CSGPResourceLoaderMuitiThread)
};

#endif		// __SGP_RESOURCEMTLOADER_HEADER__