#ifndef _MODULE_AHCI_CONTROLLER_H
#define _MODULE_AHCI_CONTROLLER_H

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000
#define HBA_PORT_IPM_ACTIVE 0x1
#define HBA_PORT_DEV_PRESENT 0x3

#define PORT_MAX_COUNT 0x20

#define PORT_TYPE_NONE                  (0)
#define PORT_TYPE_SATA                  (1)
#define PORT_TYPE_SATAPI                (2)
#define PORT_TYPE_SEMB                  (3)
#define PORT_TYPE_PM                    (4)

#include <port.h>
#include <stdint.h>


typedef struct{
    uint64_t command_list_base;
    uint64_t fis_base_address;
    uint32_t interrupt_status;
    uint32_t interrupt_enable;
    uint32_t command_status;
    uint32_t reserved0;
    uint32_t task_file_data;
    uint32_t signature;
    uint32_t sata_status;
    uint32_t sata_control;
    uint32_t sata_error;
    uint32_t sata_active;
    uint32_t command_issue;
    uint32_t sata_notification;
    uint32_t fis_switch_control;
    uint32_t reserved1[11];
    uint32_t vendor[4];
}__attribute__((packed)) hba_port_t;

typedef struct{
    uint32_t host_capability;
    uint32_t global_host_control;
    uint32_t interrupt_status;
    uint32_t ports_implemented;
    uint32_t version;
    uint32_t ccc_control;
    uint32_t ccc_ports;
    uint32_t enclosure_management_location;
    uint32_t enclosure_management_control;
    uint32_t host_capabilities_extended;
    uint32_t bios_handoff_ctrl_status;
    uint8_t reserved0[0x74];
    uint8_t vendor[0x60];
    hba_port_t ports[];
}__attribute__((packed)) hba_memory_t;

typedef struct{
    hba_memory_t* abar;
    uint8_t port_count;
    struct device_t* devices[PORT_MAX_COUNT];
}ahci_controller_t;

#endif // _MODULE_AHCI_CONTROLLER_H