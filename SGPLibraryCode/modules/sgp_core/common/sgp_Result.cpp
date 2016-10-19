

Result::Result (const String& message) noexcept
    : errorMessage (message)
{
}

Result::Result (const Result& other)
    : errorMessage (other.errorMessage)
{
}

Result& Result::operator= (const Result& other)
{
    errorMessage = other.errorMessage;
    return *this;
}



bool Result::operator== (const Result& other) const noexcept
{
    return errorMessage == other.errorMessage;
}

bool Result::operator!= (const Result& other) const noexcept
{
    return errorMessage != other.errorMessage;
}

Result Result::ok() noexcept
{
    return Result (String::empty);
}

Result Result::fail (const String& errorMessage) noexcept
{
    return Result (errorMessage.isEmpty() ? "Unknown Error" : errorMessage);
}

const String& Result::getErrorMessage() const noexcept
{
    return errorMessage;
}

bool Result::wasOk() const noexcept         { return errorMessage.isEmpty(); }
Result::operator bool() const noexcept      { return errorMessage.isEmpty(); }
bool Result::failed() const noexcept        { return errorMessage.isNotEmpty(); }
bool Result::operator!() const noexcept     { return errorMessage.isNotEmpty(); }
