

int64 sgp_fileSetPosition (void* handle, int64 pos);

//==============================================================================
FileOutputStream::FileOutputStream (const File& f, const int bufferSize_)
    : file (f),
      fileHandle (nullptr),
      status (Result::ok()),
      currentPosition (0),
      bufferSize (bufferSize_),
      bytesInBuffer (0),
      buffer ((size_t) jmax (bufferSize_, 16))
{
    openHandle();
}

FileOutputStream::~FileOutputStream()
{
    flushBuffer();
    flushInternal();
    closeHandle();
}

int64 FileOutputStream::getPosition()
{
    return currentPosition;
}

bool FileOutputStream::setPosition (int64 newPosition)
{
    if (newPosition != currentPosition)
    {
        flushBuffer();
        currentPosition = sgp_fileSetPosition (fileHandle, newPosition);
    }

    return newPosition == currentPosition;
}

bool FileOutputStream::flushBuffer()
{
    bool ok = true;

    if (bytesInBuffer > 0)
    {
        ok = (writeInternal (buffer, bytesInBuffer) == bytesInBuffer);
        bytesInBuffer = 0;
    }

    return ok;
}

void FileOutputStream::flush()
{
    flushBuffer();
    flushInternal();
}

bool FileOutputStream::write (const void* const src, const int numBytes)
{
    jassert (src != nullptr && numBytes >= 0);

    if (bytesInBuffer + numBytes < bufferSize)
    {
        memcpy (buffer + bytesInBuffer, src, (size_t) numBytes);
        bytesInBuffer += numBytes;
        currentPosition += numBytes;
    }
    else
    {
        if (! flushBuffer())
            return false;

        if (numBytes < bufferSize)
        {
            memcpy (buffer + bytesInBuffer, src, (size_t) numBytes);
            bytesInBuffer += numBytes;
            currentPosition += numBytes;
        }
        else
        {
            const int bytesWritten = writeInternal (src, numBytes);

            if (bytesWritten < 0)
                return false;

            currentPosition += bytesWritten;
            return bytesWritten == numBytes;
        }
    }

    return true;
}

void FileOutputStream::writeRepeatedByte (uint8 byte, int numBytes)
{
    jassert (numBytes >= 0);

    if (bytesInBuffer + numBytes < bufferSize)
    {
        memset (buffer + bytesInBuffer, byte, (size_t) numBytes);
        bytesInBuffer += numBytes;
        currentPosition += numBytes;
    }
    else
    {
        OutputStream::writeRepeatedByte (byte, numBytes);
    }
}
