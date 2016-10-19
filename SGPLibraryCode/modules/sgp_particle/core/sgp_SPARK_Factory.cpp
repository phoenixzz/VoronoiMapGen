

SPKFactory* SPKFactory::instance = NULL;
uint32 SPKFactory::currentID = NO_SPARK_ID;

SPKFactory& SPKFactory::getInstance()
{
	if( instance == NULL )
		instance = new SPKFactory;
	return *instance;
}

void SPKFactory::destroyInstance()
{
	if( instance != NULL )
	{
		delete instance;
		instance = NULL;
	}
}

uint32 SPKFactory::create(const Registerable& base)
{
	// Clears the adresses set
	SPKAdresses.clear();

	// clones the reference
	Registerable* innerBase = base.clone(true);

	// registers the base
	registerObject(innerBase);

	return innerBase->ID;
}

Registerable* SPKFactory::get(uint32 ID)
{
	if( SPKRegister.contains( ID ) )
		return SPKRegister[ID];		// the ID was found

	return NULL;					// the ID is not registered
}

Registerable* SPKFactory::copy(uint32 ID)
{
	// Clears the adresses set
	SPKAdresses.clear();

	if( SPKRegister.contains( ID ) )
		return registerObject( SPKRegister[ID]->clone(false) );	// registers a copy

	return NULL;						// the ID is not registered
}

Registerable* SPKFactory::copy(const Registerable* registerable)
{
	// Clears the adresses set
	SPKAdresses.clear();

	if( registerable->isRegistered() )
		return registerObject( registerable->clone(false) );	// registers a copy
	return NULL;
}

bool SPKFactory::destroy(uint32 ID, bool checkNbReferences)
{
	if( SPKRegister.contains(ID) &&					// the ID was found
		((!checkNbReferences) || (SPKRegister[ID]->getNumOfReferences() == 0)) )
	{
		unregisterObject(ID, false);
		return true;
	}

	return false;					// the ID is not registered
}

bool SPKFactory::destroy(Registerable* object, bool checkNbReferences)
{
	if( object == NULL )
		return false;

	return destroy(object->getSPARKID(), checkNbReferences);
}

void SPKFactory::destroyAll()
{
	for( HashMap<uint32, Registerable*>::Iterator it (SPKRegister); it.next(); )
	{
		Registerable* object = it.getValue();
		object->onUnregister();

		object->ID = NO_SPARK_ID;
		object->destroyChildren(true);			// unregister and destroy registered children	
		delete object;							// destroy object
		object = NULL;
	}
	SPKRegister.clear();
}

Registerable* SPKFactory::findByName(const String& name)
{
	for( HashMap<uint32, Registerable*>::Iterator it (SPKRegister); it.next(); )
		if( it.getValue()->getName().compare(name) == 0 )
			return it.getValue();
	return NULL;
}


Registerable* SPKFactory::registerObject(Registerable* object)
{
	object->onRegister();
	object->ID = ++currentID;
		
	SPKRegister.set(object->ID, object);

	return object;
}



bool SPKFactory::unregisterObject(uint32 ID, bool keepChildren)
{
	if( SPKRegister.contains(ID) )
	{
		Registerable* object = SPKRegister[ID];
		object->onUnregister();


		SPKRegister.remove(ID);					// unregister object
		object->ID = NO_SPARK_ID;
		object->destroyChildren(keepChildren);	// unregister and destroy registered children	
		delete object;							// destroy object
		object = NULL;

		return true;
	}

	return false;
}