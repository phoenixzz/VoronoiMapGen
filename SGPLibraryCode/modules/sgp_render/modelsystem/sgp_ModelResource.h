#ifndef __SGP_MODELRESOURCE_HEADER__
#define __SGP_MODELRESOURCE_HEADER__


class CMF1FileResource
{
public:
	CMF1FileResource() : pModelMF1(NULL), pMF1RawMemoryAddress(0), deleteTimeStamp(0) {}
	~CMF1FileResource()
	{
        // it's dangerous to delete an object that's still referenced by something else!
        jassert (getReferenceCount() == 0);

		if( pMF1RawMemoryAddress )
			delete [] pMF1RawMemoryAddress;
		pMF1RawMemoryAddress = NULL;

		for(int j=0; j<pBF1RawMemoryAddress.size(); j++)
		{
			uint8* pBF1Address = pBF1RawMemoryAddress.getReference(j);
			if( pBF1Address )
				delete [] pBF1Address;
			pBF1Address = NULL;
		}

		pModelMF1 = NULL;
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
	CSGPModelMF1 * pModelMF1;
	uint8* pMF1RawMemoryAddress;
	Array<uint8*> pBF1RawMemoryAddress;

	Array<uint32> StaticMeshIDArray;		// Staticmesh chunk ID Array of this Model file
	Array<uint32> ParticleSystemIDArray;	// particle system ID Array of this Model file

	uint32 deleteTimeStamp;

private:
	Atomic <int> refCount;
};

#endif		// __SGP_MODELRESOURCE_HEADER__