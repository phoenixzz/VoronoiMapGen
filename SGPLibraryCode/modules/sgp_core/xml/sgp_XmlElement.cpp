XmlElement::XmlAttributeNode::XmlAttributeNode (const XmlAttributeNode& other) noexcept
    : name (other.name),
      value (other.value)
{
}

XmlElement::XmlAttributeNode::XmlAttributeNode (const String& name_, const String& value_) noexcept
    : name (name_),
      value (value_)
{
   #if SGP_DEBUG
    // this checks whether the attribute name string contains any illegal characters..
    for (String::CharPointerType t (name.getCharPointer()); ! t.isEmpty(); ++t)
        jassert (t.isLetterOrDigit() || *t == '_' || *t == '-' || *t == ':');
   #endif
}

inline bool XmlElement::XmlAttributeNode::hasName (const String& nameToMatch) const noexcept
{
    return name.equalsIgnoreCase (nameToMatch);
}

//==============================================================================
XmlElement::XmlElement (const String& tagName_) noexcept
    : tagName (tagName_)
{
    // the tag name mustn't be empty, or it'll look like a text element!
    jassert (tagName_.containsNonWhitespaceChars())

    // The tag can't contain spaces or other characters that would create invalid XML!
    jassert (! tagName_.containsAnyOf (" <>/&"));
}

XmlElement::XmlElement (int /*dummy*/) noexcept
{
}

XmlElement::XmlElement (const XmlElement& other)
    : tagName (other.tagName)
{
    copyChildrenAndAttributesFrom (other);
}

XmlElement& XmlElement::operator= (const XmlElement& other)
{
    if (this != &other)
    {
        removeAllAttributes();
        deleteAllChildElements();

        tagName = other.tagName;

        copyChildrenAndAttributesFrom (other);
    }

    return *this;
}



void XmlElement::copyChildrenAndAttributesFrom (const XmlElement& other)
{
    jassert (firstChildElement.get() == nullptr);
    firstChildElement.addCopyOfList (other.firstChildElement);

    jassert (attributes.get() == nullptr);
    attributes.addCopyOfList (other.attributes);
}

XmlElement::~XmlElement() noexcept
{
    firstChildElement.deleteAll();
    attributes.deleteAll();
}

//==============================================================================
namespace XmlOutputFunctions
{
    static bool isLegalXmlChar (const uint32 c) noexcept
    {
        static const unsigned char legalChars[] = { 0, 0, 0, 0, 187, 255, 255, 175, 255, 255, 255, 191, 254, 255, 255, 127 };

        return c < sizeof (legalChars) * 8
                 && (legalChars [c >> 3] & (1 << (c & 7))) != 0;
    }

    static void escapeIllegalXmlChars (OutputStream& outputStream, const String& text, const bool changeNewLines)
    {
        String::CharPointerType t (text.getCharPointer());

        for (;;)
        {
            const uint32 character = (uint32) t.getAndAdvance();

            if (character == 0)
                break;

            if (isLegalXmlChar (character))
            {
                outputStream << (char) character;
            }
            else
            {
                switch (character)
                {
                case '&':   outputStream << "&amp;"; break;
                case '"':   outputStream << "&quot;"; break;
                case '>':   outputStream << "&gt;"; break;
                case '<':   outputStream << "&lt;"; break;

                case '\n':
                case '\r':
                    if (! changeNewLines)
                    {
                        outputStream << (char) character;
                        break;
                    }
                    // Note: deliberate fall-through here!
                default:
                    outputStream << "&#" << ((int) character) << ';';
                    break;
                }
            }
        }
    }

    static void writeSpaces (OutputStream& out, const int numSpaces)
    {
        out.writeRepeatedByte (' ', numSpaces);
    }
}

