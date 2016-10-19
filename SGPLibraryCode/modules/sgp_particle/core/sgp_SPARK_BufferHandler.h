#ifndef __SGP_SPARKBUFFERHANDLER_HEADER__
#define __SGP_SPARKBUFFERHANDLER_HEADER__

class Group;

#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif


/**
* A base interface that allows manipulation on Buffer
*/
class SGP_API BufferHandler
{
public :
	virtual ~BufferHandler() {}

	/**
	* Enables or disables the automatic buffers creation in a static way
	* By default, the buffers creation is enabled.
	* param  true to enable the buffers creation, false to disable it
	*/
	static void enableBuffersCreation(bool creation);


	// Tells whether the automatic buffers creation is enabled or not
	static bool isBuffersCreationEnabled();

	/**
	* Creates the buffers for this buffer handler in the given group
	* If the buffers for this type of buffer handler already exists within the Group, they are priorly destroyed.
	* The type of buffers created depends on the state of the buffer handler at the time this method is called.
	* This method has to be overridden in derived classes that use buffers
	* param group : the Group in which to create the buffers for this buffer handler
	*/
	virtual void createBuffers(const Group& group) {};

	/**
	* Destroys the buffers for this buffer handler in the given group
	* if the buffers dont exist, nothing happens.
	* This method has to be overridden in derived classes that use buffers
	* param group : the Group in which to destroy the buffers for this buffer handler
	*/
	virtual void destroyBuffers(const Group& group) {};

protected :

	// The constructor is private so that the class is not instanciable
	BufferHandler() {}

	/**
	* prepares the buffers of the given Group for processing
	* Internally, this methods perfoms the following operations :

	if check buffers is false
		if buffers creation is enabled
			destroy buffers
			create buffers
			return true
		else return false
	else return true


	* param group : the group whose buffers must be prepared
	* return true if the buffers are ready, false if not
	*/
	bool prepareBuffers(const Group& group);

	/**
	* Checks the buffers and prepare them
	* This method has to be implemented in derived class that uses buffers.
	* true must be returned if the buffers are found and initialized, false otherwise.
	*/
	virtual bool checkBuffers(const Group& group);

private :
	static bool bufferCreation;
};

inline bool BufferHandler::checkBuffers(const Group& group)
{
	return true;
}

#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKBUFFERHANDLER_HEADER__