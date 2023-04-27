#ifndef _ABIBITS_PID_T_H
#define _ABIBITS_PID_T_H

#if __WORDSIZE == 64
typedef unsigned long int           pid_t;
#else
typedef unsigned long long int      pid_t;
#endif

#endif // _ABIBITS_PID_T_H