void XmlElement::writeElementAsText (OutputStream& outputStream,
                                     const int indentationLevel,
                                     const int lineWrapLength) const
{
    using namespace XmlOutputFunctions;

    if (indentationLevel >= 0)
        writeSpaces (outputStream, indentationLevel);

    if (! isTextElement())
    {
        outputStream.writeByte ('<');
        outputStream << tagName;

        {
            const int attIndent = indentationLevel + tagName.length() + 1;
            int lineLen = 0;

            for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
            {
                if (lineLen > lineWrapLength && indentationLevel >= 0)
                {
                    outputStream << newLine;
                    writeSpaces (outputStream, attIndent);
                    lineLen = 0;
                }

                const int64 startPos = outputStream.getPosition();
                outputStream.writeByte (' ');
                outputStream << att->name;
                outputStream.write ("=\"", 2);
                escapeIllegalXmlChars (outputStream, att->value, true);
                outputStream.writeByte ('"');
                lineLen += (int) (outputStream.getPosition() - startPos);
            }
        }

        if (firstChildElement != nullptr)
        {
            outputStream.writeByte ('>');

            bool lastWasTextNode = false;

            for (XmlElement* child = firstChildElement; child != nullptr; child = child->nextListItem)
            {
                if (child->isTextElement())
                {
                    escapeIllegalXmlChars (outputStream, child->getText(), false);
                    lastWasTextNode = true;
                }
                else
                {
                    if (indentationLevel >= 0 && ! lastWasTextNode)
                        outputStream << newLine;

                    child->writeElementAsText (outputStream,
                                               lastWasTextNode ? 0 : (indentationLevel + (indentationLevel >= 0 ? 2 : 0)), lineWrapLength);
                    lastWasTextNode = false;
                }
            }

            if (indentationLevel >= 0 && ! lastWasTextNode)
            {
                outputStream << newLine;
                writeSpaces (outputStream, indentationLevel);
            }

            outputStream.write ("</", 2);
            outputStream << tagName;
            outputStream.writeByte ('>');
        }
        else
        {
            outputStream.write ("/>", 2);
        }
    }
    else
    {
        escapeIllegalXmlChars (outputStream, getText(), false);
    }
}

String XmlElement::createDocument (const String& dtdToUse,
                                   const bool allOnOneLine,
                                   const bool includeXmlHeader,
                                   const String& encodingType,
                                   const int lineWrapLength) const
{
    MemoryOutputStream mem (2048);
    writeToStream (mem, dtdToUse, allOnOneLine, includeXmlHeader, encodingType, lineWrapLength);

    return mem.toUTF8();
}

void XmlElement::writeToStream (OutputStream& output,
                                const String& dtdToUse,
                                const bool allOnOneLine,
                                const bool includeXmlHeader,
                                const String& encodingType,
                                const int lineWrapLength) const
{
    using namespace XmlOutputFunctions;

    if (includeXmlHeader)
    {
        output << "<?xml version=\"1.0\" encoding=\"" << encodingType << "\"?>";

        if (allOnOneLine)
            output.writeByte (' ');
        else
            output << newLine << newLine;
    }

    if (dtdToUse.isNotEmpty())
    {
        output << dtdToUse;

        if (allOnOneLine)
            output.writeByte (' ');
        else
            output << newLine;
    }

    writeElementAsText (output, allOnOneLine ? -1 : 0, lineWrapLength);

    if (! allOnOneLine)
        output << newLine;
}

bool XmlElement::writeToFile (const File& file,
                              const String& dtdToUse,
                              const String& encodingType,
                              const int lineWrapLength) const
{
    File tempFile (file);

    {
        FileOutputStream out (tempFile);

        if (! out.openedOk())
            return false;

        writeToStream (out, dtdToUse, false, true, encodingType, lineWrapLength);
    }

    return true;
}

//==============================================================================
bool XmlElement::hasTagName (const String& tagNameWanted) const noexcept
{
   #if SGP_DEBUG
    // if debugging, check that the case is actually the same, because
    // valid xml is case-sensitive, and although this lets it pass, it's
    // better not to..
    if (tagName.equalsIgnoreCase (tagNameWanted))
    {
        jassert (tagName == tagNameWanted);
        return true;
    }
    else
    {
        return false;
    }
   #else
    return tagName.equalsIgnoreCase (tagNameWanted);
   #endif
}

XmlElement* XmlElement::getNextElementWithTagName (const String& requiredTagName) const
{
    XmlElement* e = nextListItem;

    while (e != nullptr && ! e->hasTagName (requiredTagName))
        e = e->nextListItem;

    return e;
}

//==============================================================================
int XmlElement::getNumAttributes() const noexcept
{
    return attributes.size();
}

const String& XmlElement::getAttributeName (const int index) const noexcept
{
    const XmlAttributeNode* const att = attributes [index];
    return att != nullptr ? att->name : String::empty;
}

const String& XmlElement::getAttributeValue (const int index) const noexcept
{
    const XmlAttributeNode* const att = attributes [index];
    return att != nullptr ? att->value : String::empty;
}

bool XmlElement::hasAttribute (const String& attributeName) const noexcept
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
        if (att->hasName (attributeName))
            return true;

    return false;
}

//==============================================================================
const String& XmlElement::getStringAttribute (const String& attributeName) const noexcept
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
        if (att->hasName (attributeName))
            return att->value;

    return String::empty;
}

String XmlElement::getStringAttribute (const String& attributeName, const String& defaultReturnValue) const
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
        if (att->hasName (attributeName))
            return att->value;

    return defaultReturnValue;
}

