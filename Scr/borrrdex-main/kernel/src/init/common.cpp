#include "common.h"
#include "graphics/BasicRenderer.h"
#include "Panic.h"
#include "fs/vfs.h"
#include "proc/process.h"
#include "io/serial.h"

void init_startup_thread(uint32_t arg) {
    arg = arg;

    uart_print("Starting initial program (usertest).\r\n");

    vfs_mount_all();

    process_start("[disk]/usertest", nullptr);

    Panic("Should never return from initprog");
    __builtin_unreachable();
}