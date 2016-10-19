

MemoryInputStream::MemoryInputStream (const void* const sourceData,
                                      const size_t sourceDataSize,
                                      const bool keepInternalCopy)
    : data (sourceData),
      dataSize (sourceDataSize),
      position (0)
{
    if (keepInternalCopy)
        createInternalCopy();
}

MemoryInputStream::MemoryInputStream (const MemoryBlock& sourceData,
                                      const bool keepInternalCopy)
    : data (sourceData.getData()),
      dataSize (sourceData.getSize()),
      position (0)
{
    if (keepInternalCopy)
        createInternalCopy();
}

void MemoryInputStream::createInternalCopy()
{
    internalCopy.malloc (dataSize);
    memcpy (internalCopy, data, dataSize);
    data = internalCopy;
}

MemoryInputStream::~MemoryInputStream()
{
}

int64 MemoryInputStream::getTotalLength()
{
    return dataSize;
}

int MemoryInputStream::read (void* const buffer, const int howMany)
{
    jassert (buffer != nullptr && howMany >= 0);

    const int num = jmin (howMany, (int) (dataSize - position));
    if (num <= 0)
        return 0;

    memcpy (buffer, addBytesToPointer (data, position), (size_t) num);
    position += (unsigned int) num;
    return num;
}

bool MemoryInputStream::isExhausted()
{
    return position >= dataSize;
}

bool MemoryInputStream::setPosition (const int64 pos)
{
    position = (size_t) jlimit ((int64) 0, (int64) dataSize, pos);
    return true;
}

int64 MemoryInputStream::getPosition()
{
    return position;
}
