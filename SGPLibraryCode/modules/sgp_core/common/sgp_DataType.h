
#ifndef __SPG_DATATYPE_HEADER__
#define __SPG_DATATYPE_HEADER__

//==============================================================================
/*
    This file sets up some handy mathematical typdefs.
*/

//==============================================================================
// Definitions for the int8, int16, int32, int64 and pointer_sized_int types.


/** A platform-independent 8-bit signed integer type. */
typedef signed char                 int8;
/** A platform-independent 8-bit unsigned integer type. */
typedef unsigned char               uint8;
/** A platform-independent 16-bit signed integer type. */
typedef signed short                int16;
/** A platform-independent 16-bit unsigned integer type. */
typedef unsigned short              uint16;
/** A platform-independent 32-bit signed integer type. */
typedef signed int                  int32;
/** A platform-independent 32-bit unsigned integer type. */
typedef unsigned int                uint32;

#if SGP_MSVC
  /** A platform-independent 64-bit integer type. */
  typedef __int64                   int64;
  /** A platform-independent 64-bit unsigned integer type. */
  typedef unsigned __int64          uint64;
  /** A platform-independent macro for writing 64-bit literals, needed because
      different compilers have different syntaxes for this.

      E.g. writing literal64bit (0x1000000000) will translate to 0x1000000000LL for
      GCC, or 0x1000000000 for MSVC.
  */
  #define literal64bit(longLiteral)     ((__int64) longLiteral)
#else
  /** A platform-independent 64-bit integer type. */
  typedef long long                 int64;
  /** A platform-independent 64-bit unsigned integer type. */
  typedef unsigned long long        uint64;
  /** A platform-independent macro for writing 64-bit literals, needed because
      different compilers have different syntaxes for this.

      E.g. writing literal64bit (0x1000000000) will translate to 0x1000000000LL for
      GCC, or 0x1000000000 for MSVC.
  */
  #define literal64bit(longLiteral)     (longLiteral##LL)
#endif


#if SGP_64BIT
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef int64                     pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef uint64                    pointer_sized_uint;
#elif SGP_MSVC
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef _W64 int                  pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef _W64 unsigned int         pointer_sized_uint;
#else
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef int                       pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef unsigned int              pointer_sized_uint;
#endif

#if SGP_MSVC
  typedef pointer_sized_int ssize_t;
#endif


//==============================================================================
/** A predefined value for Pi, at double-precision.

    @see float_Pi
*/
const double  double_Pi  = 3.1415926535897932384626433832795;
const double  double_2pi = 6.283185307179586476925286766558;

const float pi_over_180 = 0.01745329251994329577f;
const float pi_over_360 = 0.00872664625997164788f;

/** A predefined value for Pi, at sngle-precision.

    @see double_Pi
*/
const float   float_Pi   = 3.14159265358979323846f;
const float   float_2pi  = 6.28318530717958647692f;

//==============================================================================
// Some indispensible min/max functions

/** Returns the larger of two values. */
template <typename Type>
inline Type jmax (const Type a, const Type b)                                               { return (a < b) ? b : a; }

/** Returns the larger of three values. */
template <typename Type>
inline Type jmax (const Type a, const Type b, const Type c)                                 { return (a < b) ? ((b < c) ? c : b) : ((a < c) ? c : a); }

/** Returns the larger of four values. */
template <typename Type>
inline Type jmax (const Type a, const Type b, const Type c, const Type d)                   { return jmax (a, jmax (b, c, d)); }

/** Returns the smaller of two values. */
template <typename Type>
inline Type jmin (const Type a, const Type b)                                               { return (b < a) ? b : a; }

/** Returns the smaller of three values. */
template <typename Type>
inline Type jmin (const Type a, const Type b, const Type c)                                 { return (b < a) ? ((c < b) ? c : b) : ((c < a) ? c : a); }

/** Returns the smaller of four values. */
template <typename Type>
inline Type jmin (const Type a, const Type b, const Type c, const Type d)                   { return jmin (a, jmin (b, c, d)); }

/** Scans an array of values, returning the minimum value that it contains. */
template <typename Type>
const Type findMinimum (const Type* data, int numValues)
{
    if (numValues <= 0)
        return Type();

    Type result (*data++);

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        const Type& v = *data++;
        if (v < result)  result = v;
    }

    return result;
}

// Find the Minimum power of two Number larger than value
// If value itself is power of 2, return itself
// If value itself is less or equal 1, return itself
template <typename Type>
const Type findMinimumPower2Number (const Type value)
{
	Type rt = value;
    if (rt <= 1) return rt;  
    if (value & (value-1))   
    {  
        Type result = 1;  
        while(rt)  
        {
            rt >>= 1;  
            result <<= 1;  
        }  
        return result;
    }  
    else  
    {  
        return value;  
    }     
}

/** Scans an array of values, returning the maximum value that it contains. */
template <typename Type>
const Type findMaximum (const Type* values, int numValues)
{
    if (numValues <= 0)
        return Type();

    Type result (*values++);

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        const Type& v = *values++;
        if (result < v)  result = v;
    }

    return result;
}

