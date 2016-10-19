
const SystemStats::CPUFlags& SystemStats::getCPUFlags()
{
    static CPUFlags cpuFlags;
    return cpuFlags;
}

String SystemStats::getSGPVersion()
{
    // Some basic tests, to keep an eye on things and make sure these types work ok
    // on all platforms. Let me know if any of these assertions fail on your system!
    static_jassert (sizeof (pointer_sized_int) == sizeof (void*));
    static_jassert (sizeof (int8) == 1);
    static_jassert (sizeof (uint8) == 1);
    static_jassert (sizeof (int16) == 2);
    static_jassert (sizeof (uint16) == 2);
    static_jassert (sizeof (int32) == 4);
    static_jassert (sizeof (uint32) == 4);
    static_jassert (sizeof (int64) == 8);
    static_jassert (sizeof (uint64) == 8);

    return "SGP Engine v" SGP_STRINGIFY(SGP_MAJOR_VERSION)
					  "." SGP_STRINGIFY(SGP_MINOR_VERSION)
                      "." SGP_STRINGIFY(SGP_BUILDNUMBER);
}
