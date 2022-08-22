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
#define KeyholeFlagDataTypethreadIsUnpauseable              0x5
#define KeyholeFlagDataTypethreadIsEventable                0x6
#define KeyholeFlagDataTypethreadIsDuplicable               0x7
#define KeyholeFlagDataTypethreadIsExecutableWithQueue      0x8
#define KeyholeFlagDataTypethreadIsExecutableOneshot        0x9

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