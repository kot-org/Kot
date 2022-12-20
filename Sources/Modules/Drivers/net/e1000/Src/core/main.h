#pragma once

#define INTEL_VEND     0x8086  // Vendor ID for Intel 
#define E1000_DEV      0x100E  // Device ID for the e1000 Qemu, Bochs, and VirtualBox emmulated NICs
#define E1000_I217     0x153A  // Device ID for Intel I217
#define E1000_82577LM  0x10EA  // Device ID for Intel 82577LM
 
#define REG_CTRL        0x0000
#define REG_STATUS      0x0008
#define REG_EEPROM      0x0014
#define REG_CTRL_EXT    0x0018
#define REG_IMASK       0x00D0
#define REG_RCTRL       0x0100
#define REG_RXDESC_LOW  0x2800
#define REG_RXDESC_HIGH 0x2804
#define REG_RXDESC_LEN  0x2808
#define REG_RXDESC_HEAD 0x2810
#define REG_RXDESC_TAIL 0x2818
 
#define REG_TCTRL       0x0400
#define REG_TXDESC_LOW  0x3800
#define REG_TXDESC_HIGH 0x3804
#define REG_TXDESC_LEN  0x3808
#define REG_TXDESC_HEAD 0x3810
#define REG_TXDESC_TAIL 0x3818
 
#define REG_RDTR         0x2820 // RX Delay Timer Register
#define REG_RXDCTL       0x3828 // RX Descriptor Control
#define REG_RADV         0x282C // RX Int. Absolute Delay Timer
#define REG_RSRPD        0x2C00 // RX Small Packet Detect Interrupt
#define REG_MAC          0x5400
 
#define REG_TIPG         0x0410      // Transmit Inter Packet Gap
#define ECTRL_SLU        0x40        //set link up
 
// Buffer Sizes
#define RCTL_BSIZE_256                  (3 << 16)
#define RCTL_BSIZE_512                  (2 << 16)
#define RCTL_BSIZE_1024                 (1 << 16)
#define RCTL_BSIZE_2048                 (0 << 16)
#define RCTL_BSIZE_4096                 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192                 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384                ((1 << 16) | (1 << 25))
 
// TCTL Register
#define TSTA_DD                         (1 << 0)    // Descriptor Done

#include <kot/sys.h>

#include <kot/uisd/srvs/pci.h>