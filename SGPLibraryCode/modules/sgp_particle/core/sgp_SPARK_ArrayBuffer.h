#ifndef __SGP_SPARKARRAYBUFFER_HEADER__
#define __SGP_SPARKARRAYBUFFER_HEADER__

class Group;
template<class T> class ArrayBufferCreator;

/**
* A template buffer that holds an array of elements of type T
*/
template<class T>
class ArrayBuffer : public Buffer
{
	friend class ArrayBufferCreator<T>;

public :
	/** Gets the starting address of the inner array */
	T* getData() const;

	/** Gets the number of elements for a single particle */
	const uint32 getParticleSize() const;

	/**
	* Gets the total number of T in the inner array
	* The value is equal to particleSize * nbParticles.
	*/
	uint32 getDataSize() const;

private :

	T* data;

	uint32 particleSize;
	uint32 dataSize;

	ArrayBuffer<T>(uint32 nbParticles, uint32 particleSize);
	ArrayBuffer<T>(const ArrayBuffer<T>& buffer);
	virtual ~ArrayBuffer<T>();

	virtual void swap(int index0, int index1);
};

/** Template class to create an ArrayBuffer */
template<class T>
class ArrayBufferCreator : public BufferCreator
{
public :
	ArrayBufferCreator<T>(uint32 particleSize);

private :
	uint32 particleSize;

	virtual ArrayBuffer<T>* createBuffer(uint32 nbParticles,const Group& group) const;
};


typedef ArrayBuffer<float> FloatBuffer;

typedef ArrayBufferCreator<float> FloatBufferCreator;


template<class T>
ArrayBuffer<T>::ArrayBuffer(uint32 nbParticles, uint32 particleSize) :
	Buffer(),
	dataSize(nbParticles * particleSize),
	particleSize(particleSize)
{
	data = new T[dataSize];
}

template<class T>
ArrayBuffer<T>::ArrayBuffer(const ArrayBuffer<T>& buffer) :
	Buffer(buffer),
	dataSize(buffer.dataSize),
	particleSize(buffer.particleSize)
{
	data = new T[dataSize];
	memcpy(data, buffer.data, dataSize * sizeof(T));
}

template<class T>
ArrayBuffer<T>::~ArrayBuffer()
{
	delete[] data;
	data = NULL;
}

template<class T>
inline T* ArrayBuffer<T>::getData() const
{
	return data;
}

template<class T>
inline const uint32 ArrayBuffer<T>::getParticleSize() const
{
	return particleSize;
}

template<class T>
inline uint32 ArrayBuffer<T>::getDataSize() const
{
	return dataSize;
}

template<class T>
void ArrayBuffer<T>::swap(int index0, int index1)
{
	T* address0 = data + index0 * particleSize;
	T* address1 = data + index1 * particleSize;
	for (int i = 0; i < particleSize; ++i)
		swapVariables(address0[i], address1[i]);
}

template<class T>
ArrayBufferCreator<T>::ArrayBufferCreator(uint32 particleSize) :
	BufferCreator(),
	particleSize(particleSize)
{}

template<class T>
ArrayBuffer<T>* ArrayBufferCreator<T>::createBuffer(uint32 nbParticles,const Group& group) const
{
	return new ArrayBuffer<T>(nbParticles,particleSize);
}

#endif		// __SGP_SPARKARRAYBUFFER_HEADER__