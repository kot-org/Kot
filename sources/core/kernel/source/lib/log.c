#include <lib/assert.h>
#include <lib/log.h>
#include <lib/lock.h>
#include <lib/printf.h>

#include <impl/serial.h>

#include <stdint.h>
#include <global/console.h>

void log_print(const char *str) {
    static spinlock_t print_lock = SPINLOCK_INIT;
    assert(!spinlock_acquire(&print_lock));
    console_print(str);
    spinlock_release(&print_lock);
}

void log_printv(const char *fmt, va_list args) {
    char buffer[1024];
    vsnprintf_(buffer, sizeof(buffer), fmt, args);
    log_print(buffer);
}

void log_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_printv(fmt, args);
    va_end(args);
}

void putchar_(char _) {
    (void)_;
}


void log_count(void){
    static int count = 0;
    log_printf("[%d]", count);
    count++;
}