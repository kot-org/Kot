// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef unsigned short      CHAR16;
typedef unsigned char       BOOL;

typedef void                *EFI_HANDLE;
typedef UINT64              EFI_STATUS;

typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_TEXT_RESET)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOL ExtendedVerification);
typedef EFI_STATUS (*EFI_TEXT_STRING)(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
{
    EFI_TEXT_RESET      Reset;
    EFI_TEXT_STRING     OutputString;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct EFI_TABLE_HEADER
{
    UINT64    Signature;
    UINT32    Revision;
    UINT32    HeaderSize;
    UINT32    CRC32;
    UINT32    Reserved;
} EFI_TABLE_HEADER;

typedef struct EFI_SYSTEM_TABLE
{
    EFI_TABLE_HEADER                hrd;
    CHAR16                          *FirmwareVendor;
    UINT32                          FirmwareVersion;
    EFI_HANDLE                      ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *ConIn;
    EFI_HANDLE                      ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
} EFI_SYSTEM_TABLE ;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Testing...\r\n");
    while(1){};
	return 0;
}
