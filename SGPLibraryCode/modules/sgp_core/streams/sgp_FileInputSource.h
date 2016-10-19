#ifndef __SGP_FILEINPUTSOURCE_HEADER__
#define __SGP_FILEINPUTSOURCE_HEADER__

//==============================================================================
/**
    A type of InputSource that represents a normal file.

    @see InputSource
*/
class SGP_API  FileInputSource     : public InputSource
{
public:
    //==============================================================================
    /** Creates a FileInputSource for a file.
        If the useFileTimeInHashGeneration parameter is true, then this object's
        hashCode() method will incorporate the file time into its hash code; if
        false, only the file name will be used for the hash.
    */
    FileInputSource (const File& file, bool useFileTimeInHashGeneration = false);

    /** Destructor. */
    ~FileInputSource();

    InputStream* createInputStream();
    InputStream* createInputStreamFor (const String& relatedItemPath);
    int64 hashCode() const;

private:
    //==============================================================================
    const File file;
    bool useFileTimeInHashGeneration;

    SGP_DECLARE_NON_COPYABLE (FileInputSource)
};

#endif		// __SGP_FILEINPUTSOURCE_HEADER__