int XmlElement::getIntAttribute (const String& attributeName, const int defaultReturnValue) const
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
        if (att->hasName (attributeName))
            return att->value.getIntValue();

    return defaultReturnValue;
}

double XmlElement::getDoubleAttribute (const String& attributeName, const double defaultReturnValue) const
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
        if (att->hasName (attributeName))
            return att->value.getDoubleValue();

    return defaultReturnValue;
}

bool XmlElement::getBoolAttribute (const String& attributeName, const bool defaultReturnValue) const
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
    {
        if (att->hasName (attributeName))
        {
            sgp_wchar firstChar = att->value[0];

            if (CharacterFunctions::isWhitespace (firstChar))
                firstChar = att->value.trimStart() [0];

            return firstChar == '1'
                || firstChar == 't'
                || firstChar == 'y'
                || firstChar == 'T'
                || firstChar == 'Y';
        }
    }

    return defaultReturnValue;
}

bool XmlElement::compareAttribute (const String& attributeName,
                                   const String& stringToCompareAgainst,
                                   const bool ignoreCase) const noexcept
{
    for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
        if (att->hasName (attributeName))
            return ignoreCase ? att->value.equalsIgnoreCase (stringToCompareAgainst)
                              : att->value == stringToCompareAgainst;

    return false;
}

//==============================================================================
void XmlElement::setAttribute (const String& attributeName, const String& value)
{
    if (attributes == nullptr)
    {
        attributes = new XmlAttributeNode (attributeName, value);
    }
    else
    {
        for (XmlAttributeNode* att = attributes; ; att = att->nextListItem)
        {
            if (att->hasName (attributeName))
            {
                att->value = value;
                break;
            }
            else if (att->nextListItem == nullptr)
            {
                att->nextListItem = new XmlAttributeNode (attributeName, value);
                break;
            }
        }
    }
}

void XmlElement::setAttribute (const String& attributeName, const int number)
{
    setAttribute (attributeName, String (number));
}

void XmlElement::setAttribute (const String& attributeName, const double number)
{
    setAttribute (attributeName, String (number));
}

void XmlElement::removeAttribute (const String& attributeName) noexcept
{
    for (LinkedListPointer<XmlAttributeNode>* att = &attributes;
         att->get() != nullptr;
         att = &(att->get()->nextListItem))
    {
        if (att->get()->hasName (attributeName))
        {
            delete att->removeNext();
            break;
        }
    }
}

void XmlElement::removeAllAttributes() noexcept
{
    attributes.deleteAll();
}

//==============================================================================
int XmlElement::getNumChildElements() const noexcept
{
    return firstChildElement.size();
}

XmlElement* XmlElement::getChildElement (const int index) const noexcept
{
    return firstChildElement [index].get();
}

XmlElement* XmlElement::getChildByName (const String& childName) const noexcept
{
    for (XmlElement* child = firstChildElement; child != nullptr; child = child->nextListItem)
        if (child->hasTagName (childName))
            return child;

    return nullptr;
}

void XmlElement::addChildElement (XmlElement* const newNode) noexcept
{
    if (newNode != nullptr)
        firstChildElement.append (newNode);
}

void XmlElement::insertChildElement (XmlElement* const newNode,
                                     int indexToInsertAt) noexcept
{
    if (newNode != nullptr)
    {
        removeChildElement (newNode, false);
        firstChildElement.insertAtIndex (indexToInsertAt, newNode);
    }
}

XmlElement* XmlElement::createNewChildElement (const String& childTagName)
{
    XmlElement* const newElement = new XmlElement (childTagName);
    addChildElement (newElement);
    return newElement;
}

bool XmlElement::replaceChildElement (XmlElement* const currentChildElement,
                                      XmlElement* const newNode) noexcept
{
    if (newNode != nullptr)
    {
        if (LinkedListPointer<XmlElement>* const p = firstChildElement.findPointerTo (currentChildElement))
        {
            if (currentChildElement != newNode)
                delete p->replaceNext (newNode);

            return true;
        }
    }

    return false;
}

void XmlElement::removeChildElement (XmlElement* const childToRemove,
                                     const bool shouldDeleteTheChild) noexcept
{
    if (childToRemove != nullptr)
    {
        firstChildElement.remove (childToRemove);

        if (shouldDeleteTheChild)
            delete childToRemove;
    }
}

