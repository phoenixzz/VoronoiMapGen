
//==============================================================================
#if SGP_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4514 4996)
#endif

sgp_wchar CharacterFunctions::toUpperCase (const sgp_wchar character) noexcept
{
    return towupper ((wchar_t) character);
}

sgp_wchar CharacterFunctions::toLowerCase (const sgp_wchar character) noexcept
{
    return towlower ((wchar_t) character);
}

bool CharacterFunctions::isUpperCase (const sgp_wchar character) noexcept
{
   #if SGP_WINDOWS
    return iswupper ((wchar_t) character) != 0;
   #else
    return toLowerCase (character) != character;
   #endif
}

bool CharacterFunctions::isLowerCase (const sgp_wchar character) noexcept
{
   #if SGP_WINDOWS
    return iswlower ((wchar_t) character) != 0;
   #else
    return toUpperCase (character) != character;
   #endif
}

#if SGP_MSVC
 #pragma warning (pop)
#endif

//==============================================================================
bool CharacterFunctions::isWhitespace (const char character) noexcept
{
    return character == ' ' || (character <= 13 && character >= 9);
}

bool CharacterFunctions::isWhitespace (const sgp_wchar character) noexcept
{
    return iswspace ((wchar_t) character) != 0;
}

bool CharacterFunctions::isDigit (const char character) noexcept
{
    return (character >= '0' && character <= '9');
}

bool CharacterFunctions::isDigit (const sgp_wchar character) noexcept
{
    return iswdigit ((wchar_t) character) != 0;
}

bool CharacterFunctions::isLetter (const char character) noexcept
{
    return (character >= 'a' && character <= 'z')
        || (character >= 'A' && character <= 'Z');
}

bool CharacterFunctions::isLetter (const sgp_wchar character) noexcept
{
    return iswalpha ((wchar_t) character) != 0;
}

bool CharacterFunctions::isLetterOrDigit (const char character) noexcept
{
    return (character >= 'a' && character <= 'z')
        || (character >= 'A' && character <= 'Z')
        || (character >= '0' && character <= '9');
}

bool CharacterFunctions::isLetterOrDigit (const sgp_wchar character) noexcept
{
    return iswalnum ((wchar_t) character) != 0;
}

int CharacterFunctions::getHexDigitValue (const sgp_wchar digit) noexcept
{
    unsigned int d = (unsigned int) digit - '0';
    if (d < (unsigned int) 10)
        return (int) d;

    d += (unsigned int) ('0' - 'a');
    if (d < (unsigned int) 6)
        return (int) d + 10;

    d += (unsigned int) ('a' - 'A');
    if (d < (unsigned int) 6)
        return (int) d + 10;

    return -1;
}

double CharacterFunctions::mulexp10 (const double value, int exponent) noexcept
{
    if (exponent == 0)
        return value;

    if (value == 0)
        return 0;

    const bool negative = (exponent < 0);
    if (negative)
        exponent = -exponent;

    double result = 1.0, power = 10.0;
    for (int bit = 1; exponent != 0; bit <<= 1)
    {
        if ((exponent & bit) != 0)
        {
            exponent ^= bit;
            result *= power;
            if (exponent == 0)
                break;
        }
        power *= power;
    }

    return negative ? (value / result) : (value * result);
}

template <typename DestCharPointerType, typename SrcCharPointerType>
int CharacterFunctions::copyWithDestByteLimit (DestCharPointerType& dest, SrcCharPointerType src, int maxBytes) noexcept
{
    typename DestCharPointerType::CharType const* const startAddress = dest.getAddress();
    maxBytes -= sizeof (typename DestCharPointerType::CharType); // (allow for a terminating null)

    for (;;)
    {
        const sgp_wchar c = src.getAndAdvance();
        const int bytesNeeded = (int) DestCharPointerType::getBytesRequiredFor (c);

        maxBytes -= bytesNeeded;
        if (c == 0 || maxBytes < 0)
            break;

        dest.write (c);
    }

    dest.writeNull();

    return (int) ((size_t) getAddressDifference (dest.getAddress(), startAddress) + sizeof (typename DestCharPointerType::CharType));
}
