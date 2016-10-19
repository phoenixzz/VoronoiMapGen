

Random::Random (const int64 seedValue) noexcept
    : seed (seedValue)
{
}

Random::Random()
    : seed (1)
{
    setSeedRandomly();
}

Random::~Random() noexcept
{
}

void Random::setSeed (const int64 newSeed) noexcept
{
    seed = newSeed;
}

void Random::combineSeed (const int64 seedValue) noexcept
{
    seed ^= nextInt64() ^ seedValue;
}

void Random::setSeedRandomly()
{
    static int64 globalSeed = 0;

    combineSeed (globalSeed ^ (int64) (pointer_sized_int) this);
    combineSeed (Time::getMillisecondCounter());
    combineSeed (Time::getHighResolutionTicks());
    combineSeed (Time::getHighResolutionTicksPerSecond());
    combineSeed (Time::currentTimeMillis());
    globalSeed ^= seed;
}

Random& Random::getSystemRandom() noexcept
{
    static Random sysRand;
    return sysRand;
}

//==============================================================================
int Random::nextInt() noexcept
{
    seed = (seed * literal64bit (0x5deece66d) + 11) & literal64bit (0xffffffffffff);

    return (int) (seed >> 16);
}

int Random::nextInt (const int maxValue) noexcept
{
    jassert (maxValue > 0);
    return (int) ((((unsigned int) nextInt()) * (uint64) maxValue) >> 32);
}

int64 Random::nextInt64() noexcept
{
    return (((int64) nextInt()) << 32) | (int64) (uint64) (uint32) nextInt();
}

bool Random::nextBool() noexcept
{
    return (nextInt() & 0x40000000) != 0;
}

float Random::nextFloat() noexcept
{
    return static_cast <uint32> (nextInt()) / (float) 0xffffffff;
}

double Random::nextDouble() noexcept
{
    return static_cast <uint32> (nextInt()) / (double) 0xffffffff;
}
