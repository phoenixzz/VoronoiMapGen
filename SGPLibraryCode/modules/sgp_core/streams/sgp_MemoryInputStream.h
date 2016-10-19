
#ifndef __SGP_MEMORYINPUTSTREAM_HEADER__
#define __SGP_MEMORYINPUTSTREAM_HEADER__

#include "sgp_InputStream.h"
#include "../common/sgp_HeapBlock.h"


//==============================================================================
/**
    Allows a block of data and to be accessed as a stream.

    This can either be used to refer to a shared block of memory, or can make its
    own internal copy of the data when the MemoryInputStream is created.
*/
class SGP_API  MemoryInputStream  : public InputStream
{
public:
    //==============================================================================
    /** Creates a MemoryInputStream.

        @param sourceData               the block of data to use as the stream's source
        @param sourceDataSize           the number of bytes in the source data block
        @param keepInternalCopyOfData   if false, the stream will just keep a pointer to
                                        the source data, so this data shouldn't be changed
                                        for the lifetime of the stream; if this parameter is
                                        true, the stream will make its own copy of the
                                        data and use that.
    */
    MemoryInputStream (const void* sourceData,
                       size_t sourceDataSize,
                       bool keepInternalCopyOfData);

    /** Creates a MemoryInputStream.

        @param data                     a block of data to use as the stream's source
        @param keepInternalCopyOfData   if false, the stream will just keep a reference to
                                        the source data, so this data shouldn't be changed
                                        for the lifetime of the stream; if this parameter is
                                        true, the stream will make its own copy of the
                                        data and use that.
    */
    MemoryInputStream (const MemoryBlock& data,
                       bool keepInternalCopyOfData);

    /** Destructor. */
    ~MemoryInputStream();

    /** Returns a pointer to the source data block from which this stream is reading. */
    const void* getData() const noexcept        { return data; }

    /** Returns the number of bytes of source data in the block from which this stream is reading. */
    size_t getDataSize() const noexcept         { return dataSize; }

    //==============================================================================
    int64 getPosition();
    bool setPosition (int64 pos);
    int64 getTotalLength();
    bool isExhausted();
    int read (void* destBuffer, int maxBytesToRead);

private:
    //==============================================================================
    const void* data;
    size_t dataSize, position;
    HeapBlock<char> internalCopy;

    void createInternalCopy();

    SGP_DECLARE_NON_COPYABLE (MemoryInputStream)
};

#endif   // __SGP_MEMORYINPUTSTREAM_HEADER__
