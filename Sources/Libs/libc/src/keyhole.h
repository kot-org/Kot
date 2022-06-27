#ifndef _KEYHOLE_H
#define _KEYHOLE_H 1

#define KeyholeFlagFullPermissions                          0xFFFFFFFFFFFFFFFF
#define KeyholeFlagPresent                                  0x0
#define KeyholeFlagCloneable                                0x1
#define KeyholeFlagEditable                                 0x2

/* Specific thread for types */

/* DataTypeThread */
#define KeyholeFlagDataTypeThreadMemoryAccessible           0x3
#define KeyholeFlagDataTypeThreadIsExecutableAsCIP          0x4
#define KeyholeFlagDataTypeThreadIsExitable                 0x5 
#define KeyholeFlagDataTypeThreadIsPauseable                0x6 
#define KeyholeFlagDataTypeThreadIsUnpauseable              0x7
#define KeyholeFlagDataTypeThreadIsEventable                0x8
#define KeyholeFlagDataTypeThreadIsDuplicable               0x9
#define KeyholeFlagDataTypeThreadIsExecutable               0x10

/* DataTypeProcess */
#define KeyholeFlagDataTypeProcessMemoryAccessible          0x3
#define KeyholeFlagDataTypeProcessIsThreadCreateable        0x4

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