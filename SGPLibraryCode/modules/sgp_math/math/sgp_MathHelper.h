#ifndef __SPG_MATHHELPER_HEADER__
#define __SPG_MATHHELPER_HEADER__

//==============================================================================
/*
    This file sets up some handy mathematical typdefs and functions.
*/

#define SGP_FRONT    0
#define SGP_BACK     1
#define SGP_PLANAR   2
#define SGP_CLIPPED  3
#define SGP_CULLED   4
#define SGP_VISIBLE  5

//==============================================================================
/** Handy function to swap two values. */
template <typename Type>
inline void swapVariables (Type& variable1, Type& variable2)
{
    std::swap (variable1, variable2);
}

//==============================================================================
// Some useful maths functions that aren't always present with all compilers and build settings.

/** Using sgp_hypot is easier than dealing with the different types of hypot function
    that are provided by the various platforms and compilers. */
template <typename Type>
inline Type sgp_hypot (Type a, Type b) noexcept
{
   #if SGP_MSVC
    return static_cast <Type> (_hypot (a, b));
   #else
    return static_cast <Type> (hypot (a, b));
   #endif
}



//==============================================================================
/** The isfinite() method seems to vary between platforms, so this is a
    platform-independent function for it.
*/
template <typename FloatingPointType>
inline bool sgp_isfinite (FloatingPointType value)
{
   #if SGP_WINDOWS
    return _finite (value);
   #elif SGP_ANDROID
    return isfinite (value);
   #else
    return std::isfinite (value);
   #endif
}

//==============================================================================
#if SGP_MSVC
 #pragma optimize ("t", off)
 #ifndef __INTEL_COMPILER
  #pragma float_control (precise, on, push)
 #endif
#endif

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
*/
template <typename FloatType>
inline int roundToInt (const FloatType value) noexcept
{
  #ifdef __INTEL_COMPILER
   #pragma float_control (precise, on, push)
  #endif

    union { int asInt[2]; double asDouble; } n;
    n.asDouble = ((double) value) + 6755399441055744.0;

   #if SGP_BIG_ENDIAN
    return n.asInt [1];
   #else
    return n.asInt [0];
   #endif

  #ifdef __INTEL_COMPILER
   #pragma float_control (pop)
  #endif
}

#if SGP_MSVC
 #ifndef __INTEL_COMPILER
  #pragma float_control (pop)
 #endif
 #pragma optimize ("", on)  // resets optimisations to the project defaults
#endif

/** Fast floating-point-to-integer conversion.

    This is a slightly slower and slightly more accurate version of roundDoubleToInt(). It works
    fine for values above zero, but negative numbers are rounded the wrong way.
*/
inline int roundToIntAccurate (const double value) noexcept
{
    return roundToInt (value + 1.5e-8);
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a double to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently. For a more accurate conversion,
    see roundDoubleToIntAccurate().
*/
inline int roundDoubleToInt (const double value) noexcept
{
    return roundToInt (value);
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
*/
inline int roundFloatToInt (const float value) noexcept
{
    return roundToInt (value);
}

//==============================================================================
/** Returns true if the specified integer is a power-of-two.
*/
template <typename IntegerType>
bool isPowerOfTwo (IntegerType value)
{
   return (value & (value - 1)) == 0;
}

/** Returns the smallest power-of-two which is equal to or greater than the given integer.
*/
inline int nextPowerOfTwo (int n) noexcept
{
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return n + 1;
}

/** Performs a modulo operation, but can cope with the dividend being negative.
    The divisor must be greater than zero.
*/
template <typename IntegerType>
int negativeAwareModulo (IntegerType dividend, const IntegerType divisor) noexcept
{
    jassert (divisor > 0);
    dividend %= divisor;
    return (dividend < 0) ? (dividend + divisor) : dividend;
}

/**
   Float compare Function
*/
inline int FloatCmp(float a, float b, float e = 0.0001f)
{
	if( std::fabs( a-b ) < e )
		return 0;
	if( a < b )
		return -1;
	return 1;
}

/**
   Double compare Function
*/
inline int DoubleCmp(double a, double b, double e = 0.0001)
{
	if( std::fabs( a-b ) < e )
		return 0;
	if( a < b )
		return -1;
	return 1;
}

#endif