/** Scans an array of values, returning the minimum and maximum values that it contains. */
template <typename Type>
void findMinAndMax (const Type* values, int numValues, Type& lowest, Type& highest)
{
    if (numValues <= 0)
    {
        lowest = Type();
        highest = Type();
    }
    else
    {
        Type mn (*values++);
        Type mx (mn);

        while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
        {
            const Type& v = *values++;

            if (mx < v)  mx = v;
            if (v < mn)  mn = v;
        }

        lowest = mn;
        highest = mx;
    }
}

//==============================================================================
/** Constrains a value to keep it within a given range.

    This will check that the specified value lies between the lower and upper bounds
    specified, and if not, will return the nearest value that would be in-range. Effectively,
    it's like calling jmax (lowerLimit, jmin (upperLimit, value)).

    Note that it expects that lowerLimit <= upperLimit. If this isn't true,
    the results will be unpredictable.

    @param lowerLimit           the minimum value to return
    @param upperLimit           the maximum value to return
    @param valueToConstrain     the value to try to return
    @returns    the closest value to valueToConstrain which lies between lowerLimit
                and upperLimit (inclusive)
    @see jlimit0To, jmin, jmax
*/
template <typename Type>
inline Type jlimit (const Type lowerLimit,
                    const Type upperLimit,
                    const Type valueToConstrain) noexcept
{
    jassert (lowerLimit <= upperLimit); // if these are in the wrong order, results are unpredictable..

    return (valueToConstrain < lowerLimit) ? lowerLimit
                                           : ((upperLimit < valueToConstrain) ? upperLimit
                                                                              : valueToConstrain);
}

/** Returns true if a value is at least zero, and also below a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest < upperLimit
    @endcode
*/
template <typename Type>
inline bool isPositiveAndBelow (Type valueToTest, Type upperLimit) noexcept
{
    jassert (Type() <= upperLimit); // makes no sense to call this if the upper limit is itself below zero..
    return Type() <= valueToTest && valueToTest < upperLimit;
}

template <>
inline bool isPositiveAndBelow (const int valueToTest, const int upperLimit) noexcept
{
    jassert (upperLimit >= 0); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast <unsigned int> (valueToTest) < static_cast <unsigned int> (upperLimit);
}

//==============================================================================
/** 64-bit abs function. */
inline int64 abs64 (const int64 n) noexcept
{
    return (n >= 0) ? n : -n;
}

//==============================================================================
// BKDR Hash Function
inline uint64 WChar_tStringHash( const wchar_t* p_string, int length )
{
	uint64 seed = 131;
    uint64 hash = 0;
    for(int i = 0; i < length; i++ )
    {
		hash = hash * seed + p_string[i];
    }

    return (hash & 0x7FFFFFFF);
}



//==============================================================================
/** This namespace contains a few template classes for helping work out class type variations.
*/
namespace TypeHelpers
{
   #if SGP_VC8_OR_EARLIER
    #define PARAMETER_TYPE(type) const type&
   #else
    /** The ParameterType struct is used to find the best type to use when passing some kind
        of object as a parameter.

        Of course, this is only likely to be useful in certain esoteric template situations.

        Because "typename TypeHelpers::ParameterType<SomeClass>::type" is a bit of a mouthful, there's
        a PARAMETER_TYPE(SomeClass) macro that you can use to get the same effect.

        E.g. "myFunction (PARAMETER_TYPE (int), PARAMETER_TYPE (MyObject))"
        would evaluate to "myfunction (int, const MyObject&)", keeping any primitive types as
        pass-by-value, but passing objects as a const reference, to avoid copying.
    */
    template <typename Type> struct ParameterType                   { typedef const Type& type; };


    template <typename Type> struct ParameterType <Type&>           { typedef Type& type; };
    template <typename Type> struct ParameterType <Type*>           { typedef Type* type; };
    template <>              struct ParameterType <char>            { typedef char type; };
    template <>              struct ParameterType <unsigned char>   { typedef unsigned char type; };
    template <>              struct ParameterType <short>           { typedef short type; };
    template <>              struct ParameterType <unsigned short>  { typedef unsigned short type; };
    template <>              struct ParameterType <int>             { typedef int type; };
    template <>              struct ParameterType <unsigned int>    { typedef unsigned int type; };
    template <>              struct ParameterType <long>            { typedef long type; };
    template <>              struct ParameterType <unsigned long>   { typedef unsigned long type; };
    template <>              struct ParameterType <int64>           { typedef int64 type; };
    template <>              struct ParameterType <uint64>          { typedef uint64 type; };
    template <>              struct ParameterType <bool>            { typedef bool type; };
    template <>              struct ParameterType <float>           { typedef float type; };
    template <>              struct ParameterType <double>          { typedef double type; };


    /** A helpful macro to simplify the use of the ParameterType template.
        @see ParameterType
    */
    #define PARAMETER_TYPE(a)    typename TypeHelpers::ParameterType<a>::type
   #endif


    /** These templates are designed to take a type, and if it's a double, they return a double
        type; for anything else, they return a float type.
    */
    template <typename Type> struct SmallestFloatType             { typedef float  type; };
    template <>              struct SmallestFloatType <double>    { typedef double type; };
}

#endif   // __SPG_DATATYPE_HEADER__
