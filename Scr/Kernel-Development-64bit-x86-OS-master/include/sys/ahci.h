#ifndef _AHCI_H
#define _AHCI_H
#include <sys/defs.h>
#define HBA_GHC_AE     (1U << 31)
#define HBA_GHC_IE     (1U << 1)
#define HBA_GHC_HR     (1U)

#define HBA_PxCMD_ST   (1U)
#define HBA_PxCMD_CLO  (1U << 3)
#define HBA_PxCMD_FRE  (1U << 4)
#define HBA_PxCMD_FR   (1U << 14)
#define HBA_PxCMD_CR   (1U << 15)
#define HBA_PxIS_TFES  (1U << 30)

#define AHCI_DEV_SATA   0x00000101  // SATA drive
#define AHCI_DEV_SATAPI 0xEB140101  // SATAPI drive
#define AHCI_DEV_SEMB   0xC33C0101  // Enclosure management bridge
#define AHCI_DEV_PM     0x96690101  // Port multiplier

#define ATA_STATUS_ERR  0x01 // Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).
#define ATA_STATUS_DRQ  0x08 // Set when the drive has PIO data to transfer, or is ready to accept PIO data.
#define ATA_STATUS_SRV  0x10 // Overlapped Mode Service Request.
#define ATA_STATUS_DF   0x20 // Drive Fault Error (does not set ERR).
#define ATA_STATUS_RDY  0x40 // Bit is clear when drive is spun down, or after an error. Set otherwise.
#define ATA_STATUS_BSY  0x80 // Indicates the drive is preparing to send/receive data (wait for it to clear).
                             // In case of 'hang' (it never clears), do a software reset.

#define CMD_FIS_DEV_LBA (1U << 6)

#define MAX_CMD_SLOT_CNT 32
#define MAX_PORT_CNT     32


typedef enum {
  FIS_TYPE_REG_H2D = 0x27,   // Register FIS - host to device
  FIS_TYPE_REG_D2H = 0x34,   // Register FIS - device to host
  FIS_TYPE_DMA_ACT = 0x39,   // DMA activate FIS - device to host
  FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS - bidirectional
  FIS_TYPE_DATA = 0x46,      // Data FIS - bidirectional
  FIS_TYPE_BIST = 0x58,      // BIST activate FIS - bidirectional
  FIS_TYPE_PIO_SETUP = 0x5F, // PIO setup FIS - device to host
  FIS_TYPE_DEV_BITS = 0xA1,  // Set device bits FIS - device to host
} fis_type_t;

/* Register Host to Device FIS */
typedef struct {
  // DWORD 0
  uint8_t fis_type;          // FIS_TYPE_REG_H2D

  uint8_t pmport:4;          // Port multiplier
  uint8_t rsv0:3;            // Reserved
  uint8_t c:1;               // 1: Command, 0: Control

  uint8_t command;           // Command register
  uint8_t featurel;          // Feature register, 7:0

  // DWORD 1
  uint8_t lba0;              // LBA low register, 7:0
  uint8_t lba1;              // LBA mid register, 15:8
  uint8_t lba2;              // LBA high register, 23:16
  uint8_t device;            // Device register

  // DWORD 2
  uint8_t lba3;              // LBA register, 31:24
  uint8_t lba4;              // LBA register, 39:32
  uint8_t lba5;              // LBA register, 47:40
  uint8_t featureh;          // Feature register, 15:8

  // DWORD 3
  uint8_t countl;
  uint8_t counth;  

  uint16_t count;            // Count register
  uint8_t icc;               // Isochronous command completion
  uint8_t control;           // Control register

  // DWORD 4
  uint16_t aux;
  uint8_t rsv1[2];           // Reserved
}__attribute__((__packed__)) fis_reg_h2d_t;

/* Register Device to Host FIS */
typedef struct {
  // DWORD 0
  uint8_t fis_type;          // FIS_TYPE_REG_D2H

  uint8_t pmport:4;          // Port multiplier
  uint8_t rsv0:2;            // Reserved
  uint8_t i:1;               // Interrupt bit
  uint8_t rsv1:1;            // Reserved

  uint8_t status;            // Status register
  uint8_t error;             // Error register

  // DWORD 1
  uint8_t lba0;              // LBA low register, 7:0
  uint8_t lba1;              // LBA mid register, 15:8
  uint8_t lba2;              // LBA high register, 23:16
  uint8_t device;            // Device register

  // DWORD 2
  uint8_t lba3;              // LBA register, 31:24
  uint8_t lba4;              // LBA register, 39:32
  uint8_t lba5;              // LBA register, 47:40
  uint8_t rsv2;              // Reserved

  // DWORD 3
  uint16_t count;            // Count register
  uint8_t rsv3[2];           // Reserved

  // DWORD 4
  uint8_t rsv4[4];           // Reserved
}__attribute__((__packed__)) fis_reg_d2h_t;

