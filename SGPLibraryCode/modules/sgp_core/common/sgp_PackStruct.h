
// include this file right before the data structures to be 1-aligned
// and add to each structure the PACK_STRUCT define just like this:
// struct mystruct
// {
//	...
// } PACK_STRUCT;
// Always include the sgp_UnPackStruct.h file right after the last type declared
// like this, and do not put any other types with different alignment
// in between!

// byte-align structures
#if SGP_MSVC
#	pragma warning(disable: 4103)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif SGP_GCC
	// Using pragma pack might work with earlier gcc versions already, but
	// it started to be necessary with gcc 4.7 on mingw unless compiled with -mno-ms-bitfields.
	// And I found some hints on the web that older gcc versions on the other hand had sometimes
	// trouble with pragma pack while they worked with __attribute__((packed)).
#	if (__GNUC__ >= 4 ) && (__GNUC_MINOR__ >= 7)
#		pragma pack( push, packing )
#		pragma pack( 1 )
#		define PACK_STRUCT
#	else
#		define PACK_STRUCT	__attribute__((packed))
	#endif
#else
#	error compiler not supported
#endif

