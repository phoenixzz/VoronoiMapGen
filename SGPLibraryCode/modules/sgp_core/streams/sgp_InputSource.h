#ifndef __SGP_INPUTSOURCE_HEADER__
#define __SGP_INPUTSOURCE_HEADER__

//==============================================================================
/**
    A lightweight object that can create a stream to read some kind of resource.

    This may be used to refer to a file, or some other kind of source, allowing a
    caller to create an input stream that can read from it when required.


*/
class SGP_API  InputSource
{
public:
    //==============================================================================
    InputSource() noexcept      {}

    /** Destructor. */
    virtual ~InputSource()      {}

    //==============================================================================
    /** Returns a new InputStream to read this item.

        @returns            an inputstream that the caller will delete, or nullptr if
                            the filename isn't found.
    */
    virtual InputStream* createInputStream() = 0;

    /** Returns a new InputStream to read an item, relative.

        @param relatedItemPath  the relative pathname of the resource that is required
        @returns            an inputstream that the caller will delete, or nullptr if
                            the item isn't found.
    */
    virtual InputStream* createInputStreamFor (const String& relatedItemPath) = 0;

    /** Returns a hash code that uniquely represents this item.
    */
    virtual int64 hashCode() const = 0;



};

#endif		// __SGP_INPUTSOURCE_HEADER__