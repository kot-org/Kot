#include "EfiMemory.h"

const char* EFI_MEMORY_TYPE_STRINGS[] {
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiAcpiReclaimMemory",
    "EfiAcpiMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode"
};