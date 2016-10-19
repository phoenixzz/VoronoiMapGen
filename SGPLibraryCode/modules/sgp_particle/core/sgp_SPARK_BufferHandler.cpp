
bool BufferHandler::bufferCreation = true;

void BufferHandler::enableBuffersCreation(bool creation)
{
	bufferCreation = creation;
}

bool BufferHandler::isBuffersCreationEnabled()
{
	return bufferCreation;
}

bool BufferHandler::prepareBuffers(const Group& group)
{
	if( !checkBuffers(group) )
	{
		if( isBuffersCreationEnabled() )
		{
			destroyBuffers(group);
			createBuffers(group);
			return true;
		}
		return false;
	}
	return true;
}