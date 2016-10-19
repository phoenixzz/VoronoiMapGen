
const uint32 NO_SPARK_ID = 0;
const String NO_SPARK_NAME = String::empty;

Registerable::Registerable() : ID(NO_SPARK_ID), nbReferences(0),
	shared(false), destroyable(true), name(NO_SPARK_NAME)
{}

Registerable::Registerable(const Registerable& registerable) :
	ID(NO_SPARK_ID), nbReferences(0),
	shared(registerable.shared), destroyable(registerable.destroyable),
	name(registerable.name)
{}

Registerable::~Registerable(){}

Registerable* Registerable::copyChild(Registerable* child, bool createBase)
{
	if( !child )
		return NULL;

	if( (child->isRegistered() && !child->isShared()) || createBase )
	{
		if( SPKFactory::getInstance().isAlreadyProcessed(child) )
		{
			Registerable* processedClone = SPKFactory::getInstance().getProcessedObject(child);
			processedClone->incrementReference();
			return processedClone;
		}

		Registerable* cloneChild = child->clone(createBase);
		SPKFactory::getInstance().registerObject(cloneChild);
		cloneChild->incrementReference();
		SPKFactory::getInstance().markAsProcessed(child,cloneChild);
		return cloneChild;
	}

	child->incrementReference();
	return child;
}

bool Registerable::destroyChild(Registerable* child, bool keepChildren)
{
	if( !child || keepChildren )
		return false;

	child->decrementReference();

	if( child->isRegistered() &&
		child->isDestroyable() &&
		(child->getNumOfReferences() == 0) )
	{
		SPKFactory::getInstance().unregisterObject( child->getSPARKID() );
		return true;
	}

	return false;
}

void Registerable::registerChild(Registerable* child, bool registerAll)
{
	if( !child )
		return;

	if( child->isRegistered() )
	{
		child->incrementReference();
		child->registerChildren(registerAll);
	}
	else if( registerAll )
	{
		SPKFactory::getInstance().registerObject(child);
		child->incrementReference();
		child->registerChildren(registerAll);
	}
}

void Registerable::registerObject(Registerable* obj, bool registerAll)
{
	if( (obj != NULL) && !obj->isRegistered() )
	{
		SPKFactory::getInstance().registerObject(obj);
		obj->registerChildren(registerAll);
	}
}