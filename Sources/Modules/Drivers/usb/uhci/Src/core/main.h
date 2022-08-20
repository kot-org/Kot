#pragma once

/* I/O Registers */
#define REG_USB_COMMAND     0x0
#define REG_USB_STATUS      0x2
#define REG_USB_INTR        0x4 /* Usb interrupt enable */
#define REG_FRAME_NUM       0x6
#define REG_FRAME_BASE_ADDR 0x8
#define REG_SOFMOD          0xC
#define REG_PORT1           0x10 /* Port 1 Status/Control */
#define REG_PORT2           0x12 /* Port 2 Status/Control */

/* Command Register */
#define CMD_RS          1 << 0 /* Run/Stop */
#define CMD_HCRST       1 << 1 /* Host controller reset */
#define CMD_GRST        1 << 2 /* Global reset */
#define CMD_GSPD        1 << 3 /* Global suspend */
#define CMD_GRSM        1 << 4 /* Global resume */
#define CMD_STWDBG      1 << 5 /* Software debug */
#define CMD_CFG         1 << 6 /* Configure */
#define CMD_MAXP        1 << 7 /* Max packet */

/* Status register */
#define STS_INTR        1 << 0 /* Interrupt */
#define STS_ERRINTR     1 << 1 /* Error interrupt */
#define STS_RSMDTD      1 << 2 /* Resume detected */
#define STS_SYSERR      1 << 3 /* System error */
#define STS_PROCERR     1 << 4 /* Process error */
#define STS_HLT         1 << 5 /* Halted */

/* Interrupt enable register */
/* 
 * 1 = enable interrupt
 * 0 = disable interrupt  
*/
#define INTR_TIMEOUT    1 << 0
#define INTR_RSM        1 << 1 /* Resume */
#define INTR_CPLT       1 << 2 /* Complete */
#define INTR_SHRTPKT    1 << 3 /* Short packet */   


#include <kot/sys.h>