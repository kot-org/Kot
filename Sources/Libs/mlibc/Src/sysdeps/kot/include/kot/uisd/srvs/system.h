#ifndef KOT_SRV_SYSTEM_H
#define KOT_SRV_SYSTEM_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/memory.h>

typedef KResult (*kot_SystemCallbackHandler)(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef struct {
    uint64_t Address;
    uint64_t Width;
    uint64_t Height;
    uint64_t Pitch;
    uint64_t Bpp;
} kot_srv_system_framebuffer_t;

struct kot_srv_system_sdtheader_t {
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
}__attribute__((packed));

struct kot_srv_system_callback_t{
    kot_thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    kot_SystemCallbackHandler Handler;
};

void kot_Srv_System_Initialize();

void kot_Srv_System_Callback(KResult Status, struct kot_srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct kot_srv_system_callback_t* kot_Srv_System_LoadExecutable(uint64_t Priviledge, char* Path, bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_GetFramebuffer(bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_ReadFileInitrd(char* Name,  bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_GetTableInRootSystemDescription(char* Name, bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_GetSystemManagementBIOSTable(bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_BindIRQLine(uint8_t IRQLineNumber, kot_thread_t Target, bool IgnoreMissedEvents, bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_UnbindIRQLine(uint8_t IRQLineNumber, kot_thread_t Target, bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_BindFreeIRQ(kot_thread_t Target, bool IgnoreMissedEvents, bool IsAwait);
struct kot_srv_system_callback_t* kot_Srv_System_UnbindIRQ(uint8_t Vector, kot_thread_t Target, bool IsAwait);

#endif