/* Set Device Bits - Device to Host FIS */
typedef struct {
  // DWORD 0
  uint8_t fis_type;          // FIS_TYPE_DEV_BITS

  uint8_t pmport:4;          // Port multiplier
  uint8_t rsv0:2;            // Reserved
  uint8_t i:1;               // Interrupt bit
  uint8_t n:1;               // Notification bit

  uint8_t statusl:3;
  uint8_t rsv1:1;            // Reserved
  uint8_t statush:3;
  uint8_t rsv2:1;

  uint8_t error;

  // DWORD 1
  uint32_t protocol;         // protocol specific
}__attribute__((__packed__)) fis_dev_bits_t;

/* DMA Activate - Device to Host (skip) */

/* DMA Setup - Device to Host FIS or Host to Device FIS (Bidirectional) */
typedef struct {
  // DWORD 0
  uint8_t fis_type;          // FIS_TYPE_DMA_SETUP

  uint8_t pmport:4;          // Port multiplier
  uint8_t rsv0:1;            // Reserved
  uint8_t d:1;               // Data transfer direction, 1 - device to host
  uint8_t i:1;               // Interrupt bit
  uint8_t a:1;               // Auto-activate. Specifies if DMA Activate FIS is needed

  uint8_t rsved[2];          // Reserved

  //DWORD 1&2
  uint64_t buf_id;           // DMA Buffer Identifier.

  //DWORD 3
  uint32_t rsv1;             // More reserved

  //DWORD 4
  uint32_t buf_offset;       // Byte offset into buffer. First 2 bits must be 0

  //DWORD 5
  uint32_t trans_count;      // Number of bytes to transfer. Bit 0 must be 0

  //DWORD 6
  uint32_t rsv2;             // Reserved

}__attribute__((__packed__)) fis_dma_setup_t;

/* BIST Activate FIS - Bidirectional (skip) */

/* PIO Setup - Device to Host FIS */
typedef struct {
  // DWORD 0
  uint8_t fis_type;          // FIS_TYPE_PIO_SETUP

  uint8_t pmport:4;          // Port multiplier
  uint8_t rsv0:1;            // Reserved
  uint8_t d:1;               // Data transfer direction, 1 - device to host
  uint8_t i:1;               // Interrupt bit
  uint8_t rsv1:1;

  uint8_t status;            // Status register
  uint8_t error;             // Error register

  // DWORD 1
  uint8_t lba0;              // LBA low register, 7:0
  uint8_t lba1;              // LBA mid register, 15:8
  uint8_t lba2;              // LBA high register, 23:16
  uint8_t device;            // Device register

  // DWORD 2
  uint8_t lba3;              // LBA register, 31:24
  uint8_t lba4;              // LBA register, 39:32
  uint8_t lba5;              // LBA register, 47:40
  uint8_t rsv2;              // Reserved

  // DWORD 3
  uint16_t count;            // Count register
  uint8_t rsv3;              // Reserved
  uint8_t e_status;          // New value of status register

  // DWORD 4
  uint16_t trans_count;      // Transfer count
  uint8_t rsv4[2];           // Reserved
}__attribute__((__packed__)) fis_pio_setup_t;

/* Data - Host to Device FIS or Device to Host FIS (Bidirectional) */
typedef struct {
  // DWORD 0
  uint8_t fis_type;          // FIS_TYPE_DATA

  uint8_t pmport:4;          // Port multiplier
  uint8_t rsv0:4;            // Reserved

  uint8_t rsv1[2];           // Reserved

  // DWORD 1 ~ 2048
  uint32_t data[1];          // Payload
}__attribute__((__packed__)) fis_data_t;

/* physical region descriptor table entry */
typedef struct {
  // DW0 & DW1
   uint64_t dba;              // Data base address
  // uint32_t	dba;		// Data base address
  // uint32_t	dbau;		// Data base address upper 32 bits
  // DW2
  uint32_t rsv0;             // Reserved
  // DW3
  uint32_t dbc:22;           // Byte count, 4M max
  uint32_t rsv1:9;           // Reserved
  uint32_t i:1;              // Interrupt on completion
}__attribute__((__packed__)) hba_prdt_entry_t;

/* Command Table */
typedef struct {
  // 0x00
  uint8_t cfis[64];          // Command FIS
  // 0x40
  uint8_t acmd[16];          // ATAPI command, 12 or 16 bytes
  // 0x50
  uint8_t rsv[48];           // Reserved
  // 0x80
  hba_prdt_entry_t prdt_entry[1]; // Physical region descriptor table entries, 0 ~ 65535
}__attribute__((__packed__)) __attribute__((aligned(128))) hba_cmd_tbl_t;

