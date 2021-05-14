#pragma once

// Page 15 - UEFI Specs 2.8b
#define EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION          10 // EfiLoaderCode,          EfiLoaderData
#define EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11 // EfiBootServicesCode,    EfiBootServicesData
#define EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER       12 // EfiRuntimeServicesCode, EfiRuntimeServicesData

// Page 15 - UEFI Specs 2.8b
// I didn't include other hardware codes, like ARM for example
#define EFI_IMAGE_MACHINE_IA32   0x014C
#define EFI_IMAGE_MACHINE_IA64   0x0200
#define EFI_IMAGE_MACHINE_X64    0x8664

// REVISIONS
#define EFI_FILE_PROTOCOL_REVISION          0x00010000
#define EFI_FILE_PROTOCOL_REVISION2         0x00020000
#define EFI_FILE_PROTOCOL_LATEST_REVISION   EFI_FILE_PROTOCOL_REVISION2
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION    0x00010000
// Page 825 - UEFI Specs 2.8b
#define EFI_USBFN_IO_PROTOCOL_REVISION      0x00010001

// Page 509 - UEFI Specs 2.8b
// Open Modes
#define EFI_FILE_MODE_READ      0x0000000000000001
#define EFI_FILE_MODE_WRITE     0x0000000000000002
#define EFI_FILE_MODE_CREATE    0x8000000000000000
// File Attributes
#define EFI_FILE_READ_ONLY      0x0000000000000001
#define EFI_FILE_HIDDEN         0x0000000000000002
#define EFI_FILE_SYSTEM         0x0000000000000004
#define EFI_FILE_RESERVED       0x0000000000000008
#define EFI_FILE_DIRECTORY      0x0000000000000010
#define EFI_FILE_ARCHIVE        0x0000000000000020
#define EFI_FILE_VALID_ATTR     0x0000000000000037

// Page 167 - UEFI Specs 2.8b
// Memory Attribute Definitions
#define EFI_MEMORY_UC              0x0000000000000001
#define EFI_MEMORY_WC              0x0000000000000002
#define EFI_MEMORY_WT              0x0000000000000004
#define EFI_MEMORY_WB              0x0000000000000008
#define EFI_MEMORY_UCE             0x0000000000000010
#define EFI_MEMORY_WP              0x0000000000001000
#define EFI_MEMORY_RP              0x0000000000002000
#define EFI_MEMORY_XP              0x0000000000004000
#define EFI_MEMORY_NV              0x0000000000008000
#define EFI_MEMORY_MORE_RELIABLE   0x0000000000010000
#define EFI_MEMORY_RO              0x0000000000020000
#define EFI_MEMORY_SP              0x0000000000040000
#define EFI_MEMORY_CPU_CRYPTO      0x0000000000080000
#define EFI_MEMORY_RUNTIME         0x8000000000000000

// Page 451 - UEFI Specs 2.8b
#define EFI_BLACK                               0x00
#define EFI_BLUE                                0x01
#define EFI_GREEN                               0x02
#define EFI_CYAN                                0x03
#define EFI_RED                                 0x04
#define EFI_MAGENTA                             0x05
#define EFI_BROWN                               0x06
#define EFI_LIGHTGRAY                           0x07
#define EFI_DARKGRAY                            0x08
#define EFI_LIGHTBLUE                           0x09
#define EFI_LIGHTGREEN                          0x0A
#define EFI_LIGHTCYAN                           0x0B
#define EFI_LIGHTRED                            0x0C
#define EFI_LIGHTMAGENTA                        0x0D
#define EFI_YELLOW                              0x0E
#define EFI_WHITE                               0x0F
                                               
#define EFI_BACKGROUND_BLACK                    0x00
#define EFI_BACKGROUND_BLUE                     0x10
#define EFI_BACKGROUND_GREEN                    0x20
#define EFI_BACKGROUND_CYAN                     0x30
#define EFI_BACKGROUND_RED                      0x40
#define EFI_BACKGROUND_MAGENTA                  0x50
#define EFI_BACKGROUND_BROWN                    0x60
#define EFI_BACKGROUND_LIGHTGRAY                0x70

#define EFI_TEXT_ATTR(Forground, Background) ((Forground) | ((Background) << 4))

// Page 190 - UEFI Specs 2.8b
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL    0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL          0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL         0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER   0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER             0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE             0x00000020

// Page 775 - UEFI Specs 2.8b
#define EFI_USB_SPEED_FULL                  0x0000
#define EFI_USB_SPEED_LOW                   0x0001
#define EFI_USB_SPEED_HIGH                  0x0002
#define EFI_USB_SPEED_SUPER                 0x0003

// Page 776 - UEFI Specs 2.8b
#define EFI_USB_HC_RESET_GLOBAL             0x0001
#define EFI_USB_HC_RESET_HOST_CONTROLLER    0x0002
#define EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG  0x0004
#define EFI_USB_HC_RESET_HOST_WITH_DEBUG    0x0008

// Page 806 - UEFI Specs 2.8b
#define EFI_USB_NOERROR                     0x0000
#define EFI_USB_ERR_NOTEXECUTE              0x0001
#define EFI_USB_ERR_STALL                   0x0002
#define EFI_USB_ERR_BUFFER                  0x0004
#define EFI_USB_ERR_BABBLE                  0x0008
#define EFI_USB_ERR_NAK                     0x0010
#define EFI_USB_ERR_CRC                     0x0020
#define EFI_USB_ERR_TIMEOUT                 0x0040
#define EFI_USB_ERR_BITSTUF                 0x0080
#define EFI_USB_ERR_SYSTEM                  0x0100

// Page 463 - UEFI Specs 2.8b
#define EFI_ABSP_SupportsAltActive    0x00000001
#define EFI_ABSP_SupportsPressureAsZ  0x00000002
// Page 466 - UEFI Specs 2.8b
#define EFI_ABSP_TouchActive          0x00000001
#define EFI_ABS_AltActive             0x00000002

// Page 794 - UEFI Specs 2.8b
#define USB_PORT_STAT_CONNECTION            0x0001
#define USB_PORT_STAT_ENABLE                0x0002
#define USB_PORT_STAT_SUSPEND               0x0004
#define USB_PORT_STAT_OVERCURRENT           0x0008
#define USB_PORT_STAT_RESET                 0x0010
#define USB_PORT_STAT_POWER                 0x0100
#define USB_PORT_STAT_LOW_SPEED             0x0200
#define USB_PORT_STAT_HIGH_SPEED            0x0400
#define USB_PORT_STAT_SUPER_SPEED           0x0800
#define USB_PORT_STAT_OWNER                 0x2000
#define USB_PORT_STAT_C_CONNECTION          0x0001
#define USB_PORT_STAT_C_ENABLE              0x0002
#define USB_PORT_STAT_C_SUSPEND             0x0004
#define USB_PORT_STAT_C_OVERCURRENT         0x0008
#define USB_PORT_STAT_C_RESET               0x0010

// Page 789 - UEFI Specs 2.8b
#define EFI_USB_MAX_ISO_BUFFER_NUM  7   // Listed on Page 789
#define EFI_USB_MAX_ISO_BUFFER_NUM1 2

// This is listed on page 782. However is missing the value that is supposed to be assigned to it.
// I added the 1024 just so that it will compile. I do not know the real value it should have.
// I emailed uefi.org about it. Waiting on response.
#define EFI_USB_MAX_BULK_BUFFER_NUM 1024


// Page 650 - UEFI Specs 2.8b
#define EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO    0x0001
#define EFI_PCI_ATTRIBUTE_ISA_IO                0x0002
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO        0x0004
#define EFI_PCI_ATTRIBUTE_VGA_MEMORY            0x0008
#define EFI_PCI_ATTRIBUTE_VGA_IO                0x0010
#define EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO        0x0020
#define EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO      0x0040
#define EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE  0x0080
#define EFI_PCI_ATTRIBUTE_MEMORY_CACHED         0x0800
#define EFI_PCI_ATTRIBUTE_MEMORY_DISABLE        0x1000
#define EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE    0x8000
#define EFI_PCI_ATTRIBUTE_ISA_IO_16             0x10000
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16     0x20000
#define EFI_PCI_ATTRIBUTE_VGA_IO_16             0x40000

// Page 689 - UEFI Specs 2.8b
#define EFI_PCI_IO_PASS_THROUGH_BAR                0xff

// Page 690 - UEFI Specs 2.8b
#define EFI_PCI_IO_ATTRIBUTE_ISA_MOTHERBOARD_IO    0x0001
#define EFI_PCI_IO_ATTRIBUTE_ISA_IO                0x0002
#define EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO        0x0004
#define EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY            0x0008
#define EFI_PCI_IO_ATTRIBUTE_VGA_IO                0x0010
#define EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO        0x0020
#define EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO      0x0040
#define EFI_PCI_IO_ATTRIBUTE_MEMORY_WRITE_COMBINE  0x0080
#define EFI_PCI_IO_ATTRIBUTE_IO                    0x0100
#define EFI_PCI_IO_ATTRIBUTE_MEMORY                0x0200
#define EFI_PCI_IO_ATTRIBUTE_BUS_MASTER            0x0400
#define EFI_PCI_IO_ATTRIBUTE_MEMORY_CACHED         0x0800
#define EFI_PCI_IO_ATTRIBUTE_MEMORY_DISABLE        0x1000
#define EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE       0x2000
#define EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM          0x4000
#define EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE    0x8000
#define EFI_PCI_IO_ATTRIBUTE_ISA_IO_16             0x10000
#define EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO_16     0x20000
#define EFI_PCI_IO_ATTRIBUTE_VGA_IO_16             0x40000


//typedef unsigned int            UINTN;     // Uncomment for 32-Bit
typedef unsigned long long      UINTN;     // Uncomment for 64-Bit

typedef unsigned char           UINT8;
typedef unsigned short          UINT16;
typedef unsigned int            UINT32;
typedef unsigned long long      UINT64;
typedef short                   INT16;
typedef int                     INT32;
typedef long long               INT64;
typedef unsigned char           CHAR8;
typedef unsigned short          CHAR16;
typedef unsigned char           BOOLEAN;
typedef void                    *EFI_HANDLE;
typedef void                    *EFI_EVENT;
typedef UINT64                  EFI_STATUS;
typedef UINTN                   EFI_TPL;
typedef UINT64                  EFI_LBA;
typedef UINT64                  EFI_PHYSICAL_ADDRESS;
typedef UINT64                  EFI_VIRTUAL_ADDRESS;

