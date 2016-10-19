

StringPool::StringPool() noexcept   {}
StringPool::~StringPool()           {}

namespace StringPoolHelpers
{
    template <class StringType>
    String::CharPointerType getPooledStringFromArray (Array<String>& strings,
                                                      StringType newString,
                                                      const CriticalSection& lock)
    {
        const ScopedLock sl (lock);
        int start = 0;
        int end = strings.size();

        for (;;)
        {
            if (start >= end)
            {
                jassert (start <= end);
                strings.insert (start, newString);
                return strings.getReference (start).getCharPointer();
            }
            else
            {
                const String& startString = strings.getReference (start);

                if (startString == newString)
                    return startString.getCharPointer();

                const int halfway = (start + end) >> 1;

                if (halfway == start)
                {
                    if (startString.compare (newString) < 0)
                        ++start;

                    strings.insert (start, newString);
                    return strings.getReference (start).getCharPointer();
                }

                const int comp = strings.getReference (halfway).compare (newString);

                if (comp == 0)
                    return strings.getReference (halfway).getCharPointer();
                else if (comp < 0)
                    start = halfway;
                else
                    end = halfway;
            }
        }
    }
}

String::CharPointerType StringPool::getPooledString (const String& s)
{
    if (s.isEmpty())
        return String::empty.getCharPointer();

    return StringPoolHelpers::getPooledStringFromArray (strings, s, lock);
}

String::CharPointerType StringPool::getPooledString (const char* const s)
{
    if (s == nullptr || *s == 0)
        return String::empty.getCharPointer();

    return StringPoolHelpers::getPooledStringFromArray (strings, s, lock);
}

String::CharPointerType StringPool::getPooledString (const wchar_t* const s)
{
    if (s == nullptr || *s == 0)
        return String::empty.getCharPointer();

    return StringPoolHelpers::getPooledStringFromArray (strings, s, lock);
}

int StringPool::size() const noexcept
{
    return strings.size();
}

String::CharPointerType StringPool::operator[] (const int index) const noexcept
{
    return strings [index].getCharPointer();
}
