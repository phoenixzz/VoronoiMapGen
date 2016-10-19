

String String::fromCFString (CFStringRef cfString)
{
    if (cfString == 0)
        return String::empty;

    CFRange range = { 0, CFStringGetLength (cfString) };
    HeapBlock <UniChar> u ((size_t) range.length + 1);
    CFStringGetCharacters (cfString, range, u);
    u[range.length] = 0;

    return String (CharPointer_UTF16 ((const CharPointer_UTF16::CharType*) u.getData()));
}

CFStringRef String::toCFString() const
{
    CharPointer_UTF16 utf16 (toUTF16());
    return CFStringCreateWithCharacters (kCFAllocatorDefault, (const UniChar*) utf16.getAddress(), (CFIndex) utf16.length());
}

String String::convertToPrecomposedUnicode() const
{
   #if SGP_IOS
    SGP_AUTORELEASEPOOL
    return nsStringToSGP ([SGPStringToNS (*this) precomposedStringWithCanonicalMapping]);
   #else
    UnicodeMapping map;

    map.unicodeEncoding = CreateTextEncoding (kTextEncodingUnicodeDefault,
                                              kUnicodeNoSubset,
                                              kTextEncodingDefaultFormat);

    map.otherEncoding = CreateTextEncoding (kTextEncodingUnicodeDefault,
                                            kUnicodeCanonicalCompVariant,
                                            kTextEncodingDefaultFormat);

    map.mappingVersion = kUnicodeUseLatestMapping;

    UnicodeToTextInfo conversionInfo = 0;
    String result;

    if (CreateUnicodeToTextInfo (&map, &conversionInfo) == noErr)
    {
        const size_t bytesNeeded = CharPointer_UTF16::getBytesRequiredFor (getCharPointer());

        HeapBlock <char> tempOut;
        tempOut.calloc (bytesNeeded + 4);

        ByteCount bytesRead = 0;
        ByteCount outputBufferSize = 0;

        if (ConvertFromUnicodeToText (conversionInfo,
                                      bytesNeeded, (ConstUniCharArrayPtr) toUTF16().getAddress(),
                                      kUnicodeDefaultDirectionMask,
                                      0, 0, 0, 0,
                                      bytesNeeded, &bytesRead,
                                      &outputBufferSize, tempOut) == noErr)
        {
            result = String (CharPointer_UTF16 ((CharPointer_UTF16::CharType*) tempOut.getData()));
        }

        DisposeUnicodeToTextInfo (&conversionInfo);
    }

    return result;
   #endif
}