// Forward Declarations
struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct EFI_FILE_PROTOCOL;
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
struct EFI_DRIVER_BINDING_PROTOCOL;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct EFI_SIMPLE_POINTER_PROTOCOL;
struct EFI_ABSOLUTE_POINTER_PROTOCOL;
struct EFI_USB2_HC_PROTOCOL;
struct EFI_USB_IO_PROTOCOL;
struct EFI_USBFN_IO_PROTOCOL;
struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL;
struct EFI_PCI_IO_PROTOCOL;
struct EFI_ACPI_TABLE_PROTOCOL;

// Page 177 - UEFI Specs 2.8b
typedef struct EFI_GUID
{
    UINT32    Data1;
    UINT16    Data2;
    UINT16    Data3;
    UINT8     Data4[8];
} EFI_GUID;

// GUIDs from all over the UEFI 2.8b Specs
struct EFI_GUID EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID = {0x964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
struct EFI_GUID EFI_SIMPLE_POINTER_PROTOCOL_GUID     = {0x31878c87, 0x0b75, 0x11d5, {0x9a, 0x4f, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID EFI_LOADED_IMAGE_PROTOCOL_GUID       = {0x5b1b31a1, 0x9562, 0x11d2, {0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
struct EFI_GUID EFI_DEVICE_PATH_PROTOCOL_GUID        = {0x09576e91, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
struct EFI_GUID EFI_FILE_INFO_GUID                   = {0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
struct EFI_GUID EFI_FILE_SYSTEM_INFO_GUID            = {0x09576e93, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
struct EFI_GUID EFI_FILE_SYSTEM_VOLUME_LABEL_GUID    = {0xdb47d7d3, 0xfe81, 0x11d3, {0x9a, 0x35, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID EFI_DRIVER_BINDING_PROTOCOL_GUID     = {0x18a031ab, 0xb443, 0x4d1a, {0xa5, 0xc0, 0x0c, 0x09, 0x26, 0x1e, 0x9f, 0x71}};
struct EFI_GUID EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID    = {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};
struct EFI_GUID EFI_ABSOLUTE_POINTER_PROTOCOL_GUID   = {0x8D59D32B, 0xC655, 0x4AE9, {0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43}};
struct EFI_GUID EFI_GLOBAL_VARIABLE_GUID             = {0x8BE4DF61, 0x93CA, 0x11d2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C}};
struct EFI_GUID MPS_TABLE_GUID                       = {0xeb9d2d2f, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID ACPI_TABLE_GUID                      = {0xeb9d2d30, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID ACPI_20_TABLE_GUID                   = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};
struct EFI_GUID EFI_ACPI_TABLE_PROTOCOL_GUID         = {0xffe06bdd, 0x6107, 0x46a6, {0x7b, 0xb2, 0x5a, 0x9c, 0x7e, 0xc5, 0x27, 0x5c}};
struct EFI_GUID SMBIOS_TABLE_GUID                    = {0xeb9d2d31, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID SMBIOS3_TABLE_GUID                   = {0xf2fd1544, 0x9794, 0x4a2c, {0x99, 0x2e, 0xe5, 0xbb, 0xcf, 0x20, 0xe3, 0x94}};
struct EFI_GUID SAL_SYSTEM_TABLE_GUID                = {0xeb9d2d32, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID EFI_USB2_HC_PROTOCOL_GUID            = {0x3e745226, 0x9818, 0x45b6, {0xa2, 0xac, 0xd7, 0xcd, 0x0e, 0x8b, 0xa2, 0xbc}};
struct EFI_GUID EFI_USB_IO_PROTOCOL_GUID             = {0x2b2f68d6, 0x0cd2, 0x44cf, {0x8e, 0x8b, 0xbb, 0xa2, 0x0b, 0x1b, 0x5b, 0x75}};
struct EFI_GUID EFI_USBFN_IO_PROTOCOL_GUID           = {0x32d2963a, 0xfe5d, 0x4f30, {0xb6, 0x33, 0x6e, 0x5d, 0xc5, 0x58, 0x30, 0xcc}};
struct EFI_GUID EFI_BTT_ABSTRACTION_GUID             = {0x18633bfc, 0x1735, 0x4217, {0x8a, 0xc9, 0x17, 0x23, 0x92, 0x82, 0xd3, 0xf8}};
struct EFI_GUID EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID = {0x2f707ebb, 0x4a1a, 0x11d4, {0x9a, 0x38, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
struct EFI_GUID EFI_PCI_IO_PROTOCOL_GUID             = {0x4cf5b200, 0x68b8, 0x4ca5, {0x9e, 0xec, 0xb2, 0x3e, 0x3f, 0x50, 0x02, 0x9a}};

// Page 90 - UEFI Specs 2.8b
typedef struct EFI_TABLE_HEADER
{
    UINT64        Signature;
	UINT32        Revision;
	UINT32        HeaderSize;
	UINT32        CRC32;
	UINT32        Reserved;
} EFI_TABLE_HEADER;

// Page 99 - UEFI Specs 2.8b
typedef struct EFI_CONFIGURATION_TABLE
{
    EFI_GUID                    VendorGuid;
    void                        *VendorTable;
} EFI_CONFIGURATION_TABLE;

// Page 156 - UEFI Specs 2.8b
typedef enum EFI_TIMER_DELAY
{
    TimerCancel,
    TimerPeriodic,
    TimerRelative
} EFI_TIMER_DELAY;

// Page 160 ( BEFORE BOOT EXIT ) - UEFI Specs 2.8b
// Page 161 ( AFTER BOOT EXIT ) - UEFI Specs 2.8b
typedef enum EFI_MEMORY_TYPE
{
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

// Page 162 - UEFI Specs 2.8b
typedef enum EFI_ALLOCATE_TYPE
{
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

// Page 166 - UEFI Specs 2.8b
typedef struct EFI_MEMORY_DESCRIPTOR
{
    UINT32                   Type;
    EFI_PHYSICAL_ADDRESS     PhysicalStart;
    EFI_VIRTUAL_ADDRESS      VirtualStart;
    UINT64                   NumberOfPages;
    UINT64                   Attribute;
} EFI_MEMORY_DESCRIPTOR;

// Page 177 - UEFI Specs 2.8b
typedef enum EFI_INTERFACE_TYPE
{
    EFI_NATIVE_INTERFACE
} EFI_INTERFACE_TYPE;

// Page 183 - UEFI Specs 2.8b
typedef enum EFI_LOCATE_SEARCH_TYPE
{
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

// Page 198 - UEFI Specs 2.8b
typedef struct EFI_OPEN_PROTOCOL_INFORMATION_ENTRY
{
    EFI_HANDLE                  AgentHandle;
    EFI_HANDLE                  ControllerHandle;
    UINT32                      Attributes;
    UINT32                      OpenCount;
} EFI_OPEN_PROTOCOL_INFORMATION_ENTRY;

// Page 254 - UEFI Specs 2.8b
typedef struct EFI_TIME
{
	UINT16     Year;
	UINT8      Month;
	UINT8      Day;
	UINT8      Hour;
	UINT8      Minute;
	UINT8      Second;
	UINT8      Pad1;
	UINT32     Nanosecond;
	UINT16     TimeZone;
	UINT8      DayLight;
	UINT8      Pad2;
} EFI_TIME;

// Page 256 - UEFI Specs 2.8b
typedef struct EFI_TIME_CAPABILITIES
{
	UINT32      Resolution;
	UINT32      Accuracy;
	BOOLEAN     SetsToZero;
} EFI_TIME_CAPABILITIES;

// Page 265 - UEFI Specs 2.8b
typedef enum EFI_RESET_TYPE
{
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EFI_RESET_TYPE;

// Page 267 - UEFI Specs 2.8b
typedef struct EFI_CAPSULE_BLOCK_DESCRIPTOR
{
    UINT64     Length;
    union {
        EFI_PHYSICAL_ADDRESS    DataBlock;
        EFI_PHYSICAL_ADDRESS    ContinuationPointer;
    } Union;
} EFI_CAPSULE_BLOCK_DESCRIPTOR;

// Page 268 - UEFI Specs 2.8b
typedef struct EFI_CAPSULE_HEADER
{
    EFI_GUID                    CapsuleGuid;
    UINT32                      HeaderSize;
    UINT32                      Flags;
    UINT32                      CapsuleImageSize;
} EFI_CAPSULE_HEADER;

// Page 287 - UEFI Specs 2.8b
typedef struct EFI_DEVICE_PATH_PROTOCOL
{
    UINT8   Type;
    UINT8   SubType;
    UINT8   Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

// Page 442 - UEFI Specs 2.8b
typedef struct EFI_INPUT_KEY
{
	UINT16    ScanCode;
	UINT16    UnicodeChar;
}EFI_INPUT_KEY;

// Page 444 - UEFI Specs 2.8b
typedef struct SIMPLE_TEXT_OUTPUT_MODE
{
    INT32                       MaxMode;
    INT32                       Mode;
    INT32                       Attribute;
    INT32                       CursorColumn;
    INT32                       CursorRow;
    BOOLEAN                     CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

// Page 514 - UEFI Specs 2.8b
typedef struct EFI_FILE_IO_TOKEN
{
    EFI_EVENT   Event;
    EFI_STATUS  Status;
    UINTN       BufferSize;
    void        *Buffer;
} EFI_FILE_IO_TOKEN;


// Page 524 - UEFI Specs 2.8b
typedef struct EFI_FILE_INFO {
    UINT64      Size;
    UINT64      FileSize;
    UINT64      PhysicalSize;
    EFI_TIME    CreateTime;
    EFI_TIME    LastAccessTime;
    EFI_TIME    ModificationTime;
    UINT64      Attribute;
    CHAR16      FileName[];
} EFI_FILE_INFO;

// Page 526 - UEFI Specs 2.8b
typedef struct EFI_FILE_SYSTEM_INFO
{
    UINT64  Size;
    BOOLEAN ReadOnly;
    UINT64  VolumeSize;
    UINT64  FreeSpace;
    UINT32  BlockSize;
    CHAR16  VolumeLabel[];
} EFI_FILE_SYSTEM_INFO;

// Page 649 - UEFI Specs 2.8b
typedef enum EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH
{
    EfiPciWidthUint8,
    EfiPciWidthUint16,
    EfiPciWidthUint32,
    EfiPciWidthUint64,
    EfiPciWidthFifoUint8,
    EfiPciWidthFifoUint16,
    EfiPciWidthFifoUint32,
    EfiPciWidthFifoUint64,
    EfiPciWidthFillUint8,
    EfiPciWidthFillUint16,
    EfiPciWidthFillUint32,
    EfiPciWidthFillUint64,
    EfiPciWidthMaximum
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH;

// Page 653 - UEFI Specs 2.8b
typedef enum EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION
{
    EfiPciOperationBusMasterRead,
    EfiPciOperationBusMasterWrite,
    EfiPciOperationBusMasterCommonBuffer,
    EfiPciOperationBusMasterRead64,
    EfiPciOperationBusMasterWrite64,
    EfiPciOperationBusMasterCommonBuffer64,
    EfiPciOperationMaximum
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION;

// Page 689 - UEFI Specs 2.8b
typedef enum EFI_PCI_IO_PROTOCOL_WIDTH
{
    EfiPciIoWidthUint8,
    EfiPciIoWidthUint16,
    EfiPciIoWidthUint32,
    EfiPciIoWidthUint64,
    EfiPciIoWidthFifoUint8,
    EfiPciIoWidthFifoUint16,
    EfiPciIoWidthFifoUint32,
    EfiPciIoWidthFifoUint64,
    EfiPciIoWidthFillUint8,
    EfiPciIoWidthFillUint16,
    EfiPciIoWidthFillUint32,
    EfiPciIoWidthFillUint64,
    EfiPciIoWidthMaximum
} EFI_PCI_IO_PROTOCOL_WIDTH;

// Page 694 - UEFI Specs 2.8b
typedef enum EFI_PCI_IO_PROTOCOL_OPERATION
{
    EfiPciIoOperationBusMasterRead,
    EfiPciIoOperationBusMasterWrite,
    EfiPciIoOperationBusMasterCommonBuffer,
    EfiPciIoOperationMaximum
} EFI_PCI_IO_PROTOCOL_OPERATION;

// Page 713 - UEFI Specs 2.8b
typedef enum EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION
{
    EfiPciIoAttributeOperationGet,
    EfiPciIoAttributeOperationSet,
    EfiPciIoAttributeOperationEnable,
    EfiPciIoAttributeOperationDisable,
    EfiPciIoAttributeOperationSupport,
    EfiPciIoAttributeOperationMaximum
} EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION;

typedef void(*EFI_EVENT_NOTIFY)(EFI_EVENT Event, void *Context);

typedef EFI_STATUS (*EFI_INPUT_RESET)(struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_INPUT_READ_KEY)(struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);

// Page 440 - UEFI Specs 2.8b
typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL
{
	EFI_INPUT_RESET        Reset;
	EFI_INPUT_READ_KEY     ReadKeyStroke;
	EFI_EVENT              WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_TEXT_RESET)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_TEXT_STRING)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
typedef EFI_STATUS (*EFI_TEXT_TEST_STRING)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
typedef EFI_STATUS (*EFI_TEXT_QUERY_MODE)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber, UINTN *Columns, UINTN *Rows);
typedef EFI_STATUS (*EFI_TEXT_SET_MODE)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN ModeNumber);
typedef EFI_STATUS (*EFI_TEXT_SET_ATTRIBUTE)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Attribute);
typedef EFI_STATUS (*EFI_TEXT_CLEAR_SCREEN)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Column, UINTN Row);
typedef EFI_STATUS (*EFI_TEXT_ENABLE_CURSOR)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN Visible);

// Page 443 - UEFI Specs 2.8b
typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET                         Reset;
    EFI_TEXT_STRING                        OutputString;
    EFI_TEXT_TEST_STRING                   TestString;
    EFI_TEXT_QUERY_MODE                    QueryMode;
    EFI_TEXT_SET_MODE                      SetMode;
    EFI_TEXT_SET_ATTRIBUTE                 SetAttribute;
    EFI_TEXT_CLEAR_SCREEN                  ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION           SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR                 EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE                *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_RAISE_TPL)(EFI_TPL NewTpl);
typedef EFI_STATUS (*EFI_RESTORE_TPL)(EFI_TPL OldTpl);
typedef EFI_STATUS (*EFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE Type, UINTN MemoryType, UINTN Pages, EFI_PHYSICAL_ADDRESS *Memory);
typedef EFI_STATUS (*EFI_FREE_PAGES)(EFI_PHYSICAL_ADDRESS Memory, UINTN Pages);
typedef EFI_STATUS (*EFI_GET_MEMORY_MAP)(UINTN *MemoryMapSize, EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN *MapKey, UINTN *DescriptorSize, UINT32 *DescriptorVersion);
typedef EFI_STATUS (*EFI_ALLOCATE_POOL)(UINTN PoolType, UINTN Size, void **Buffer);
typedef EFI_STATUS (*EFI_FREE_POOL)(void *Buffer);
typedef EFI_STATUS (*EFI_CREATE_EVENT)(UINT32 Type, EFI_TPL NotifyTpl, EFI_EVENT_NOTIFY NotifyFunction, void *NotifyContext, EFI_GUID *EventGroup, EFI_EVENT *Event);
typedef EFI_STATUS (*EFI_SET_TIMER)(EFI_EVENT Event, EFI_TIMER_DELAY Type, UINT64 TriggerTime);
typedef EFI_STATUS (*EFI_WAIT_FOR_EVENT)(UINTN NumberOfEvents, EFI_EVENT *Event, UINTN *Index);
typedef EFI_STATUS (*EFI_SIGNAL_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*EFI_CLOSE_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*EFI_CHECK_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*EFI_INSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE *Handle, EFI_GUID *Protocol, EFI_INTERFACE_TYPE InterfaceType, void *Interface);
typedef EFI_STATUS (*EFI_REINSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE Handle, EFI_GUID *Protocol, void *OldInterface, void *NewInterface);
typedef EFI_STATUS (*EFI_UNINSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE Handle, EFI_GUID *Protocol, void *Interface);
typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol, void **Interface);
typedef EFI_STATUS (*EFI_REGISTER_PROTOCOL_NOTIFY)(EFI_GUID *Protocol, EFI_EVENT Event, void **Registration);
typedef EFI_STATUS (*EFI_LOCATE_HANDLE)(EFI_LOCATE_SEARCH_TYPE SearchType, EFI_GUID *Protocol, void *SearchKey, UINTN *BufferSize, EFI_HANDLE *Buffer);
typedef EFI_STATUS (*EFI_LOCATE_DEVICE_PATH)(EFI_GUID *Protocol, EFI_DEVICE_PATH_PROTOCOL **DevicePath, EFI_HANDLE *Device);
typedef EFI_STATUS (*EFI_INSTALL_CONFIGURATION_TABLE)(EFI_GUID *Guid, void *Table);
typedef EFI_STATUS (*EFI_IMAGE_LOAD)(BOOLEAN BootPolicy, EFI_HANDLE ParentImageHandle, EFI_DEVICE_PATH_PROTOCOL *DevicePath, void *SourceBuffer, UINTN SourceSize, EFI_HANDLE *ImageHandle);
typedef EFI_STATUS (*EFI_IMAGE_START)(EFI_HANDLE ImageHandle, UINTN *ExitDataSize, CHAR16 **ExitData);
typedef EFI_STATUS (*EFI_EXIT)(EFI_HANDLE ImageHandle, EFI_STATUS ExitStatus, UINTN ExitDataSize, CHAR16 *ExitData);
typedef EFI_STATUS (*EFI_IMAGE_UNLOAD)(EFI_HANDLE ImageHandle);
typedef EFI_STATUS (*EFI_EXIT_BOOT_SERVICES)(EFI_HANDLE ImageHandle, UINTN MapKey);
typedef EFI_STATUS (*EFI_GET_NEXT_MONOTONIC_COUNT)(UINT64 *Count);
typedef EFI_STATUS (*EFI_STALL)(UINTN Microseconds);
typedef EFI_STATUS (*EFI_SET_WATCHDOG_TIMER)(UINTN Timeout, UINT64 WatchdogCode, UINTN DataSize, CHAR16 *WatchdogData);
typedef EFI_STATUS (*EFI_CONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle, EFI_HANDLE *DriverImageHandle, EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath, BOOLEAN Recursive);
typedef EFI_STATUS (*EFI_DISCONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle, EFI_HANDLE DriverImageHandle, EFI_HANDLE ChildHandle);
typedef EFI_STATUS (*EFI_OPEN_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol, void **Interface, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle, UINT32 Attributes);
typedef EFI_STATUS (*EFI_CLOSE_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle);
typedef EFI_STATUS (*EFI_OPEN_PROTOCOL_INFORMATION)(EFI_HANDLE Handle, EFI_GUID *Protocol, EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **EntryBuffer, UINTN *EntryCount);
typedef EFI_STATUS (*EFI_PROTOCOLS_PER_HANDLE)(EFI_HANDLE Handle, EFI_GUID ***ProtocolBuffer, UINTN *ProtocolBufferCount);
typedef EFI_STATUS (*EFI_LOCATE_HANDLE_BUFFER)(EFI_LOCATE_SEARCH_TYPE SearchType, EFI_GUID *Protocol, void *SearchKey, UINTN *NoHandles, EFI_HANDLE **Buffer);
typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL)(EFI_GUID *Protocol, void *Registration, void **Interface);
typedef EFI_STATUS (*EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES)(EFI_HANDLE *Handle, ...);
typedef EFI_STATUS (*EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES)(EFI_HANDLE *Handle, ...);
typedef EFI_STATUS (*EFI_CALCULATE_CRC32)(void *Data, UINTN DataSize, UINT32 *Crc32);
typedef EFI_STATUS (*EFI_COPY_MEM)(void *Destination, void *Source, UINTN Length);
typedef EFI_STATUS (*EFI_SET_MEM)(void *Buffer, UINTN Size, UINT8 Value);
typedef EFI_STATUS (*EFI_CREATE_EVENT_EX)(UINT32 Type, EFI_TPL NotifyTpl, EFI_EVENT_NOTIFY NotifyFunction, void *NotifyContext, EFI_GUID *EventGroup, EFI_EVENT *Event);

// Page 93 - UEFI Specs 2.8b
typedef struct EFI_BOOT_SERVICES
{
    EFI_TABLE_HEADER                                 Hdr;
    EFI_RAISE_TPL                                    RaiseTPL;
    EFI_RESTORE_TPL                                  RestoreTPL;
    EFI_ALLOCATE_PAGES                               AllocatePages;
    EFI_FREE_PAGES                                   FreePages;
    EFI_GET_MEMORY_MAP                               GetMemoryMap;
    EFI_ALLOCATE_POOL                                AllocatePool;
    EFI_FREE_POOL                                    FreePool;
    EFI_CREATE_EVENT                                 CreateEvent;
    EFI_SET_TIMER                                    SetTimer;
    EFI_WAIT_FOR_EVENT                               WaitForEvent;
    EFI_SIGNAL_EVENT                                 SignalEvent;
    EFI_CLOSE_EVENT                                  CloseEvent;
    EFI_CHECK_EVENT                                  CheckEvent;
    EFI_INSTALL_PROTOCOL_INTERFACE                   InstallProtocolInterface;
    EFI_REINSTALL_PROTOCOL_INTERFACE                 ReinstallProtocolInterface;
    EFI_UNINSTALL_PROTOCOL_INTERFACE                 UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL                              HandleProtocol;
    void                                             *Reserved;
    EFI_REGISTER_PROTOCOL_NOTIFY                     RegisterProtocolNotify;
    EFI_LOCATE_HANDLE                                LocateHandle;
    EFI_LOCATE_DEVICE_PATH                           LocateDevicePath;
    EFI_INSTALL_CONFIGURATION_TABLE                  InstallConfigurationTable;
    EFI_IMAGE_LOAD                                   LoadImage;
    EFI_IMAGE_START                                  StartImage;
    EFI_EXIT                                         Exit;
    EFI_IMAGE_UNLOAD                                 UnloadImage;
    EFI_EXIT_BOOT_SERVICES                           ExitBootServices;
    EFI_GET_NEXT_MONOTONIC_COUNT                     GetNextMonotonicCount;
    EFI_STALL                                        Stall;
    EFI_SET_WATCHDOG_TIMER                           SetWatchdogTimer;
    EFI_CONNECT_CONTROLLER                           ConnectController;
    EFI_DISCONNECT_CONTROLLER                        DisconnectController;
    EFI_OPEN_PROTOCOL                                OpenProtocol;
    EFI_CLOSE_PROTOCOL                               CloseProtocol;
    EFI_OPEN_PROTOCOL_INFORMATION                    OpenProtocolInformation;
    EFI_PROTOCOLS_PER_HANDLE                         ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER                         LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL                              LocateProtocol;
    EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES         InstallMultipleProtocolInterfaces;
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES       UninstallMultipleProtocolInterfaces;
    EFI_CALCULATE_CRC32                              CalculateCrc32;
    EFI_COPY_MEM                                     CopyMem;
    EFI_SET_MEM                                      SetMem;
    EFI_CREATE_EVENT_EX                              CreateEventEx;
} EFI_BOOT_SERVICES;

typedef EFI_STATUS (*EFI_GET_TIME)(EFI_TIME *Time, EFI_TIME_CAPABILITIES *Capabilities);
typedef EFI_STATUS (*EFI_SET_TIME)(EFI_TIME *Time);
typedef EFI_STATUS (*EFI_GET_WAKEUP_TIME)(BOOLEAN *Enabled, BOOLEAN *Pending, EFI_TIME *Time);
typedef EFI_STATUS (*EFI_SET_WAKEUP_TIME)(BOOLEAN Enable, EFI_TIME *Time);
typedef EFI_STATUS (*EFI_SET_VIRTUAL_ADDRESS_MAP)(UINTN MemoryMapSize, UINTN DescriptorSize, UINT32 DescriptorVersion, EFI_MEMORY_DESCRIPTOR *VirtualMap);
typedef EFI_STATUS (*EFI_CONVERT_POINTER)(UINTN DebugDisposition, void **Address);
typedef EFI_STATUS (*EFI_GET_VARIABLE)(CHAR16 *VariableName, EFI_GUID *VendorGuid, UINT32 *Attributes, UINTN *DataSize, void *Data);
typedef EFI_STATUS (*EFI_GET_NEXT_VARIABLE_NAME)(UINTN *VariableNameSize, CHAR16 *VariableName, EFI_GUID *VendorGuid);
typedef EFI_STATUS (*EFI_SET_VARIABLE)(CHAR16 *VariableName, EFI_GUID *VendorGuid, UINT32 Attributes, UINTN DataSize, void *Data);
typedef EFI_STATUS (*EFI_GET_NEXT_HIGH_MONO_COUNT)(UINT32 *HighCount);
typedef EFI_STATUS (*EFI_RESET_SYSTEM)(EFI_RESET_TYPE ResetType, EFI_STATUS ResetStatus, UINTN DataSize, void *ResetData);
typedef EFI_STATUS (*EFI_UPDATE_CAPSULE)(EFI_CAPSULE_HEADER **CapsuleHeaderArray, UINTN CapsuleCount, EFI_PHYSICAL_ADDRESS ScatterGatherList);
typedef EFI_STATUS (*EFI_QUERY_CAPSULE_CAPABILITIES)(EFI_CAPSULE_HEADER **CapsuleHeaderArray, UINTN CapsuleCount, UINT64 *MaximumCapsuleSize, EFI_RESET_TYPE *ResetType);
typedef EFI_STATUS (*EFI_QUERY_VARIABLE_INFO)(UINT32 Attributes, UINT64 *MaximumVariableStorageSize, UINT64 *RemainingVariableStorageSize, UINT64 *MaximumVariableSize);

// Page 97 - UEFI Specs 2.8b
typedef struct EFI_RUNTIME_SERVICES
{
    EFI_TABLE_HEADER                    Hdr;
    EFI_GET_TIME                        GetTime;
    EFI_SET_TIME                        SetTime;
    EFI_GET_WAKEUP_TIME                 GetWakeupTime;
    EFI_SET_WAKEUP_TIME                 SetWakeupTime;
    EFI_SET_VIRTUAL_ADDRESS_MAP         SetVirtualAddressMap;
    EFI_CONVERT_POINTER                 ConvertPointer;
    EFI_GET_VARIABLE                    GetVariable;
    EFI_GET_NEXT_VARIABLE_NAME          GetNextVariableName;
    EFI_SET_VARIABLE                    SetVariable;
    EFI_GET_NEXT_HIGH_MONO_COUNT        GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM                    ResetSystem;
    EFI_UPDATE_CAPSULE                  UpdateCapsule;
    EFI_QUERY_CAPSULE_CAPABILITIES      QueryCapsuleCapabilities;
    EFI_QUERY_VARIABLE_INFO             QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

// Page 92 - UEFI Specs 2.8b
typedef struct EFI_SYSTEM_TABLE
{
	EFI_TABLE_HEADER                              hdr;
	CHAR16                                        *FirmwareVendor;
	UINT32                                        FirmwareVersion;
	EFI_HANDLE                                    ConsoleInHandle;
	EFI_SIMPLE_TEXT_INPUT_PROTOCOL                *ConIn;
	EFI_HANDLE                                    ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL               *ConOut;
	EFI_HANDLE                                    StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL               *StdErr;
	EFI_RUNTIME_SERVICES                          *RuntimeServices;
	EFI_BOOT_SERVICES                             *BootServices;
	UINTN                                         NumberOfTableEntries;
	EFI_CONFIGURATION_TABLE                       *ConfigurationTable;
} EFI_SYSTEM_TABLE;

// Page 283 - UEFI Spacs 2.8b
typedef struct EFI_LOADED_IMAGE_PROTOCOL
{
    UINT32                      Revision;
    EFI_HANDLE                  ParentHandle;
    EFI_SYSTEM_TABLE            *SystemTable;
    EFI_HANDLE                  DeviceHandle;
    EFI_DEVICE_PATH_PROTOCOL    *FilePath;
    void                        *Reserved;
    UINT32                      LoadOptionsSize;
    void                        *LoadOptions;
    void                        *ImageBase;
    UINT64                      ImageSize;
    EFI_MEMORY_TYPE             ImageCodeType;
    EFI_MEMORY_TYPE             ImageDataType;
    EFI_IMAGE_UNLOAD            Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

typedef EFI_STATUS (*EFI_FILE_OPEN)(struct EFI_FILE_PROTOCOL *This, struct EFI_FILE_PROTOCOL **NewHandle, CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes);
typedef EFI_STATUS (*EFI_FILE_CLOSE)(struct EFI_FILE_PROTOCOL *This);
typedef EFI_STATUS (*EFI_FILE_DELETE)(struct EFI_FILE_PROTOCOL *This);
typedef EFI_STATUS (*EFI_FILE_READ)(struct EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_FILE_WRITE)(struct EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_FILE_GET_POSITION)(struct EFI_FILE_PROTOCOL *This, UINT64 *Position);
typedef EFI_STATUS (*EFI_FILE_SET_POSITION)(struct EFI_FILE_PROTOCOL *This, UINT64 Position);
typedef EFI_STATUS (*EFI_FILE_GET_INFO)(struct EFI_FILE_PROTOCOL *This, EFI_GUID *InformationType, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_FILE_SET_INFO)(struct EFI_FILE_PROTOCOL *This, EFI_GUID *InformationType, UINTN BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_FILE_FLUSH)(struct EFI_FILE_PROTOCOL *This);
typedef EFI_STATUS (*EFI_FILE_OPEN_EX)(struct EFI_FILE_PROTOCOL *This, struct EFI_FILE_PROTOCOL **NewHandle, CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes, EFI_FILE_IO_TOKEN *Token);
typedef EFI_STATUS (*EFI_FILE_READ_EX)(struct EFI_FILE_PROTOCOL *This, EFI_FILE_IO_TOKEN *Token);
typedef EFI_STATUS (*EFI_FILE_WRITE_EX)(struct EFI_FILE_PROTOCOL *This, EFI_FILE_IO_TOKEN *Token);
typedef EFI_STATUS (*EFI_FILE_FLUSH_EX)(struct EFI_FILE_PROTOCOL *This, EFI_FILE_IO_TOKEN *Token);

// Page 507 - UEFI Specs 2.8b
typedef struct EFI_FILE_PROTOCOL
{
    UINT64                  Revision;
    EFI_FILE_OPEN           Open;
    EFI_FILE_CLOSE          Close;
    EFI_FILE_DELETE         Delete;
    EFI_FILE_READ           Read;
    EFI_FILE_WRITE          Write;
    EFI_FILE_GET_POSITION   GetPosition;
    EFI_FILE_SET_POSITION   SetPosition;
    EFI_FILE_GET_INFO       GetInfo;
    EFI_FILE_SET_INFO       SetInfo;
    EFI_FILE_FLUSH          Flush;
    EFI_FILE_OPEN_EX        OpenEx;
    EFI_FILE_READ_EX        ReadEx;
    EFI_FILE_WRITE_EX       WriteEx;
    EFI_FILE_FLUSH_EX       FlushEx;
} EFI_FILE_PROTOCOL;

typedef EFI_STATUS (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **Root);

// Page 504 - UEFI Specs 2.8b
typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
{
    UINT64                                       Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME  OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EFI_STATUS (*EFI_DRIVER_BINDING_PROTOCOL_SUPPORTED)(struct EFI_DRIVER_BINDING_PROTOCOL *This, EFI_HANDLE ControllerHandle, EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath);
typedef EFI_STATUS (*EFI_DRIVER_BINDING_PROTOCOL_START)(struct EFI_DRIVER_BINDING_PROTOCOL *This, EFI_HANDLE ControllerHandle, EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath);
typedef EFI_STATUS (*EFI_DRIVER_BINDING_PROTOCOL_STOP)(struct EFI_DRIVER_BINDING_PROTOCOL *This, EFI_HANDLE ControllerHandle, UINTN NumberOfChildren, EFI_HANDLE *ChildHandleBuffer);

// Page 356 - UEFI Specs 2.8b
typedef struct EFI_DRIVER_BINDING_PROTOCOL
{
    EFI_DRIVER_BINDING_PROTOCOL_SUPPORTED    Supported;
    EFI_DRIVER_BINDING_PROTOCOL_START        Start;
    EFI_DRIVER_BINDING_PROTOCOL_STOP         Stop;
    UINT32                                   Version;
    EFI_HANDLE                               ImageHandle;
    EFI_HANDLE                               DriverBindingHandle;
} EFI_DRIVER_BINDING_PROTOCOL;


// GRAPHICS
// Page 485 - UEFI Specs 2.8b
typedef enum EFI_GRAPHICS_OUTPUT_BLT_OPERATION
{
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

// Page 485 - UEFI Specs 2.8b
typedef struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL
{
    UINT8   Blue;
    UINT8   Green;
    UINT8   Red;
    UINT8   Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

// Page 480 - UEFI Specs 2.8b
typedef enum EFI_GRAPHICS_PIXEL_FORMAT
{
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

// Page 479 - UEFI Specs 2.8b
typedef struct EFI_PIXEL_BITMASK
{
    UINT32    RedMask;
    UINT32    GreenMask;
    UINT32    BlueMask;
    UINT32    ReservedMask;
} EFI_PIXEL_BITMASK;

// Page 480 - UEFI Specs 2.8b
typedef struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION
{
    UINT32                      Version;
    UINT32                      HorizontalResolution;
    UINT32                      VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT   PixelFormat;
    EFI_PIXEL_BITMASK           PixelInformation;
    UINT32                      PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

// Page 482 - UEFI Specs 2.8b
typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
{
    UINT32                                MaxMode;
    UINT32                                Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
    UINTN                                 SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                  FrameBufferBase;
    UINTN                                 FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *This, UINT32 ModeNumber, UINTN *SizeOfInfo, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info);
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *This, UINT32 ModeNumber);
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT)(struct EFI_GRAPHICS_OUTPUT_PROTOCOL *This, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer, EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation, UINTN SourceX, UINTN SourceY, UINTN DestinationX, UINTN DestinationY, UINTN Width, UINTN Height, UINTN Delta);

// Page 479 - UEFI Specs 2.8b
typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE  QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE    SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT         Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE        *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;


// MOUSE
// Page 455 - UEFI Specs 2.8b
typedef struct EFI_SIMPLE_POINTER_MODE
{
	UINT64      ResolutionX;
	UINT64      ResolutionY;
	UINT64      ResolutionZ;
	BOOLEAN     LeftButton;
	BOOLEAN     RightButton;
} EFI_SIMPLE_POINTER_MODE;

// Page 457 - UEFI Specs 2.8b
typedef struct EFI_SIMPLE_POINTER_STATE
{
	INT32 RelativeMovementX;
	INT32 RelativeMovementY;
	INT32 RelativeMovementZ;
	BOOLEAN LeftButton;
	BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef EFI_STATUS (*EFI_SIMPLE_POINTER_RESET)(struct EFI_SIMPLE_POINTER_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_SIMPLE_POINTER_GET_STATE)(struct EFI_SIMPLE_POINTER_PROTOCOL *This, EFI_SIMPLE_POINTER_STATE *State);

// Page 455 - UEFI Specs 2.8b
typedef struct EFI_SIMPLE_POINTER_PROTOCOL {
	EFI_SIMPLE_POINTER_RESET       Reset;
	EFI_SIMPLE_POINTER_GET_STATE   GetState;
	EFI_EVENT                      WaitForInput;
	EFI_SIMPLE_POINTER_MODE        *Mode;
} EFI_SIMPLE_POINTER_PROTOCOL;


// Touch Screen
// Page 463 - UEFI Specs 2.8b
typedef struct {
	UINT64    AbsoluteMinX;
	UINT64    AbsoluteMinY;
	UINT64    AbsoluteMinZ;
	UINT64    AbsoluteMaxX;
	UINT64    AbsoluteMaxY;
	UINT64    AbsoluteMaxZ;
	UINT32    Attributes;
} EFI_ABSOLUTE_POINTER_MODE;

// Page 465 - UEFI Specs 2.8b
typedef struct EFI_ABSOLUTE_POINTER_STATE
{
	UINT64    CurrentX;
	UINT64    CurrentY;
	UINT64    CurrentZ;
	UINT32    ActiveButtons;
} EFI_ABSOLUTE_POINTER_STATE;

typedef EFI_STATUS (*EFI_ABSOLUTE_POINTER_RESET)(struct EFI_ABSOLUTE_POINTER_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_ABSOLUTE_POINTER_GET_STATE)(struct EFI_ABSOLUTE_POINTER_PROTOCOL *This, EFI_ABSOLUTE_POINTER_STATE *State);

// Page 462 - UEFI Specs 2.8b
typedef struct EFI_ABSOLUTE_POINTER_PROTOCOL
{
	EFI_ABSOLUTE_POINTER_RESET      Reset;
	EFI_ABSOLUTE_POINTER_GET_STATE  GetState;
	EFI_EVENT                       WaitForInput;
	EFI_ABSOLUTE_POINTER_MODE       *Mode;
} EFI_ABSOLUTE_POINTER_PROTOCOL;

// USB2
// Page 777 - UEFI Specs 2.8b
typedef enum EFI_USB_HC_STATE
{
    EfiUsbHcStateHalt,
    EfiUsbHcStateOperational,
    EfiUsbHcStateSuspend,
    EfiUsbHcStateMaximum
} EFI_USB_HC_STATE;

// Page 780 - UEFI Specs 2.8b
typedef struct EFI_USB2_HC_TRANSACTION_TRANSLATOR // Typo here, fixed
{
    UINT8 TransLatorHubAddress;
    UINT8 TranslatorPortNumber;
} EFI_USB2_HC_TRANSACTION_TRANSLATOR;

// Page 806 - UEFI Specs 2.8b
typedef enum EFI_USB_DATA_DIRECTION
{
    EfiUsbDataIn,
    EfiUsbDataOut,
    EfiUsbNoData
} EFI_USB_DATA_DIRECTION;

// Page 797 - UEFI Specs 2.8b
typedef enum EFI_USB_PORT_FEATURE
{
    EfiUsbPortEnable             = 1,
    EfiUsbPortSuspend            = 2,
    EfiUsbPortReset              = 4,
    EfiUsbPortPower              = 8,
    EfiUsbPortOwner              = 13,
    EfiUsbPortConnectChange      = 16,
    EfiUsbPortEnableChange       = 17,
    EfiUsbPortSuspendChange      = 18,
    EfiUsbPortOverCurrentChange  = 19,
    EfiUsbPortResetChange        = 20
} EFI_USB_PORT_FEATURE;

// Page 806 - UEFI Specs 2.8b
typedef struct EFI_USB_DEVICE_REQUEST
{
    UINT8    RequestType;
    UINT8    Request;
    UINT16   Value;
    UINT16   Index;
    UINT16   Length;
} EFI_USB_DEVICE_REQUEST;

// Page 794 - UEFI Specs 2.8b
typedef struct EFI_USB_PORT_STATUS
{
    UINT16 PortStatus;
    UINT16 PortChangeStatus;
} EFI_USB_PORT_STATUS;

typedef EFI_STATUS (*EFI_ASYNC_USB_TRANSFER_CALLBACK)(void *Data, UINTN DataLength, void *Context, UINT32 Status);

typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_GET_CAPABILITY)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 *MaxSpeed, UINT8 *PortNumber, UINT8 *Is64BitCapabile);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_RESET)(struct EFI_USB2_HC_PROTOCOL *This, UINT16 Attributes);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_GET_STATE)(struct EFI_USB2_HC_PROTOCOL *This, EFI_USB_HC_STATE *State);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_SET_STATE)(struct EFI_USB2_HC_PROTOCOL *This, EFI_USB_HC_STATE *State);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 DeviceAddress, UINT8 DeviceSpeed, UINTN MaximumPacketLength, EFI_USB_DEVICE_REQUEST *Request, EFI_USB_DATA_DIRECTION TransferDirection, void *Data, UINTN *DataLength, UINTN TimeOut, EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, UINT32 *TransferResult);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_BULK_TRANSFER)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 DeviceAddress, UINT8 EndPointAddress, UINT8 DeviceSpeed, UINTN MaximumPacketLength, UINT8 DataBuffersNumber, void *Data[EFI_USB_MAX_BULK_BUFFER_NUM], UINTN *DataLength, UINT8 *DataToggle, UINT8 TimeOut, EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, UINT32 *TransferResult);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 DeviceAddress, UINT8 EndPointAddress, UINT8 DeviceSpeed, UINTN MaximumPacketLength, BOOLEAN IsNewTransfer, UINT8 *DataToggle, UINTN PollingInterview, UINTN DataLength, EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, EFI_ASYNC_USB_TRANSFER_CALLBACK CallBackFunction, void *Context);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 DeviceAddress, UINT8 EndPointAddress, UINT8 DeviceSpeed, UINTN MaximumPacketLength, void *Data, UINTN *DataLength, UINT8 *DataToggle, UINTN TimeOut, EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, UINT32 *TransferResult);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 DeviceAddress, UINT8 EndPointAddress, UINT8 DeviceSpeed, UINTN MaximumPacketLength, UINT8 DataBuffersNumber, void *Data[EFI_USB_MAX_ISO_BUFFER_NUM], UINTN DataLength, EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, UINT32 *TransferResult);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 DeviceAddress, UINT8 EndPointAddress, UINT8 DeviceSpeed, UINTN MaximumPacketLength, UINT8 DataBuffersNumber, void *Data[EFI_USB_MAX_ISO_BUFFER_NUM], UINTN DataLength, EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, EFI_ASYNC_USB_TRANSFER_CALLBACK IsochronousCallBack, void *Context);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 PortNumber, EFI_USB_PORT_STATUS *PortStatus);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 PortNumber, EFI_USB_PORT_FEATURE PortFeature);
typedef EFI_STATUS (*EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE)(struct EFI_USB2_HC_PROTOCOL *This, UINT8 PortNumber, EFI_USB_PORT_FEATURE PortFeature);

// Page 772 - UEFI Specs 2.8b
typedef struct EFI_USB2_HC_PROTOCOL
{
    EFI_USB2_HC_PROTOCOL_GET_CAPABILITY                GetCapability;
    EFI_USB2_HC_PROTOCOL_RESET                         Reset;
    EFI_USB2_HC_PROTOCOL_GET_STATE                     GetState;
    EFI_USB2_HC_PROTOCOL_SET_STATE                     SetState;
    EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER              ControlTransfer;
    EFI_USB2_HC_PROTOCOL_BULK_TRANSFER                 BulkTransfer;
    EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER      AsyncInterruptTransfer;
    EFI_USB2_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER       SyncInterruptTransfer;
    EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER          IsochronousTranfer;
    EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER    AsyncIsochronousTransfer;
    EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS       GetRootHubPortStatus;
    EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE      SetRootHubPortFeature;
    EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE    ClearRootHubPortFeature;
    UINT16                                             MajorRevision;
    UINT16                                             MinorRevision;
} EFI_USB2_HC_PROTOCOL;


// USB_IO
// Page 817 - UEFI Specs 2.8b
typedef struct EFI_USB_DEVICE_DESCRIPTOR
{  // See USB 1.1
    UINT8    Length;
    UINT8    DescriptorType;
    UINT16   BcdUSB;
    UINT8    DeviceClass;
    UINT8    DeviceSubClass;
    UINT8    DeviceProtocol;
    UINT8    MaxPacketSize0;
    UINT16   IdVendor;
    UINT16   IdProduct;
    UINT16   BcdDevice;
    UINT8    StrManufacturer;
    UINT8    StrProduct;
    UINT8    StrSerialNumber;
    UINT8    NumConfigurations;
} EFI_USB_DEVICE_DESCRIPTOR;

// Page 818 - UEFI Specs 2.8b
typedef struct EFI_USB_CONFIG_DESCRIPTOR
{  // See USB 1.1
    UINT8   Length;
    UINT8   DescriptorType;
    UINT16  TotalLength;
    UINT8   NumInterfaces;
    UINT8   ConfigurationValue;
    UINT8   Configuration;
    UINT8   Attributes;
    UINT8   MaxPower;
} EFI_USB_CONFIG_DESCRIPTOR;

// Page 819 - UEFI Specs 2.8b
typedef struct EFI_USB_INTERFACE_DESCRIPTOR
{  // See USB 1.1
    UINT8    Length;
    UINT8    DescriptorType;
    UINT8    InterfaceNumber;
    UINT8    AlternateSetting;
    UINT8    NumEndpoints;
    UINT8    InterfaceClass;
    UINT8    InterfaceSubClass;
    UINT8    InterfaceProtocol;
    UINT8    Interface;
} EFI_USB_INTERFACE_DESCRIPTOR;

// Page 821 - UEFI Specs 2.8b
typedef struct EFI_USB_ENDPOINT_DESCRIPTOR
{  // See USB 1.1
    UINT8    Length;
    UINT8    DescriptorType;
    UINT8    EndPointAddress;
    UINT8    Attributes;
    UINT16   MaxPacketSize;
    UINT8    Interval;
} EFI_USB_ENDPOINT_DESCRIPTOR;

