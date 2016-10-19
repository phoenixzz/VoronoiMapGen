#ifndef __SGP_TEXTURERESOURCE_HEADER__
#define __SGP_TEXTURERESOURCE_HEADER__

class CTextureResource
{
public:
	CTextureResource() : pSGPTexture(NULL), pSGPImage(NULL), deleteTimeStamp(0) {}
	~CTextureResource()
	{
		// it's dangerous to delete an object that's still referenced by something else!
        jassert (getReferenceCount() == 0);

		if( pSGPTexture)
			delete pSGPTexture;
		pSGPTexture = NULL;
	}


    inline void incReferenceCount() noexcept
    {
        ++refCount;
    }
    inline void decReferenceCount() noexcept
    {
        jassert (getReferenceCount() > 0);

        --refCount;
    }

    inline int getReferenceCount() const noexcept
	{ return refCount.get(); }


public:
	ISGPTexture * pSGPTexture;		// render source
	ISGPImage* pSGPImage;			// Raw Texture data

	uint32 deleteTimeStamp;

private:
	Atomic <int> refCount;
};


#endif			// __SGP_TEXTURERESOURCE_HEADER__