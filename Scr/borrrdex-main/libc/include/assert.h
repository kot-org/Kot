#include "__config.h"
#include "features.h"

#undef assert

#ifdef NDEBUG
#define assert ((void)0);
#else
#define assert(e) ((e) ? (void)0 : __assert(__func__, __FILE__, __LINE__, #e))
#endif

#if __STDC_VERSION__ >= 201112L && !defined(__cplusplus)
#define static_assert _Static_assert
#endif

__BEGIN_DECLS
_Noreturn void __assert(const char*, const char*, int, const char*) ;
__END_DECLS