 typedef EFI_STATUS (*EFI_USB_IO_CONTROL_TRANSFER)(struct EFI_USB_IO_PROTOCOL *This, EFI_USB_DEVICE_REQUEST *Request, EFI_USB_DATA_DIRECTION Direction, UINT32 Timeout, void *Data, UINTN DataLength, UINT32 *Status);
 typedef EFI_STATUS (*EFI_USB_IO_BULK_TRANSFER)(struct EFI_USB_IO_PROTOCOL *This, UINT8 DeviceEndpoint, void *Data, UINTN *DataLength, UINTN Timeout, UINT32 *Status);
 typedef EFI_STATUS (*EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER)(struct EFI_USB_IO_PROTOCOL *This, UINT8 DeviceEndpoint, BOOLEAN IsNewTransfer, UINTN PollingInterview, UINTN DataLength, EFI_ASYNC_USB_TRANSFER_CALLBACK InterruptCallback, void *Context);
 typedef EFI_STATUS (*EFI_USB_IO_SYNC_INTERRUPT_TRANSFER)(struct EFI_USB_IO_PROTOCOL *This, UINT8 DeviceEndpoint, void *Data, UINTN *DataLength, UINTN Timeout, UINT32 *Status);
 typedef EFI_STATUS (*EFI_USB_IO_ISOCHRONOUS_TRANSFER)(struct EFI_USB_IO_PROTOCOL *This, UINT8 DeviceEndpoint, void *Data, UINTN DataLength, UINT32 *Status);
 typedef EFI_STATUS (*EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER)(struct EFI_USB_IO_PROTOCOL *This,  UINT8 DeviceEndpoint, void *Data, UINTN DataLength, EFI_ASYNC_USB_TRANSFER_CALLBACK IsochronousCallBack, void *Context);
 typedef EFI_STATUS (*EFI_USB_IO_GET_DEVICE_DESCRIPTOR)(struct EFI_USB_IO_PROTOCOL *This, EFI_USB_DEVICE_DESCRIPTOR *DeviceDescriptor);
 typedef EFI_STATUS (*EFI_USB_IO_GET_CONFIG_DESCRIPTOR)(struct EFI_USB_IO_PROTOCOL *This, EFI_USB_CONFIG_DESCRIPTOR *ConfigurationDescriptor);
 typedef EFI_STATUS (*EFI_USB_IO_INTERFACE_DESCRIPTOR)(struct EFI_USB_IO_PROTOCOL *This, EFI_USB_INTERFACE_DESCRIPTOR *InterfaceDescriptor);
 typedef EFI_STATUS (*EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR)(struct EFI_USB_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USB_ENDPOINT_DESCRIPTOR *EndpointDescriptor);
 typedef EFI_STATUS (*EFI_USB_IO_GET_STRING_DESCRIPTOR)(struct EFI_USB_IO_PROTOCOL *This, UINT16 LangID, UINT8 StringID, CHAR16 **String);
 typedef EFI_STATUS (*EFI_USB_IO_GET_SUPPORTED_LANGUAGES)(struct EFI_USB_IO_PROTOCOL *This, UINT16 **LangIDTable, UINT16 *TableSize);
 typedef EFI_STATUS (*EFI_USB_IO_PORT_RESET)(struct EFI_USB_IO_PROTOCOL *This);

// Page 803 - UEFI Specs 2.8b
typedef struct EFI_USB_IO_PROTOCOL
{
    EFI_USB_IO_CONTROL_TRANSFER               UsbControlTransfer;
    EFI_USB_IO_BULK_TRANSFER                  UsbBulkTransfer;
    EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER       UsbAsyncInterruptTransfer;
    EFI_USB_IO_SYNC_INTERRUPT_TRANSFER        UsbSyncInterruptTransfer;
    EFI_USB_IO_ISOCHRONOUS_TRANSFER           UsbIsochronousTransfer;
    EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER     UsbAsyncIsochronousTransfer;
    EFI_USB_IO_GET_DEVICE_DESCRIPTOR          UsbGetDeviceDescriptor;
    EFI_USB_IO_GET_CONFIG_DESCRIPTOR          UsbGetConfigDescriptor;
    EFI_USB_IO_INTERFACE_DESCRIPTOR           UsbGetInterfaceDescriptor;
    EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR        UsbGetendpointDescriptor;
    EFI_USB_IO_GET_STRING_DESCRIPTOR          UsbGetStringDescriptor;
    EFI_USB_IO_GET_SUPPORTED_LANGUAGES        UsbGetSupportedLanguages;
    EFI_USB_IO_PORT_RESET                     UsbPortReset;
} EFI_USB_IO_PROTOCOL;


// USBFN_IO
// Page 828 - UEFI Specs 2.8b
typedef enum EFI_USBFN_PORT_TYPE
{
    EfiUsbUnknownPort = 0,
    EfiUsbStandardDownstreamPort,
    EfiUsbChargingDownstreamPort,
    EfiUsbDedicatedChargingPort,
    EfiUsbInvalidDedicatedChargingPort
} EFI_USBFN_PORT_TYPE;

// Page 831 - UEFI Specs 2.8b
typedef enum EFI_USB_ENDPOINT_TYPE
{
    UsbEndpointControl      = 0x00,
    UsbEndpointIsochronous  = 0x01,  // This is commented out in the PDF 2.8b
    UsbEndpointBulk         = 0x02,
    UsbEndpointInterrupt    = 0x03   // This is commented out in the PDF 2.8b
} EFI_USB_ENDPOINT_TYPE;

// Page 832 - UEFI Specs 2.8b
typedef enum EFI_USBFN_DEVICE_INFO_ID
{
    EfiUsbDeviceInfoUnknown = 0,
    EfiUsbDeviceInfoSerialNumber,
    EfiUsbDeviceInfoManufacturerName,
    EfiUsbDeviceInfoProductName
} EFI_USBFN_DEVICE_INFO_ID;

// Page 833 - UEFI Specs 2.8b
typedef enum EFI_USBFN_ENDPOINT_DIRECTION
{
    EfiUsbEndpointDirectionHostOut = 0,
    EfiUsbEndpointDirectionHostIn,
    EfiUsbEndpointDirectionDeviceTx = EfiUsbEndpointDirectionHostIn,
    EfiUsbEndpointDirectionDeviceRx = EfiUsbEndpointDirectionHostOut
} EFI_USBFN_ENDPOINT_DIRECTION;

// Page 838 - UEFI Specs 2.8b
// NOTE : This has explanations within the enum on that page. READ THEM.
typedef enum EFI_USBFN_MESSAGE
{
    EfiUsbMsgNone = 0,
    EfiUsbMsgSetupPacket,
    EfiUsbMsgEndpointStatusChangedRx,
    EfiUsbMsgEndpointStatusChangedTx,
    EfiUsbMsgBusEventDetach,
    EfiUsbMsgBusEventAttach,
    EfiUsbMsgBusEventReset,
    EfiUsbMsgBusEventSuspend,
    EfiUsbMsgBusEventResume,
    EfiUsbMsgBusEventSpeed
} EFI_USBFN_MESSAGE;

// Page 839 - UEFI Specs 2.8b
typedef enum EFI_USBFN_TRANSFER_STATUS
{
    UsbTransferStatusUnknown = 0,
    UsbTransferStatusComplete,
    UsbTransferStatusAborted,
    UsbTransferStatusActive,
    UsbTransferStatusNone
} EFI_USBFN_TRANSFER_STATUS;

// Page 839 - UEFI Specs 2.8b
typedef enum EFI_USB_BUS_SPEED
{
    UsbBusSpeedUnknown = 0,
    UsbBusSpeedLow,
    UsbBusSpeedFull,
    UsbBusSpeedHigh,
    UsbBusSpeedSuper,
    UsbBusSpeedMaximum = UsbBusSpeedSuper
} EFI_USB_BUS_SPEED;

// Page 846 - UEFI Specs 2.8b
typedef enum EFI_USBFN_POLICY_TYPE
{
    EfiUsbPolicyUndefined = 0,
    EfiUsbPolicyMaxTransactionSize,
    EfiUsbPolicyLengthTerminationSupport,
    EfiUsbPolicyLengthTermination
} EFI_USBFN_POLICY_TYPE;

// Page 839 - UEFI Specs 2.8b
typedef struct EFI_USBFN_TRANSFER_RESULT
{
    UINTN                         BytesTransferred;
    EFI_USBFN_TRANSFER_STATUS     TransferStatus;
    UINT8                         EndpointIndex;
    EFI_USBFN_ENDPOINT_DIRECTION  Direction;
    void                          *Buffer;
} EFI_USBFN_TRANSFER_RESULT;

// Page 839 - UEFI Specs 2.8b
typedef union EFI_USBFN_MESSAGE_PAYLOAD
{
    EFI_USB_DEVICE_REQUEST     udr;
    EFI_USBFN_TRANSFER_RESULT  utr;
    EFI_USB_BUS_SPEED          ubs;
} EFI_USBFN_MESSAGE_PAYLOAD;

// Page 829 - UEFI Specs 2.8b
typedef struct EFI_USB_INTERFACE_INFO
{
    EFI_USB_INTERFACE_DESCRIPTOR  *InterfaceDescriptor;
    EFI_USB_ENDPOINT_DESCRIPTOR   **EndpointDescriptorTable;
} EFI_USB_INTERFACE_INFO;

// Page 830 - UEFI Specs 2.8b
typedef struct EFI_USB_CONFIG_INFO
{
    EFI_USB_CONFIG_DESCRIPTOR  *ConfigDescriptor;
    EFI_USB_INTERFACE_INFO     **InterfaceInfoTable;
} EFI_USB_CONFIG_INFO;

// Page 830 - UEFI Specs 2.8b
typedef struct EFI_USB_DEVICE_INFO
{
    EFI_USB_DEVICE_DESCRIPTOR  *DeviceDescriptor;
    EFI_USB_CONFIG_INFO        **ConfigInfoTable;
} EFI_USB_DEVICE_INFO;

typedef EFI_STATUS (*EFI_USBFN_IO_DETECT_PORT)(struct EFI_USBFN_IO_PROTOCOL *This, EFI_USBFN_PORT_TYPE *PortType);
typedef EFI_STATUS (*EFI_USBFN_IO_CONFIGURE_ENABLE_ENDPOINTS)(struct EFI_USBFN_IO_PROTOCOL *This, EFI_USB_DEVICE_INFO *DeviceInfo);
typedef EFI_STATUS (*EFI_USBFN_IO_GET_ENDPOINT_MAXPACKET_SIZE)(struct EFI_USBFN_IO_PROTOCOL *This, EFI_USB_ENDPOINT_TYPE EndpointType, EFI_USB_BUS_SPEED BusSpeed, UINT16 *MaxPacketSize);
typedef EFI_STATUS (*EFI_USBFN_IO_GET_DEVICE_INFO)(struct EFI_USBFN_IO_PROTOCOL *This, EFI_USBFN_DEVICE_INFO_ID Id, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_USBFN_IO_GET_VENDOR_ID_PRODUCT_ID)(struct EFI_USBFN_IO_PROTOCOL *This, UINT16 *Vid, UINT16 *Pid);
typedef EFI_STATUS (*EFI_USBFN_IO_ABORT_TRANSFER)(struct EFI_USBFN_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USBFN_ENDPOINT_DIRECTION Direction);
typedef EFI_STATUS (*EFI_USBFN_IO_GET_ENDPOINT_STALL_STATE)(struct EFI_USBFN_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USBFN_ENDPOINT_DIRECTION Direction, BOOLEAN *State);
typedef EFI_STATUS (*EFI_USBFN_IO_SET_ENDPOINT_STALL_STATE)(struct EFI_USBFN_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USBFN_ENDPOINT_DIRECTION Direction, BOOLEAN State);
typedef EFI_STATUS (*EFI_USBFN_IO_EVENTHANDLER)(struct EFI_USBFN_IO_PROTOCOL *This, EFI_USBFN_MESSAGE *Message, UINTN *PayloadSize, EFI_USBFN_MESSAGE_PAYLOAD *Payload);
typedef EFI_STATUS (*EFI_USBFN_IO_TRANSFER)(struct EFI_USBFN_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USBFN_ENDPOINT_DIRECTION Direction, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_USBFN_IO_GET_MAXTRANSFER_SIZE)(struct EFI_USBFN_IO_PROTOCOL *This, UINTN *MaxTransferSize);
typedef EFI_STATUS (*EFI_USBFN_IO_ALLOCATE_TRANSFER_BUFFER)(struct EFI_USBFN_IO_PROTOCOL *This, UINTN Size, void **Buffer);
typedef EFI_STATUS (*EFI_USBFN_IO_FREE_TRANSFER_BUFFER)(struct EFI_USBFN_IO_PROTOCOL *This, void *Buffer);
typedef EFI_STATUS (*EFI_USBFN_IO_START_CONTROLLER)(struct EFI_USBFN_IO_PROTOCOL *This);
typedef EFI_STATUS (*EFI_USBFN_IO_STOP_CONTROLLER)(struct EFI_USBFN_IO_PROTOCOL *This);
typedef EFI_STATUS (*EFI_USBFN_IO_SET_ENDPOINT_POLICY)(struct EFI_USBFN_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USBFN_ENDPOINT_DIRECTION Direction, EFI_USBFN_POLICY_TYPE PolicyType, UINTN BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_USBFN_IO_GET_ENDPOINT_POLICY)(struct EFI_USBFN_IO_PROTOCOL *This, UINT8 EndpointIndex, EFI_USBFN_ENDPOINT_DIRECTION Direction, EFI_USBFN_POLICY_TYPE PolicyType, UINTN *BufferSize, void *Buffer);

// Page 825 - UEFI Specs 2.8b
typedef struct EFI_USBFN_IO_PROTOCOL
{
    UINT32                                     Revision;
    EFI_USBFN_IO_DETECT_PORT                   DetectPort;
    EFI_USBFN_IO_CONFIGURE_ENABLE_ENDPOINTS    ConfigureEnableEndpoints;
    EFI_USBFN_IO_GET_ENDPOINT_MAXPACKET_SIZE   GetEndpointMaxPacketSize;
    EFI_USBFN_IO_GET_DEVICE_INFO               GetDeviceInfo;
    EFI_USBFN_IO_GET_VENDOR_ID_PRODUCT_ID      GetVendorIdProductId;
    EFI_USBFN_IO_ABORT_TRANSFER                AbortTransfer;
    EFI_USBFN_IO_GET_ENDPOINT_STALL_STATE      GetEndpointStallState;
    EFI_USBFN_IO_SET_ENDPOINT_STALL_STATE      SetEndpointStallState;
    EFI_USBFN_IO_EVENTHANDLER                  EventHandler;
    EFI_USBFN_IO_TRANSFER                      Transfer;
    EFI_USBFN_IO_GET_MAXTRANSFER_SIZE          GetMaxTransferSize;
    EFI_USBFN_IO_ALLOCATE_TRANSFER_BUFFER      AllocateTransferBuffer;
    EFI_USBFN_IO_FREE_TRANSFER_BUFFER          FreeTransferBuffer;
    EFI_USBFN_IO_START_CONTROLLER              StartController;
    EFI_USBFN_IO_STOP_CONTROLLER               StopController;
    EFI_USBFN_IO_SET_ENDPOINT_POLICY           SetEndpointPolicy;
    EFI_USBFN_IO_GET_ENDPOINT_POLICY           GetEndpointPolicy;
} EFI_USBFN_IO_PROTOCOL;

// Partition Info
// Page 114 - UEFI Specs 2.8b
#pragma pack(1)
// MBR Partition Entry
typedef struct MBR_PARTITION_RECORD
{
    UINT8    BootIndicator;
    UINT8    StartHead;
    UINT8    StartSector;
    UINT8    StartTrack;
    UINT8    OSIndicator;
    UINT8    EndHead;
    UINT8    EndSector;
    UINT8    EndTrack;
    UINT8    StartingLBA[4];
    UINT8    SizeInLBA[4];
} MBR_PARTITION_RECORD;

typedef struct MASTER_BOOT_RECORD
{
    UINT8                  BootStrapCode[440];
    UINT8                  UniqueMbrSignature[4];
    UINT8                  Unknown[2];
    MBR_PARTITION_RECORD   Partition[4];
    UINT16                 Signature;
} MASTER_BOOT_RECORD;
#pragma pack()

#pragma pack(1)
// GPT Partition Entry
// Page 123 - UEFI Specs 2.8b
typedef struct EFI_PARTITION_ENTRY
{
    EFI_GUID     PartitionTypeGUID;
    EFI_GUID     UniquePartitionGUID;
    EFI_LBA      StartingLBA;
    EFI_LBA      EndingLBA;
    UINT64       Attributes;
    CHAR16       PartitionName[36];
} EFI_PARTITION_ENTRY;
#pragma pack()

// Page 126 - UEFI Specs 2.8b
#define EFI_BTT_ALIGNMENT                 4096
#define EFI_BTT_INFO_UNUSED_LEN           3968
#define EFI_BTT_INFO_BLOCK_SIG_LEN        16
#define EFI_BTT_INFO_BLOCK_FLAGS_ERROR    0x00000001
#define EFI_BTT_INFO_BLOCK_MAJOR_VERSION  2
#define EFI_BTT_INFO_BLOCK_MINOR_VERSION  0

// Page 126 - UEFI Specs 2.8b
typedef struct EFI_BTT_INFO_BLOCK
{
    CHAR8       sig[EFI_BTT_INFO_BLOCK_SIG_LEN];
    EFI_GUID    Uuid;
    EFI_GUID    ParentUuid;
    UINT32      Flags;
    UINT16      Major;
    UINT16      Minor;
    UINT32      ExternalLbaSize;
    UINT32      ExternalNLba;
    UINT32      InternalLbaSize;
    UINT32      InternalNLba;
    UINT32      NFree;
    UINT32      InfoSize;
    UINT64      NextOff;
    UINT64      DataOff;
    UINT64      MapOff;
    UINT64      FlogOff;
    UINT64      InfoOff;
    CHAR8       Unused[EFI_BTT_INFO_UNUSED_LEN];
    UINT64      Checksum;
} EFI_BTT_INFO_BLOCK;

// Page 129 - UEFI Specs 2.8b
typedef struct EFI_BTT_MAP_ENTRY
{
    UINT32    PostMapLba : 30;
    UINT32    Error : 1;
    UINT32    Zero : 1;
} EFI_BTT_MAP_ENTRY;

#define EFI_BTT_FLOG_ENTRY_ALIGNMENT 64

typedef struct EFI_BTT_FLOG
{
    UINT32    Lba0;
    UINT32    OldMap0;
    UINT32    NewMap0;
    UINT32    Seq0;
    UINT32    Lba1;
    UINT32    OldMap1;
    UINT32    NewMap1;
    UINT32    Seq1;
} EFI_BTT_FLOG;

// PCI
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_POLL_IO_MEM)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width, UINT64 Address, UINT64 Mask, UINT64 Value, UINT64 Delay, UINT64 *Result);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width, UINT64 Address, UINTN Count, void *Buffer);

typedef struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM  Read;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM  Write;
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS;

typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_COPY_MEM)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width, UINT64 DestAddress, UINT64 SrcAddress, UINTN Count);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_MAP)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION Operation, void *HostAddress, UINTN *NumberOfBytes, EFI_PHYSICAL_ADDRESS *DeviceAddress, void **Mapping);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_UNMAP)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, void *Mapping);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ALLOCATE_BUFFER)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, EFI_ALLOCATE_TYPE Type, EFI_MEMORY_TYPE MemoryType, UINTN Pages, void **HostAddress, UINT64 Attributes);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FREE_BUFFER)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, UINTN Pages, void *HostAddress);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FLUSH)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GET_ATTRIBUTES)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, UINT64 *Supports, UINT64 *Attributes);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_SET_ATTRIBUTES)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, UINT64 Attributes, UINT64 *ResourceBase, UINT64 *ResourceLength);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_CONFIGURATION)(struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This, void **Resources);

