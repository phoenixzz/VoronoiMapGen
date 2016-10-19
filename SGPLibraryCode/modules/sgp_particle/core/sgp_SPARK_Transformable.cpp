
Transformable::Transformable() : parent(NULL), currentUpdate(0),
	lastUpdate(0), lastParentUpdate(0)
{
	localTransform.Identity();
	worldTransform.Identity();
}

Transformable::Transformable(const Transformable& transformable) : parent(NULL),
	currentUpdate(0), lastUpdate(0), lastParentUpdate(0)
{
	localTransform = transformable.localTransform;
	worldTransform = transformable.worldTransform;
}

Transformable& Transformable::operator=(const Transformable& transformable)
{
	parent = NULL;
	currentUpdate = 0;
	lastUpdate = 0;
	lastParentUpdate = 0;

	localTransform = transformable.localTransform;
	worldTransform = transformable.worldTransform;
	return *this;
}

void Transformable::updateTransform(const Transformable* _parent)
{
	if( isUpdateNotified() ||											// the local transform or instance param have been updated
		_parent != this->parent ||										// the parent has changed
		(_parent != NULL && lastParentUpdate != _parent->currentUpdate) )	// the parent transform has been modified
	{
		if( _parent == NULL )
			worldTransform = localTransform;
		else
		{
			worldTransform = _parent->worldTransform * localTransform;
			lastParentUpdate = _parent->lastUpdate;
		}

		this->parent = _parent;
		lastUpdate = ++currentUpdate;
		innerUpdateTransform();
	}

	propagateUpdateTransform();
}
