
#ifndef __SGP_SYSTEMSTATS_HEADER__
#define __SGP_SYSTEMSTATS_HEADER__

#include "../text/sgp_String.h"


//==============================================================================
/**
    Contains methods for finding out about the current hardware and OS configuration.
*/
class SGP_API  SystemStats
{
public:
    //==============================================================================
    /** Returns the current version of SGP,
        See also the SGP_VERSION, SGP_MAJOR_VERSION and SGP_MINOR_VERSION macros.
    */
    static String getSGPVersion();

    //==============================================================================
    /** The set of possible results of the getOperatingSystemType() method. */
    enum OperatingSystemType
    {
        UnknownOS   = 0,

        Linux       = 0x2000,
        Android     = 0x3000,
        iOS         = 0x5000,

        MacOSX_10_4 = 0x1004,
        MacOSX_10_5 = 0x1005,
        MacOSX_10_6 = 0x1006,
        MacOSX_10_7 = 0x1007,
        MacOSX_10_8 = 0x1008,

        Win2000     = 0x4105,
        WinXP       = 0x4106,
        WinVista    = 0x4107,
        Windows7    = 0x4108,
        Windows8    = 0x4109,

        Windows     = 0x4000,   /**< To test whether any version of Windows is running,
                                     you can use the expression ((getOperatingSystemType() & Windows) != 0). */
    };

    /** Returns the type of operating system we're running on.

        @returns one of the values from the OperatingSystemType enum.
        @see getOperatingSystemName
    */
    static OperatingSystemType getOperatingSystemType();

    /** Returns the name of the type of operating system we're running on.

        @returns a string describing the OS type.
        @see getOperatingSystemType
    */
    static String getOperatingSystemName();

    /** Returns true if the OS is 64-bit, or false for a 32-bit OS.
    */
    static bool isOperatingSystem64Bit();

    /** Returns an environment variable.
        If the named value isn't set, this will return the defaultValue string instead.
    */
    static String getEnvironmentVariable (const String& name, const String& defaultValue);

    //==============================================================================
    /** Returns the current user's name, if available.
        @see getFullUserName()
    */
    static String getLogonName();

    /** Returns the current user's full name, if available.
        On some OSes, this may just return the same value as getLogonName().
        @see getLogonName()
    */
    static String getFullUserName();

    /** Returns the host-name of the computer. */
    static String getComputerName();

    /** Returns the language of the user's locale.
        The return value is a 2 or 3 letter language code (ISO 639-1 or ISO 639-2)
    */
    static String getUserLanguage();

    /** Returns the region of the user's locale.
        The return value is a 2 letter country code (ISO 3166-1 alpha-2).
    */
    static String getUserRegion();

    //==============================================================================
    // CPU and memory information..

    /** Returns the number of CPUs. */
    static int getNumCpus() noexcept            { return getCPUFlags().numCpus; }

    /** Returns the approximate CPU speed.
        @returns    the speed in megahertz, e.g. 1500, 2500, 32000 (depending on
                    what year you're reading this...)
    */
    static int getCpuSpeedInMegaherz();

    /** Returns a string to indicate the CPU vendor.
        Might not be known on some systems.
    */
    static String getCpuVendor();

    /** Checks whether Intel MMX instructions are available. */
    static bool hasMMX() noexcept               { return getCPUFlags().hasMMX; }

    /** Checks whether Intel SSE instructions are available. */
    static bool hasSSE() noexcept               { return getCPUFlags().hasSSE; }

    /** Checks whether Intel SSE2 instructions are available. */
    static bool hasSSE2() noexcept              { return getCPUFlags().hasSSE2; }

    /** Checks whether AMD 3DNOW instructions are available. */
    static bool has3DNow() noexcept             { return getCPUFlags().has3DNow; }

    //==============================================================================
    /** Finds out how much RAM is in the machine.
        @returns    the approximate number of megabytes of memory, or zero if
                    something goes wrong when finding out.
    */
    static int getMemorySizeInMegabytes();

    /** Returns the system page-size.
        This is only used by programmers with beards.
    */
    static int getPageSize();


private:
    //==============================================================================
    struct CPUFlags
    {
        CPUFlags();

        int numCpus;
        bool hasMMX : 1;
        bool hasSSE : 1;
        bool hasSSE2 : 1;
        bool has3DNow : 1;
    };

    SystemStats();
    static const CPUFlags& getCPUFlags();

    SGP_DECLARE_NON_COPYABLE (SystemStats)
};


#endif   // __SGP_SYSTEMSTATS_HEADER__