// Page 648 - UEFI Specs 2.8b
typedef struct EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
{
    EFI_HANDLE                                        ParentHandle;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_POLL_IO_MEM       PollMem;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_POLL_IO_MEM       PollIo;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS            Mem;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS            Io;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS            Pci;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_COPY_MEM          CopyMem;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_MAP               Map;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_UNMAP             Unmap;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ALLOCATE_BUFFER   AllocateBuffer;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FREE_BUFFER       FreeBuffer;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FLUSH             Flush;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GET_ATTRIBUTES    GetAttributes;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_SET_ATTRIBUTES    SetAttributes;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_CONFIGURATION     Configuration;
    UINT32                                            SegmentNumber;
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL;

typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_POLL_IO_MEM)(struct EFI_PCI_IO_PROTOCOL *This, EFI_PCI_IO_PROTOCOL_WIDTH Width, UINT8 BarIndex, UINT64 Offset, UINT64 Mask, UINT64 Value, UINT64 Delay, UINT64 *Result);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_MEM)(struct EFI_PCI_IO_PROTOCOL *This, EFI_PCI_IO_PROTOCOL_WIDTH Width, UINT8 BarIndex, UINT64 Offset, UINTN Count, void *Buffer);

typedef struct EFI_PCI_IO_PROTOCOL_ACCESS
{
    EFI_PCI_IO_PROTOCOL_MEM  Read;
    EFI_PCI_IO_PROTOCOL_MEM  Write;
} EFI_PCI_IO_PROTOCOL_ACCESS;

typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_CONFIG)(struct EFI_PCI_IO_PROTOCOL *This, EFI_PCI_IO_PROTOCOL_WIDTH Width, UINT32 Offset, UINTN Count, void *Buffer);

typedef struct EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS
{
    EFI_PCI_IO_PROTOCOL_CONFIG  Read;
    EFI_PCI_IO_PROTOCOL_CONFIG  Write;
} EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS;

typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_COPY_MEM)(struct EFI_PCI_IO_PROTOCOL *This, EFI_PCI_IO_PROTOCOL_WIDTH Width, UINT8 DestBarIndex, UINT64 DestOffset, UINT8 SrcBarIndex, UINT64 SrcOffset, UINTN Count);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_MAP)(struct EFI_PCI_IO_PROTOCOL *This, EFI_PCI_IO_PROTOCOL_OPERATION Operation, void *HostAddress, UINTN *NumberOfBytes, EFI_PHYSICAL_ADDRESS *DeviceAddress, void **Mapping);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_UNMAP)(struct EFI_PCI_IO_PROTOCOL *This, void *Mapping);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_ALLOCATE_BUFFER)(struct EFI_PCI_IO_PROTOCOL *This, EFI_ALLOCATE_TYPE Type, EFI_MEMORY_TYPE MemoryType, UINTN Pages, void *HostAddress, UINT64 Attributes);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_FREE_BUFFER)(struct EFI_PCI_IO_PROTOCOL *This, UINTN Pages, void *HostAddress);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_FLUSH)(struct EFI_PCI_IO_PROTOCOL *This);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_GET_LOCATION)(struct EFI_PCI_IO_PROTOCOL *This, UINTN *SegmentNumber, UINTN *BusNumber, UINTN *DeviceNumber, UINTN *FunctionNumber);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_ATTRIBUTES)(struct EFI_PCI_IO_PROTOCOL *This, EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION Operation, UINT64 Attributes, UINT64 *Result);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_GET_BAR_ATTRIBUTES)(struct EFI_PCI_IO_PROTOCOL *This, UINT8 BarIndex, UINT64 *Supports, void **Resources);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_SET_BAR_ATTRIBUTES)(struct EFI_PCI_IO_PROTOCOL *This, UINT64 Attributes, UINT8 BarIndex, UINT64 *Offset,  UINT64 *Length);