bool XmlElement::isEquivalentTo (const XmlElement* const other,
                                 const bool ignoreOrderOfAttributes) const noexcept
{
    if (this != other)
    {
        if (other == nullptr || tagName != other->tagName)
            return false;

        if (ignoreOrderOfAttributes)
        {
            int totalAtts = 0;

            for (const XmlAttributeNode* att = attributes; att != nullptr; att = att->nextListItem)
            {
                if (! other->compareAttribute (att->name, att->value))
                    return false;

                ++totalAtts;
            }

            if (totalAtts != other->getNumAttributes())
                return false;
        }
        else
        {
            const XmlAttributeNode* thisAtt = attributes;
            const XmlAttributeNode* otherAtt = other->attributes;

            for (;;)
            {
                if (thisAtt == nullptr || otherAtt == nullptr)
                {
                    if (thisAtt == otherAtt) // both 0, so it's a match
                        break;

                    return false;
                }

                if (thisAtt->name != otherAtt->name
                     || thisAtt->value != otherAtt->value)
                {
                    return false;
                }

                thisAtt = thisAtt->nextListItem;
                otherAtt = otherAtt->nextListItem;
            }
        }

        const XmlElement* thisChild = firstChildElement;
        const XmlElement* otherChild = other->firstChildElement;

        for (;;)
        {
            if (thisChild == nullptr || otherChild == nullptr)
            {
                if (thisChild == otherChild) // both 0, so it's a match
                    break;

                return false;
            }

            if (! thisChild->isEquivalentTo (otherChild, ignoreOrderOfAttributes))
                return false;

            thisChild = thisChild->nextListItem;
            otherChild = otherChild->nextListItem;
        }
    }

    return true;
}

void XmlElement::deleteAllChildElements() noexcept
{
    firstChildElement.deleteAll();
}

void XmlElement::deleteAllChildElementsWithTagName (const String& name) noexcept
{
    for (XmlElement* child = firstChildElement; child != nullptr;)
    {
        XmlElement* const nextChild = child->nextListItem;

        if (child->hasTagName (name))
            removeChildElement (child, true);

        child = nextChild;
    }
}

bool XmlElement::containsChildElement (const XmlElement* const possibleChild) const noexcept
{
    return firstChildElement.contains (possibleChild);
}

XmlElement* XmlElement::findParentElementOf (const XmlElement* const elementToLookFor) noexcept
{
    if (this == elementToLookFor || elementToLookFor == nullptr)
        return nullptr;

    for (XmlElement* child = firstChildElement; child != nullptr; child = child->nextListItem)
    {
        if (elementToLookFor == child)
            return this;

        if (XmlElement* const found = child->findParentElementOf (elementToLookFor))
            return found;
    }

    return nullptr;
}

void XmlElement::getChildElementsAsArray (XmlElement** elems) const noexcept
{
    firstChildElement.copyToArray (elems);
}

void XmlElement::reorderChildElements (XmlElement** const elems, const int num) noexcept
{
    XmlElement* e = firstChildElement = elems[0];

    for (int i = 1; i < num; ++i)
    {
        e->nextListItem = elems[i];
        e = e->nextListItem;
    }

    e->nextListItem = nullptr;
}

//==============================================================================
bool XmlElement::isTextElement() const noexcept
{
    return tagName.isEmpty();
}

static const String sgp_xmltextContentAttributeName ("text");

const String& XmlElement::getText() const noexcept
{
    jassert (isTextElement());  // you're trying to get the text from an element that
                                // isn't actually a text element.. If this contains text sub-nodes, you
                                // probably want to use getAllSubText instead.

    return getStringAttribute (sgp_xmltextContentAttributeName);
}

void XmlElement::setText (const String& newText)
{
    if (isTextElement())
        setAttribute (sgp_xmltextContentAttributeName, newText);
    else
        jassertfalse; // you can only change the text in a text element, not a normal one.
}

String XmlElement::getAllSubText() const
{
    if (isTextElement())
        return getText();

    MemoryOutputStream mem (1024);

    for (const XmlElement* child = firstChildElement; child != nullptr; child = child->nextListItem)
        mem << child->getAllSubText();

    return mem.toString();
}

String XmlElement::getChildElementAllSubText (const String& childTagName,
                                              const String& defaultReturnValue) const
{
    if (const XmlElement* const child = getChildByName (childTagName))
        return child->getAllSubText();

    return defaultReturnValue;
}

XmlElement* XmlElement::createTextElement (const String& text)
{
    XmlElement* const e = new XmlElement ((int) 0);
    e->setAttribute (sgp_xmltextContentAttributeName, text);
    return e;
}

void XmlElement::addTextElement (const String& text)
{
    addChildElement (createTextElement (text));
}

void XmlElement::deleteAllTextElements() noexcept
{
    for (XmlElement* child = firstChildElement; child != nullptr;)
    {
        XmlElement* const next = child->nextListItem;

        if (child->isTextElement())
            removeChildElement (child, true);

        child = next;
    }
}
