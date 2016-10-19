#ifndef __SGP_SPARKFACTORY_HEADER__
#define __SGP_SPARKFACTORY_HEADER__

// Creates a new registered object from an existing one
// with arg being either an uint32 ID or a pointer to an object and ClassName being the class of the object.
// If the ID does not exist or the object is not registered, NULL is returned.
#define SPARK_Copy(ClassName,arg) \
dynamic_cast<ClassName*>(sgp::SPARK::SPKFactory::getInstance().copy(arg))

// Gets a registered object
// If the ID does not exist, NULL is returned
#define SPARK_Get(ClassName,ID) \
dynamic_cast<ClassName*>(sgp::SPARK::SPKFactory::getInstance().get(ID))

// Destroys a registered object
#define SPARK_Destroy(arg) \
sgp::SPARK::SPKFactory::getInstance().destroy(arg)

// Creates a registered object
#define SPARK_Create(object) \
sgp::SPARK::SPKFactory::getInstance().create(object)


/**
* A singleton class that allows to easily define, create, destroy and organize SPARK objects (Registerable)
*
* A Registerable can either be registered or not. A Registerable created with the SPKFactory becomes registered :
* it is given a unique ID and is stored within a map in the SPKFactory.

* the SPKFactory allows the correct copy of a Registerable with all its children Registerable. Children registerable are only copied 
* within the given Registerable if they are not shared (see Registerable) else only the reference to the shared Registerable is copied

* Moreover, destruction of Registerable and all children is very easy with the factory. Destroying a Registerable will destroy and all its
* children which are destroyable and only referenced within the Registerable being destroyed.
* Note that the process is recursive through the tree, so destroyable children of destroyable children will be destroyed as well and so on.

* 4 main actions are performed by the SPKFactory :

	Create a registered Registerable : This is performed with create(const Registerable&). the passed Registerable is used to create a registered Registerable
		which is an exact copy. Every elements will be copied even the shared ones. The base Registerable can be registered or not.
	Copy a registered Registerable : This is performed with copy(uint32 ID) or copy(const Registerable*). A registered copy of the registered Registerable is created.
		The shared children are only referenced while the not shared ones are copied as well. The same happens for children of children and so on.
	Destroy a registered Registerable : This is performed with destroy(uint32 ID,bool) or destroy(Registerable*,bool). The Registerable and all its destroyable
		registered children will be destroyed. The tree is parsed to destroy children of children and so on. the boolean tells whether to check the number of
		references of children or not.
	Gets a registered Registerable from its ID. This is performed with get(uint32 ID). Every registered Registerable is stored in a map. The user can retrieve
		the address of the Registerable with its ID (if the ID exists in the map).

* Note that macros are implemented to ease the syntax :

	SPK_Create(object)
	SPK_Copy(ClassName,arg)
	SPK_Get(ClassName,arg)
	SPK_Destroy(arg)

*/
class SGP_API SPKFactory
{
	friend class Registerable;

public:
	static SPKFactory& getInstance();
	static void destroyInstance();

	// Returns the number of Regiterable objects registered in the SPKFactory
	inline uint32 getNumberOfObjects() const
	{
		return SPKRegister.size();
	}

	/**
	* Creates a registered Registerable from the passed Registerable
	*  base : The Registerable to create the new registered Registerable from
	* return the ID of the new registered object
	*/
	uint32 create(const Registerable& base);


	/**
	* Creates a new Registerable object which is a copy of the object at the given ID
	*
	* If no Registerable with this ID is registered, NULL is returned.
	*
	*  ID : the ID of the Registerable to copy
	* return a registered copy of the Registerable or NULL if the passed ID is not registered
	*/
	Registerable* copy(uint32 ID);

	/**
	* Creates a new Registerable object which is a copy of the object
	*
	* If the passed Registerable is NULL or not registered, NULL is returned.
	* Note that this function call internally copy(registerable->getSPARKID()).
	*/
	Registerable* copy(const Registerable* registerable);

	/**
	* Gets the Registerable of the given ID
	* If the ID is not registered, NULL is returned
	*/
	Registerable* get(uint32 ID);

	/**
	* Destroys the Registerable with the given ID and all its destroyable children
	*
	* If the ID is not registered, nothing is destroyed and false is returned.

	* The checkNbReferences boolean tells the factory if the number of references of the Registerable to be destroyed has to be checked.
	* If set to true, the Registerable will be destroyed only if the number or references within the SPKFactory 
			(ie in all registered object in the SPKFactory) is 0.
	* If set to false, the Registerable will be destroyed in any case. Meaning that any reference within the SPKFactory
			becomes invalid.

	* IN param ID : the ID of the Registerable to destroy
	* IN param checkNbReferences : true to destroy only a Registerable with no references in the SPKFactory (safer), false not to perform the check
	* return true if the Registerable has been deleted, false if not
	*/
	bool destroy(uint32 ID, bool checkNbReferences = true);


	/**
	* Destroys the Registerable and all its destroyable children

	* If the Registerable is NULL or is not registered, nothing is destroyed and false is returned.
	* Note that this function call internally destroy(registerable->getSPARKID())

	* IN param registerable the Registerable to destroy
	* IN param checkNbReferences : true to destroy only a Registerable with no references in the SPKFactory (safer), false not to perform the check
	* return true if the Registerable has been deleted, false if not
	*/
	bool destroy(Registerable* registerable, bool checkNbReferences = true);

	// Destroys all the registered Registerable in the SPKFactory
	void destroyAll();

/*
	// Trace information on the Registerable with the given ID or Registerable
	void trace(uint32 ID);
	void trace(const Registerable* registerable);
	// Trace information on all the registered Registerable within the SPKFactory 
	void traceAll();
*/

	/**
	* Finds a registerable by name in the factory
	* Note that this method only checks registerables in the SPKFactory.
	* This method does not call the Registerable::findByName(const String&) of the registerables to check recursively.
	* return the first registerable with that name or NULL of none is found
	**/
	Registerable* findByName(const String& name);

private:
	static SPKFactory* instance;
	static uint32 currentID;

	HashMap<uint32, Registerable*> SPKRegister;
	HashMap<const Registerable*, Registerable*> SPKAdresses;

	//void traceObject(const HashMap<uint32, Registerable*>& Register, bool nextLine);

	inline bool isAlreadyProcessed(const Registerable* source)
	{
		return SPKAdresses.contains(source);
	}
	inline Registerable* getProcessedObject(const Registerable* source)
	{
		if( SPKAdresses.contains(source) )
			return SPKAdresses[source];
		return NULL;
	}

	inline void markAsProcessed(const Registerable* source, Registerable* object)
	{
		SPKAdresses.set(source, object);
	}

	Registerable* registerObject(Registerable* object);
	bool unregisterObject(uint32 ID, bool keepChildren = false);	

	// private constructors
	SPKFactory(){};
	SPKFactory(const SPKFactory&){};
	SPKFactory& operator=(const SPKFactory&);
	~SPKFactory(){this->destroyAll();}
};
#endif		// __SGP_SPARKFACTORY_HEADER__