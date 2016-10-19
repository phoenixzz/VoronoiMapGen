
//==============================================================================
OutputStream::OutputStream()
    : newLineString (NewLine::getDefault())
{

}

OutputStream::~OutputStream()
{

}

//==============================================================================
void OutputStream::writeBool (const bool b)
{
    writeByte (b ? (char) 1
                 : (char) 0);
}

void OutputStream::writeByte (char byte)
{
    write (&byte, 1);
}

void OutputStream::writeRepeatedByte (uint8 byte, int numTimesToRepeat)
{
    while (--numTimesToRepeat >= 0)
        writeByte ((char) byte);
}

void OutputStream::writeShort (short value)
{
    const unsigned short v = ByteOrder::swapIfBigEndian ((unsigned short) value);
    write (&v, 2);
}

void OutputStream::writeShortBigEndian (short value)
{
    const unsigned short v = ByteOrder::swapIfLittleEndian ((unsigned short) value);
    write (&v, 2);
}

void OutputStream::writeInt (int value)
{
    const unsigned int v = ByteOrder::swapIfBigEndian ((unsigned int) value);
    write (&v, 4);
}

void OutputStream::writeIntBigEndian (int value)
{
    const unsigned int v = ByteOrder::swapIfLittleEndian ((unsigned int) value);
    write (&v, 4);
}

void OutputStream::writeCompressedInt (int value)
{
    unsigned int un = (value < 0) ? (unsigned int) -value
                                  : (unsigned int) value;

    uint8 data[5];
    int num = 0;

    while (un > 0)
    {
        data[++num] = (uint8) un;
        un >>= 8;
    }

    data[0] = (uint8) num;

    if (value < 0)
        data[0] |= 0x80;

    write (data, num + 1);
}

void OutputStream::writeInt64 (int64 value)
{
    const uint64 v = ByteOrder::swapIfBigEndian ((uint64) value);
    write (&v, 8);
}

void OutputStream::writeInt64BigEndian (int64 value)
{
    const uint64 v = ByteOrder::swapIfLittleEndian ((uint64) value);
    write (&v, 8);
}

void OutputStream::writeFloat (float value)
{
    union { int asInt; float asFloat; } n;
    n.asFloat = value;
    writeInt (n.asInt);
}

void OutputStream::writeFloatBigEndian (float value)
{
    union { int asInt; float asFloat; } n;
    n.asFloat = value;
    writeIntBigEndian (n.asInt);
}

void OutputStream::writeDouble (double value)
{
    union { int64 asInt; double asDouble; } n;
    n.asDouble = value;
    writeInt64 (n.asInt);
}

void OutputStream::writeDoubleBigEndian (double value)
{
    union { int64 asInt; double asDouble; } n;
    n.asDouble = value;
    writeInt64BigEndian (n.asInt);
}

void OutputStream::writeString (const String& text)
{
    // (This avoids using toUTF8() to prevent the memory bloat that it would leave behind
    // if lots of large, persistent strings were to be written to streams).
    const int numBytes = text.getNumBytesAsUTF8() + 1;
    HeapBlock<char> temp ((size_t) numBytes);
    text.copyToUTF8 (temp, numBytes);
    write (temp, numBytes);
}

void OutputStream::writeText (const String& text, const bool asUTF16,
                              const bool writeUTF16ByteOrderMark)
{
    if (asUTF16)
    {
        if (writeUTF16ByteOrderMark)
            write ("\x0ff\x0fe", 2);

        String::CharPointerType src (text.getCharPointer());
        bool lastCharWasReturn = false;

        for (;;)
        {
            const sgp_wchar c = src.getAndAdvance();

            if (c == 0)
                break;

            if (c == '\n' && ! lastCharWasReturn)
                writeShort ((short) '\r');

            lastCharWasReturn = (c == L'\r');
            writeShort ((short) c);
        }
    }
    else
    {
        const char* src = text.toUTF8();
        const char* t = src;

        for (;;)
        {
            if (*t == '\n')
            {
                if (t > src)
                    write (src, (int) (t - src));

                write ("\r\n", 2);
                src = t + 1;
            }
            else if (*t == '\r')
            {
                if (t[1] == '\n')
                    ++t;
            }
            else if (*t == 0)
            {
                if (t > src)
                    write (src, (int) (t - src));

                break;
            }

            ++t;
        }
    }
}

int OutputStream::writeFromInputStream (InputStream& source, int64 numBytesToWrite)
{
    if (numBytesToWrite < 0)
        numBytesToWrite = std::numeric_limits<int64>::max();

    int numWritten = 0;

    while (numBytesToWrite > 0)
    {
        char buffer [8192];
        const int num = source.read (buffer, (int) jmin (numBytesToWrite, (int64) sizeof (buffer)));

        if (num <= 0)
            break;

        write (buffer, num);

        numBytesToWrite -= num;
        numWritten += num;
    }

    return numWritten;
}

//==============================================================================
void OutputStream::setNewLineString (const String& newLineString_)
{
    newLineString = newLineString_;
}

//==============================================================================
OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const int number)
{
    return stream << String (number);
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const int64 number)
{
    return stream << String (number);
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const double number)
{
    return stream << String (number);
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const char character)
{
    stream.writeByte (character);
    return stream;
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const char* const text)
{
    stream.write (text, (int) strlen (text));
    return stream;
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const MemoryBlock& data)
{
    if (data.getSize() > 0)
        stream.write (data.getData(), (int) data.getSize());

    return stream;
}


OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const File& fileToRead)
{
    FileInputStream in (fileToRead);

    if (in.openedOk())
        return stream << in;

    return stream;
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, InputStream& streamToRead)
{
    stream.writeFromInputStream (streamToRead, -1);
    return stream;
}

OutputStream& SGP_CALLTYPE operator<< (OutputStream& stream, const NewLine&)
{
    return stream << stream.getNewLineString();
}
