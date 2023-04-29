#ifndef _ABIBITS_MODE_T_H
#define _ABIBITS_MODE_T_H

#if __WORDSIZE == 64
typedef unsigned long int           mode_t;
#else
typedef unsigned long long int      mode_t;
#endif

#endif // _ABIBITS_MODE_T_H