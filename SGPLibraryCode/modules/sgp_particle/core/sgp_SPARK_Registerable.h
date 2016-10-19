#ifndef __SGP_SPARKREGISTERABLE_HEADER__
#define __SGP_SPARKREGISTERABLE_HEADER__


#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4100)
#endif


// A macro implementing the clone method for Registerable children
// Note that copyChildren and destroyChildren have to be implemented manually if needed
#define SPARK_IMPLEMENT_REGISTERABLE(ClassName) \
\
virtual ClassName* clone(bool createBase) const \
{ \
	ClassName* cloneObject = new ClassName(*this); \
	cloneObject->copyChildren(*this,createBase); \
	return cloneObject; \
} \
\
public : \
virtual String getClassName() const {return #ClassName;}





/** The ID constant value for unregistered Registerable */
extern const uint32 NO_SPARK_ID;

/** An empty string defining the name of an object with no name */
extern const String NO_SPARK_NAME;




/**
* The abstract base class for SPARK objects
*
* A Registerable defines a SPARK object that can be managed by the SPKFactory.

* In SPARK, a particle System is defined as a tree of objects. For instance, a System
* will contain some Renderer, Emitter and Modifier. Every Emitter and Modifier will
* contain a Zone...
* All those Registerable are linked by association. A Registerable can be shared to several
* Registerable or belong to a single Registerable and die with it (composition).

* The SPKFactory offers a flexible system to define, create and destroy complex association
* of Registerable.

* Basically a Registerable has 3 parameters that will define its behavior :

* Registered : a registered Registerable is managed by the SPKFactory to deal with copy and destruction.
* Shared     : a shared Registerable will not be copied when copying its parent. Only its reference will. This allows
				for instance to change a parameter for many system with only a call. Note that this is valid only for registered
				Registerable copied with the SPKFactory.
* Destroyable: a destroyable Registerable is a Registerable that is allowed to be destroyed by the SPKFactory when destroying
				its parent (if it has no more references on it see below).
				If not destroyable, the destruction of the Registerable is the user's responsability.
* Note that a non destroyable Registerable can still be destroyed by the SPKFactory by a direct call.

* Moreover, every registered Registerable holds a counter of references that indicates how many registered Registerable hold a reference to it.
* A registered Registerable will only be destroyed by the SPKFactory if its number of references is 0 (except for a direct call to its destruction).
*/
class SGP_API Registerable
{
	friend class SPKFactory;

public:
	Registerable();
	Registerable(const Registerable& registerable);
	virtual ~Registerable();

	/**
	* Makes this Registerable shared or not
	* By default, a Registerable is not shared
	*/
	void setShared(bool shared);

	/**
	* Makes this Registerable destroyable or not
	* A non destroyable Registerable cannot be destroyed internally.
	*/
	void setDestroyable(bool destroyable);

	/**
	* Sets the name of this Registerable
	* The name is an easy to find registerable in a tree.
	* See getName() and findByName(const String&)
	* A constant NO_SPARK_NAME exists to give no name to the registerable (an empty string)
	*/
	void setName(const String& name);



	/**
	* Gets the ID of this Registerable
	*
	* If this Registerable is unregistered, NO_SPARK_ID is returned.
	* Else an uint32 is returned. This ID uniquely identifies the Registerable.
	* return the ID of this Registerable or NO_SPARK_ID if it is not registered
	*/
	uint32 getSPARKID() const;



	/**
	* Gets the number of references of this Registerable
	* The number of references defines the number of times a registered Registerable
	* is references within all the registered Registerable.
	* 0 is always returned for an unregistered Registerable.
	*/
	uint32 getNumOfReferences() const;
		
	/**
	* Tells whether this Registerable is registered or not
	* true if this Registerable is registered, false if not
	*/
	bool isRegistered() const;

	/**
	* Tells whether this Registerable is shared or not
	* true if this Registerable is shared, false if not
	*/
	bool isShared() const;

	/**
	* Tells whether this Registerable is destroyable or not
	* true if this Registerable is destroyable, false if not
	*/
	bool isDestroyable() const;

	/**
	* Gets the name of this registerable

	* The name is an easy to find registerable in a tree.
	* See setName(const String&) and findByName(const String&)

	*/
	const String& getName() const;

	/**
	* Gets the name of the class of the object
	* This method is implemented in non abstract derived class of Registerable with the macro SPARK_IMPLEMENT_REGISTERABLE(ClassName).
	*/
	virtual String getClassName() const = 0;



	/**
	* Finds a registerable with its name recursively from this registerable
	*
	* If the name is not found, NULL is returned.
	* If the several objects with the same name exists, only the first one is returned.

	* Note that the name of the registerable itself is already tested.
	*
	* @param name : the name of the registerable to find
	* return the first registerable with that name within this registerable or NULL if none is found

	*/
	virtual Registerable* findByName(const String& name);

protected:
	/**
	* Registers a child of this Registerable
	*
	* This method has to be called in the registerChildren(bool) implementation of a derived class of Registerable.
	* It is called to allow correct registering and correct reference counting of the children Registerable when registering the Registerable.
	*
	* @param child : the child of this Registerable to register
	* @param registerAll : true to register an unregistered child, false to only increment ref counts of an already registered child
	*/
	void registerChild(Registerable* child, bool registerAll);

	/**
	* Copies a child of this Registerable
	*
	* This method has to be called in the copyChildren(const Registerable&,bool) implementation of a derived class of Registerable.
	* It is called to allow correct copy (of the object or of the reference only) of the children Registerable when copying the Registerable.
	*
	* @param child : the child of this Registerable to copy
	* @param createBase : true if a base is created, false otherwise
	* @return the children of the copy of this Registerable
	*/
	Registerable* copyChild(Registerable* child, bool createBase);

	/**
	* Destroys a child of this Registerable
	*
	* This method has to be called in the destroyChildren(bool) implementation of a derived class of Registerable.
	* It is called to allows the correct destruction (if not destroyable or references exist) of the children Registerable when destroying the Registerable.
	*
	* @param child : the child of this Registerable to destroy
	* @param keepChildren : true to keep the children (used when destroying all registered Registerable)
	* @return true if the child was destroyed, false if not
	*/
	bool destroyChild(Registerable* child, bool keepChildren);

	/**
	* Increments the number of references of the child by one
	*
	* This method has to be called when adding a child in the implementation of a derived class of Registerable.
	* It allows to keep the number of references of the child correct.
	*
	* @param child : the child of this Registerable to increment references of
	*/
	void incrementChildReference(Registerable* child);

	/**
	* Decrements the number of references of the child by one
	*
	* This method has to be called when removing a child in the implementation of a derived class of Registerable.
	* It allows to keep the number of references of the child correct.
	*
	* @param child : the child of this Registerable to decrement references of
	*/
	void decrementChildReference(Registerable* child);

	/**
	* Registers a Registerable in the factory
	*
	* This method allows to register an unregistered given object.
	* If the registerable is already registered nothing happen.
	
	* If registerAll is set to true, all the unregistered children of this Registerable will be registered as well.
	* Apart from that, all registered children see their reference count increments by one, no matter the value of registerAll.

	* Use this method with care as it is very important to never register a object that is allocated on the stack, as the factory
	* may delete its registered object with a call to delete.
	*
	* @param obj : the registerable object to register
	* @param registerAll : true to register all its unregistered children and chidren of children and so on, false not to
	*/
	static void registerObject(Registerable* obj, bool registerAll = false);

	/////////////////////
	// Virtual methods //
	/////////////////////

	/**
	* Registers the children of this Registerable
	*
	* This method has to be implemented in derived classes of Registerable which hold pointers or references of Registerable children.
	* The registerChild(Registerable*,bool) has to be called within it for each child to copy from object.
	* The registerAll parameter of registerChild is simply the registerAll parameter of registerChildren.
	*
	* registerAll : true to register unregistered children, false to only increment ref counts of already registered children

	*/
	virtual void registerChildren(bool registerAll){};

	/**
	* Copies the children of object in this Registerable
	*
	* This method has to be implemented in derived classes of Registerable which hold pointers or references of Registerable children.
	* The copyChild(Registerable*,bool) has to be called within it for each child to copy from object.
	* The createBase parameter of copyChild is simply the createBase parameter of copyChildren.
	*
	* @param object : the object to copy the children from
	* @param createBase : true if a base is created, false otherwise
	*/
	virtual void copyChildren(const Registerable& object, bool createBase){};

	/**
	* Destroys the children of this Registerable
	*
	* This method has to be implemented in derived classes of Registerable which hold pointers or references of Registerable children.
	* The destroyChild(Registerable*,bool) has to be called within it for each child to destroy.
	* The keepChildren parameter of destroyChild is simply the keepChildren parameter of destroyChildren.
	*
	* @param keepChildren : true to keep the children (used when destroying all registered Registerable)
	*/
	virtual void destroyChildren(bool keepChildren){};

private:
	uint32 ID;
	uint32 nbReferences;

	String name;

	bool shared;
	bool destroyable;

	// Those methods allow specific behavior when registering and unregistering in the SPKFactory
	virtual void onRegister() {}	// Called when a registerable is registered in the SPKFactory
	virtual void onUnregister() {}	// Called when a registerable is unregistered from the SPKFactory

	void incrementReference();
	void decrementReference();

	// the assignment operator is private
	Registerable& operator=(const Registerable& registerable);

	/////////////////////////
	// Pure virtual method //
	/////////////////////////

	/**
	* Clones this Registerable
	*
	* This method is implemented in non abstract derived class of Registerable with the macro SPARK_IMPLEMENT_REGISTERABLE(ClassName).
	*
	* @param createBase : true if a base is created, false otherwise
	* @return the clone of this Registerable
	*/
	virtual Registerable* clone(bool createBase) const = 0;
};


inline void Registerable::setShared(bool shared)
{
	this->shared = shared;
}
	
inline void Registerable::setDestroyable(bool destroyable)
{
	this->destroyable = destroyable;
}

inline void Registerable::setName(const String& name)
{
	this->name = name;
}

inline uint32 Registerable::getSPARKID() const
{
	return ID;
}



inline uint32 Registerable::getNumOfReferences() const
{
	return nbReferences;
}

inline bool Registerable::isRegistered() const
{
	return ID != NO_SPARK_ID;
}

inline bool Registerable::isShared() const
{
	return shared;
}

inline bool Registerable::isDestroyable() const
{
	return destroyable;
}

inline const String& Registerable::getName() const
{
	return name;
}

inline Registerable* Registerable::findByName(const String& n)
{
	return getName().compare(n) == 0 ? this : NULL;
}

inline void Registerable::incrementReference()
{
	if( isRegistered() )
		++nbReferences;
}

inline void Registerable::decrementReference()
{
	if( (isRegistered()) && (nbReferences > 0) )
		--nbReferences;
}

inline void Registerable::incrementChildReference(Registerable* child)
{
	if( (isRegistered()) && (child != NULL) )
		child->incrementReference();
}

inline void Registerable::decrementChildReference(Registerable* child)
{
	if( (isRegistered()) && (child != NULL) )
		child->decrementReference();
}



#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif		// __SGP_SPARKREGISTERABLE_HEADER__