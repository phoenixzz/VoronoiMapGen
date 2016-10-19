

// include this file to switch back to default alignment
// file belongs to sgp_PackStruct.h, see there for more info

// Default alignment
#if SGP_MSVC
#	pragma pack( pop, packing )
#elif SGP_GCC
#   if (__GNUC__ >= 4 ) && (__GNUC_MINOR__ >= 7)
#	    pragma pack( pop, packing )
#   endif
#endif

#undef PACK_STRUCT