// Page 687 - UEFI Specs 2.8b
typedef struct EFI_PCI_IO_PROTOCOL
{
    EFI_PCI_IO_PROTOCOL_POLL_IO_MEM           PollMem;
    EFI_PCI_IO_PROTOCOL_POLL_IO_MEM           PollIo;
    EFI_PCI_IO_PROTOCOL_ACCESS                Mem;
    EFI_PCI_IO_PROTOCOL_ACCESS                Io;
    EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS         Pci;
    EFI_PCI_IO_PROTOCOL_COPY_MEM              CopyMem;
    EFI_PCI_IO_PROTOCOL_MAP                   Map;
    EFI_PCI_IO_PROTOCOL_UNMAP                 Unmap;
    EFI_PCI_IO_PROTOCOL_ALLOCATE_BUFFER       AllocateBuffer;
    EFI_PCI_IO_PROTOCOL_FREE_BUFFER           FreeBuffer;
    EFI_PCI_IO_PROTOCOL_FLUSH                 Flush;
    EFI_PCI_IO_PROTOCOL_GET_LOCATION          GetLocation;
    EFI_PCI_IO_PROTOCOL_ATTRIBUTES            Attributes;
    EFI_PCI_IO_PROTOCOL_GET_BAR_ATTRIBUTES    GetBarAttributes;
    EFI_PCI_IO_PROTOCOL_SET_BAR_ATTRIBUTES    SetBarAttributes;
    UINT64                                    RomSize;
    void                                      *RomImage;
} EFI_PCI_IO_PROTOCOL;

// ACPI
typedef EFI_STATUS (*EFI_ACPI_TABLE_INSTALL_ACPI_TABLE)(struct EFI_ACPI_TABLE_PROTOCOL *This, void *AcpiTableBuffer, UINTN AcpiTableBufferSize, UINTN *TableKey);
typedef EFI_STATUS (*EFI_ACPI_TABLE_UNINSTALL_ACPI_TABLE)(struct EFI_ACPI_TABLE_PROTOCOL *This, UINTN TableKey);

// Page 894 - UEFI Specs 2.8b
typedef struct EFI_ACPI_TABLE_PROTOCOL
{
    EFI_ACPI_TABLE_INSTALL_ACPI_TABLE    InstallAcpiTable;
    EFI_ACPI_TABLE_UNINSTALL_ACPI_TABLE  UninstallAcpiTable;
} EFI_ACPI_TABLE_PROTOCOL;