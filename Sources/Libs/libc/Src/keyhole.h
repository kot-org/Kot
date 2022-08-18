#ifndef _KEYHOLE_H
#define _KEYHOLE_H 1

#define KeyholeFlagFullPermissions                          0xFFFFFFFFFFFFFFFF
#define KeyholeFlagPresent                                  0x0
#define KeyholeFlagCloneable                                0x1
#define KeyholeFlagEditable                                 0x2
#define KeyholeFlagOriginal                                 0x3

/* Specific thread for types */

/* DataTypethread */
#define KeyholeFlagDataTypethreadMemoryAccessible           0x4
#define KeyholeFlagDataTypethreadIsClosaable                0x5 
#define KeyholeFlagDataTypethreadIsExitable                 0x6 
#define KeyholeFlagDataTypethreadIsPauseable                0x7 
#define KeyholeFlagDataTypethreadIsUnpauseable              0x8
#define KeyholeFlagDataTypethreadIsEventable                0x9
#define KeyholeFlagDataTypethreadIsDuplicable               0x10
#define KeyholeFlagDataTypethreadIsExecutableWithQueue      0x11
#define KeyholeFlagDataTypethreadIsExecutableOneshot        0x12

/* DataTypeProcess */
#define KeyholeFlagDataTypeProcessMemoryAccessible          0x4
#define KeyholeFlagDataTypeProcessIsthreadCreateable        0x5

/* DataTypeEvent */
/* DataTypeSharedMemory */


static inline bool Keyhole_GetFlag(uint64_t Flags, uint8_t Flag){
    return (Flags >> Flag) & 0x1;
}

static inline void Keyhole_SetFlag(uint64_t* Flags, uint8_t Flag, bool Status){
    if(Status){
        *Flags |= Flag;        
    }else{
        *Flags &= ~Flag;
    }

}

#endif