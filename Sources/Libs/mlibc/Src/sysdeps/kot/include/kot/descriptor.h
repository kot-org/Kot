#ifndef KOT_DESCRIPTOR_H
#define KOT_DESCRIPTOR_H 1

#include <stdlib.h>
#include <string.h>
#include <kot/sys.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/memory.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define KOT_DESCRIPTOR_TABLE_SIZE                   (0x2000)
#define KOT_DESCRIPTOR_TABLE_COUNT                  (KOT_DESCRIPTOR_TABLE_SIZE / sizeof(void*))
#define KOT_DESCRIPTOR_MAP_DATA_INDEX               (0)
#define KOT_DESCRIPTOR_COUNT_INDEX                  (KOT_DESCRIPTOR_MAP_DATA_INDEX + 1)
#define KOT_DESCRIPTOR_FIRST_USABLE_INDEX           (KOT_DESCRIPTOR_COUNT_INDEX + 1)
#define KOT_DESCRIPTOR_FIRST_ALLOCATABLE_INDEX      (KOT_DESCRIPTOR_FIRST_USABLE_INDEX + 3) // Reserved 0, 1, 2 for stdin, stdout, stderr

typedef uint64_t descriptor_type_t;
#define KOT_DESCRIPTOR_TYPE_FILE                    0x0
#define KOT_DESCRIPTOR_TYPE_DIRECTORY               0x1

typedef struct{
    descriptor_type_t Type;
    void* Data;
    size64_t Size;
}kot_descriptor_t;

int kot_InitializeDescriptorSaver();
int64_t kot_SaveDescriptor(kot_descriptor_t* Descriptor);
kot_descriptor_t* kot_GetDescriptor(int64_t Index);
KResult kot_ModifyDescriptor(int64_t Index, kot_descriptor_t* Descriptor);
KResult kot_FreeDescriptor(int64_t Index);
size64_t kot_GetDescriptorCount();


#if defined(__cplusplus)
}
#endif

#endif