/* Command Header */
typedef struct {
  // DW0
  uint8_t cfl:5;             // Command FIS length in DWORDS, 2 ~ 16
  uint8_t a:1;               // ATAPI
  uint8_t w:1;               // Write, 1: H2D, 0: D2H
  uint8_t p:1;               // Prefetchable

  uint8_t r:1;               // Reset
  uint8_t b:1;               // BIST
  uint8_t c:1;               // Clear busy upon R_OK
  uint8_t rsv0:1;            // Reserved
  uint8_t pmp:4;             // Port multiplier port

  uint16_t prdtl;            // Physical region descriptor table length in entries

  // DW1
  volatile uint32_t prdbc;   // Physical region descriptor byte count transferred

  // DW2, 3
  uint64_t ctba;             // Command table descriptor base address
  //uint32_t ctbau;          // Command table descriptor base address upper 32 bits

  // DW4 - 7
  uint32_t rsv1[4];          // Reserved
}__attribute__((__packed__)) hba_cmd_header_t;

typedef struct {
  // 0x00
  fis_dma_setup_t dsfis;     // DMA Setup FIS
  uint8_t pad0[4];

  // 0x20
  fis_pio_setup_t psfis;     // PIO Setup FIS
  uint8_t pad1[12];

  // 0x40
  fis_reg_d2h_t rfis;        // Register - Device to Host FIS
  uint8_t pad2[4];

  // 0x58
  fis_dev_bits_t sdbfis;     // Set Device Bit FIS

  // 0x60
  uint8_t ufis[64];

  // 0xA0
  uint8_t rsv[96];
}__attribute__((__packed__)) __attribute__((aligned(256))) hba_fis_t;

typedef struct {
  hba_cmd_header_t headers[MAX_CMD_SLOT_CNT];
}__attribute__((__packed__))__attribute__((aligned(1024))) hba_cmd_list_t;

typedef volatile struct {
  uint64_t clb;              // 0x00, command list base address, 1K-byte aligned
  //uint32_t clbu;
  uint64_t fb;               // 0x08, FIS base address, 256-byte aligned
  uint32_t is_rwc;           // 0x10, interrupt status
  uint32_t ie;               // 0x14, interrupt enable
  uint32_t cmd;              // 0x18, command and status
  uint32_t rsv0;             // 0x1C, Reserved
  uint32_t tfd;              // 0x20, task file data
  uint32_t sig;              // 0x24, signature
  uint32_t ssts;             // 0x28, SATA status (SCR0:SStatus)
  uint32_t sctl;             // 0x2C, SATA control (SCR2:SControl)
  uint32_t serr_rwc;         // 0x30, SATA error (SCR1:SError)
  uint32_t sact;             // 0x34, SATA active (SCR3:SActive)
  uint32_t ci;               // 0x38, command issue
  uint32_t sntf_rwc;         // 0x3C, SATA notification (SCR4:SNotification)
  uint32_t fbs;              // 0x40, FIS-based switch control
  uint32_t rsv1[11];         // 0x44 ~ 0x6F, Reserved
  uint32_t vendor[4];        // 0x70 ~ 0x7F, vendor specific
}__attribute__((__packed__)) hba_port_t;

typedef volatile struct {
  // 0x00 - 0x2B, Generic Host Control
  uint32_t cap;              // 0x00, Host capability
  uint32_t ghc;              // 0x04, Global host control
  uint32_t is_rwc;           // 0x08, Interrupt status
  uint32_t pi;               // 0x0C, Port implemented
  uint32_t vs;               // 0x10, Version
  uint32_t ccc_ctl;          // 0x14, Command completion coalescing control
  uint32_t ccc_pts;          // 0x18, Command completion coalescing ports
  uint32_t em_loc;           // 0x1C, Enclosure management location
  uint32_t em_ctl;           // 0x20, Enclosure management control
  uint32_t cap2;             // 0x24, Host capabilities extended
  uint32_t bohc;             // 0x28, BIOS/OS handoff control and status

  // 0x2C - 0x9F, Reserved
  uint8_t rsv[0xA0 - 0x2C];

  // 0xA0 - 0xFF, Vendor specific registers
  uint8_t vendor[0x100 - 0xA0];

  // 0x100 - 0x10FF, Port control registers
  hba_port_t ports[MAX_PORT_CNT]; // 1 ~ 32
}__attribute__((__packed__)) hba_mem_t;

#endif
