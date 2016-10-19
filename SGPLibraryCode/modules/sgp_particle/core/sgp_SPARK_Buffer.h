#ifndef __SGP_SPARKBUFFER_HEADER__
#define __SGP_SPARKBUFFER_HEADER__

class Particle;
class Group;

/**
* An abstract class that defines the interface for the additional buffers of Group
*
* A buffer allows a Group to hold additional data.
* Typically, they are used for rendering as the GPU or 
	the rendering engine needs data to be organized in a specific way.

* Their use can be extended to anything to store data within a group.
* Buffers can also be swapped as particles are swap within a group. 
	This allows to have the ordering of data consistent with the ordering of particles.
* However, if the buffers are only used for temporary storage on a single frame (most of the renderers),  
	it is not necessary to swap the data.

* A buffer also contains a flag which is an unsigned integer that 
	can be used to check the validity of the buffer from frame to frame.

* Note that only a group can create and delete a buffer.
	The user can ask a group to create a new buffer by passing
	a BufferCreator object to it.

* Check out the group interface about buffers for more info.
*/
class Buffer
{
	friend class BufferCreator;
	friend class Group;
	friend void swapParticles(Particle& a, Particle& b);

public :
	/** Gets the current flag of this buffer */
	uint32 getFlag() const;

	/** Tells whether data is swapped as particles in the group are swapped */
	bool isSwapEnabled() const;

protected :
	Buffer() {};
	virtual ~Buffer() {};

private :
	uint32 flag;
	bool swapEnabled;

	/**
	* Swaps 2 particles data in this buffer
	* This is a pure virtual method that must be implemented by inherited classes of buffer.
	* index0 : the index of the first particle to swap
	* index1 : the index of the second particle to swap
	*/
	virtual void swap(int index0, int index1) = 0;
};

/**
* A class used by a Group to create an additional Buffer
*
* This class defines a temporary object to pass to a Group so that it can create and store a new buffer.
* Check out the method Group::createBuffer for more information.
*/
class BufferCreator
{
	friend class Group;

protected : 
	virtual ~BufferCreator() {}
private :

	/**
	* Creates a new buffer
	*
	* This method is called internally by a group to create and store an new buffer.
	* This is a pure virtual method that must be implemented by inherited classes of buffer.
	*
	*  nbParticles : the number of particles the buffer must be able to store
	*  group : the group which contains the buffer
	*/
	virtual Buffer* createBuffer(int nbParticles, const Group& group) const = 0;
};


inline uint32 Buffer::getFlag() const
{
	return flag;
}

inline bool Buffer::isSwapEnabled() const
{
	return swapEnabled;
}



#endif		// __SGP_SPARKBUFFER_HEADER__