

int64 sgp_fileSetPosition (void* handle, int64 pos);

//==============================================================================
FileInputStream::FileInputStream (const File& f)
    : file (f),
      fileHandle (nullptr),
      currentPosition (0),
      status (Result::ok()),
      needToSeek (true)
{
    openHandle();
}

FileInputStream::~FileInputStream()
{
    closeHandle();
}

//==============================================================================
int64 FileInputStream::getTotalLength()
{
    return file.getSize();
}

int FileInputStream::read (void* buffer, int bytesToRead)
{
    jassert (openedOk());
    jassert (buffer != nullptr && bytesToRead >= 0);

    if (needToSeek)
    {
        if (sgp_fileSetPosition (fileHandle, currentPosition) < 0)
            return 0;

        needToSeek = false;
    }

    const size_t num = readInternal (buffer, (size_t) bytesToRead);
    currentPosition += num;

    return (int) num;
}

bool FileInputStream::isExhausted()
{
    return currentPosition >= getTotalLength();
}

int64 FileInputStream::getPosition()
{
    return currentPosition;
}

bool FileInputStream::setPosition (int64 pos)
{
    jassert (openedOk());

    if (pos != currentPosition)
    {
        pos = jlimit ((int64) 0, getTotalLength(), pos);

        needToSeek |= (currentPosition != pos);
        currentPosition = pos;
    }

    return true;
}
