

MemoryOutputStream::MemoryOutputStream (const size_t initialSize)
  : data (internalBlock),
    position (0),
    size (0)
{
    internalBlock.setSize (initialSize, false);
}

MemoryOutputStream::MemoryOutputStream (MemoryBlock& memoryBlockToWriteTo,
                                        const bool appendToExistingBlockContent)
  : data (memoryBlockToWriteTo),
    position (0),
    size (0)
{
    if (appendToExistingBlockContent)
        position = size = memoryBlockToWriteTo.getSize();
}

MemoryOutputStream::~MemoryOutputStream()
{
    trimExternalBlockSize();
}

void MemoryOutputStream::flush()
{
    trimExternalBlockSize();
}

void MemoryOutputStream::trimExternalBlockSize()
{
    if (&data != &internalBlock)
        data.setSize (size, false);
}

void MemoryOutputStream::preallocate (const size_t bytesToPreallocate)
{
    data.ensureSize (bytesToPreallocate + 1);
}

void MemoryOutputStream::reset() noexcept
{
    position = 0;
    size = 0;
}

void MemoryOutputStream::prepareToWrite (int numBytes)
{
    jassert (numBytes >= 0);
    size_t storageNeeded = position + (size_t) numBytes;

    if (storageNeeded >= data.getSize())
        data.ensureSize ((storageNeeded + jmin (storageNeeded / 2, (size_t) (1024 * 1024)) + 32) & ~31u);
}

bool MemoryOutputStream::write (const void* const buffer, int howMany)
{
    jassert (buffer != nullptr && howMany >= 0);

    if (howMany > 0)
    {
        prepareToWrite (howMany);
        memcpy (static_cast<char*> (data.getData()) + position, buffer, (size_t) howMany);
        position += (size_t) howMany;
        size = jmax (size, position);
    }

    return true;
}

void MemoryOutputStream::writeRepeatedByte (uint8 byte, int howMany)
{
    if (howMany > 0)
    {
        prepareToWrite (howMany);
        memset (static_cast<char*> (data.getData()) + position, byte, (size_t) howMany);
        position += (size_t) howMany;
        size = jmax (size, position);
    }
}

MemoryBlock MemoryOutputStream::getMemoryBlock() const
{
    return MemoryBlock (getData(), getDataSize());
}

const void* MemoryOutputStream::getData() const noexcept
{
    if (data.getSize() > size)
        static_cast <char*> (data.getData()) [size] = 0;

    return data.getData();
}

bool MemoryOutputStream::setPosition (int64 newPosition)
{
    if (newPosition <= (int64) size)
    {
        // ok to seek backwards
        position = jlimit ((size_t) 0, size, (size_t) newPosition);
        return true;
    }
    else
    {
        // trying to make it bigger isn't a good thing to do..
        return false;
    }
}

int MemoryOutputStream::writeFromInputStream (InputStream& source, int64 maxNumBytesToWrite)
{
    // before writing from an input, see if we can preallocate to make it more efficient..
    int64 availableData = source.getTotalLength() - source.getPosition();

    if (availableData > 0)
    {
        if (maxNumBytesToWrite > 0 && maxNumBytesToWrite < availableData)
            availableData = maxNumBytesToWrite;

        preallocate (data.getSize() + (size_t) maxNumBytesToWrite);
    }

    return OutputStream::writeFromInputStream (source, maxNumBytesToWrite);
}

String MemoryOutputStream::toUTF8() const
{
    const char* const d = static_cast <const char*> (getData());
    return String (CharPointer_UTF8 (d), CharPointer_UTF8 (d + getDataSize()));
}

String MemoryOutputStream::toString() const
{
    return String::createStringFromData (getData(), (int) getDataSize());
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const MemoryOutputStream& streamToRead)
{
    const int dataSize = (int) streamToRead.getDataSize();
    if (dataSize > 0)
        stream.write (streamToRead.getData(), dataSize);

    return stream;
}
