#ifndef _KEYHOLE_H
#define _KEYHOLE_H 1

#define KeyholeFlagFullPermissions                          0xFFFFFFFFFFFFFFFF
#define KeyholeFlagPresent                                  (1 << 0)
#define KeyholeFlagCloneable                                (1 << 1)
#define KeyholeFlagEditable                                 (1 << 2)
#define KeyholeFlagOriginal                                 (1 << 3)

/* Specific thread for types */

/* DataTypeThread */
#define KeyholeFlagDataTypeThreadIsUnpauseable              (1 << 4)
#define KeyholeFlagDataTypeThreadIsEventable                (1 << 5)
#define KeyholeFlagDataTypeThreadIsDuplicable               (1 << 6)
#define KeyholeFlagDataTypeThreadIsExecutableWithQueue      (1 << 7)
#define KeyholeFlagDataTypeThreadIsExecutableOneshot        (1 << 8)
#define KeyholeFlagDataTypeThreadAllowChangeTCB             (1 << 9)

/* DataTypeProcess */
#define KeyholeFlagDataTypeProcessMemoryAccessible          (1 << 4)
#define KeyholeFlagDataTypeProcessIsThreadCreateable        (1 << 5)
#define KeyholeFlagDataTypeProcessIsForkable                (1 << 6)

/* DataTypeEvent */
#define KeyholeFlagDataTypeEventIsBindable                  (1 << 4)
#define KeyholeFlagDataTypeEventIsTriggerable               (1 << 5)

/* DataTypeSharedMemory */


#endif