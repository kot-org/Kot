#include <lib/log.h>
#include <lib/lock.h>
#include <lib/printf.h>

#include <impl/serial.h>

#include <stdint.h>
#include <global/term.h>

void log_print(const char *str) {
    static spinlock_t print_lock = SPINLOCK_INIT;
    spinlock_acquire(&print_lock);
    print_terminal(str);
    for (uint64_t i = 0; str[i] != '\0'; i++) {
        serial_write(str[i]);
        if (str[i] == '\n') serial_write('\r');   
    }
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
