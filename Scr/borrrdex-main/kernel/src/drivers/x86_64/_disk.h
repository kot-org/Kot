#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "drivers/disk.h"
#include <cstdint>

constexpr uint8_t IDE_CHANNELS_PER_CTRL     = 0x2;
constexpr uint8_t IDE_DEVICES_PER_CHANNEL   = 0x2;

constexpr uint16_t IDE_PRIMARY_CMD_BASE     = 0x1F0;
constexpr uint16_t IDE_PRIMARY_CTRL_BASE    = 0x3F6;
constexpr uint8_t IDE_PRIMARY_IRQ           = 14;

constexpr uint16_t IDE_SECONDARY_CMD_BASE   = 0x170;
constexpr uint16_t IDE_SECONDARY_CTRL_BASE  = 0x376;
constexpr uint8_t IDE_SECONDARY_IRQ         = 15;

constexpr uint8_t IDE_PRIMARY               = 0;
constexpr uint8_t IDE_SECONDARY             = 1;

constexpr uint8_t IDE_REGISTER_DATA         = 0x00;
constexpr uint8_t IDE_REGISTER_FEATURES     = 0x01;
constexpr uint8_t IDE_REGISTER_SECCOUNT0    = 0x02;
constexpr uint8_t IDE_REGISTER_LBA0         = 0x03;
constexpr uint8_t IDE_REGISTER_LBA1         = 0x04;
constexpr uint8_t IDE_REGISTER_LBA2         = 0x05;
constexpr uint8_t IDE_REGISTER_HDDSEL       = 0x06;
constexpr uint8_t IDE_REGISTER_COMMAND      = 0x07; // Write only
constexpr uint8_t IDE_REGISTER_STATUS       = 0x07; // Read only
constexpr uint8_t IDE_REGISTER_CTRL         = 0x0C;

constexpr uint8_t IDE_COMMAND_PIO_READ      = 0x20;
constexpr uint8_t IDE_COMMAND_PIO_WRITE     = 0x30;
constexpr uint8_t IDE_COMMAND_PACKET        = 0xA1;
constexpr uint8_t IDE_COMMAND_FLUSH         = 0xE7;
constexpr uint8_t IDE_COMMAND_IDENTIFY      = 0xEC;

constexpr uint8_t IDE_ERROR_GENERIC         = 0x01;
constexpr uint8_t IDE_ERROR_DRQ             = 0x08;
constexpr uint8_t IDE_ERROR_FAULT           = 0x20;

constexpr uint8_t IDE_ATA_BUSY              = 0x80;
constexpr uint8_t IDE_ATA_DRQ               = 0x08;

constexpr uint8_t IDE_READ                  = 0x00;
constexpr uint8_t IDE_WRITE                 = 0x01;

typedef struct ide_channel {
    uint16_t base;
    uint16_t ctrl;
    uint16_t busm;
    uint8_t irq;

    uint32_t use_irq;
    uint32_t use_dma;

    volatile uint32_t irq_wait;

    uint32_t dma_phys;
    uint32_t dma_virt;
    uint32_t dma_buf_phys;
    uint32_t dma_buf_virt;
} ide_channel_t;

typedef struct ide_device {
    uint8_t present;
    uint8_t channel;
    uint8_t drive;

    uint8_t flags;

    uint16_t type;
    uint16_t signature;

    uint16_t capabilities;
    uint32_t commandset;

    uint16_t cylinders;
    uint16_t heads_per_cylinder;
    uint64_t secs_per_head;
    uint64_t total_sectors;
} ide_device_t;