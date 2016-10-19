

#ifndef __SGP_FILEINPUTSTREAM_HEADER__
#define __SGP_FILEINPUTSTREAM_HEADER__

#include "sgp_File.h"
#include "../streams/sgp_InputStream.h"


//==============================================================================
/**
    An input stream that reads from a local file.

    @see InputStream, FileOutputStream, File::createInputStream
*/
class SGP_API  FileInputStream  : public InputStream
{
public:
    //==============================================================================
    /** Creates a FileInputStream.

        @param fileToRead   the file to read from - if the file can't be accessed for some
                            reason, then the stream will just contain no data
    */
    explicit FileInputStream (const File& fileToRead);

    /** Destructor. */
    ~FileInputStream();

    //==============================================================================
    /** Returns the file that this stream is reading from. */
    const File& getFile() const noexcept                { return file; }

    /** Returns the status of the file stream.
        The result will be ok if the file opened successfully. If an error occurs while
        opening or reading from the file, this will contain an error message.
    */
    const Result& getStatus() const noexcept            { return status; }

    /** Returns true if the stream couldn't be opened for some reason.
        @see getResult()
    */
    bool failedToOpen() const noexcept                  { return status.failed(); }

    /** Returns true if the stream opened without problems.
        @see getResult()
    */
    bool openedOk() const noexcept                      { return status.wasOk(); }


    //==============================================================================
    int64 getTotalLength();
    int read (void* destBuffer, int maxBytesToRead);
    bool isExhausted();
    int64 getPosition();
    bool setPosition (int64 pos);

private:
    //==============================================================================
    File file;
    void* fileHandle;
    int64 currentPosition;
    Result status;
    bool needToSeek;

    void openHandle();
    void closeHandle();
    size_t readInternal (void* buffer, size_t numBytes);

    SGP_DECLARE_NON_COPYABLE (FileInputStream)
};

#endif   // __SGP_FILEINPUTSTREAM_HEADER__
