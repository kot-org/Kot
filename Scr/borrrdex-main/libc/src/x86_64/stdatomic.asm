[bits 64]

GLOBAL atomic_exchange_u8
GLOBAL atomic_exchange_i8
GLOBAL atomic_exchange_u16
GLOBAL atomic_exchange_i16
GLOBAL atomic_exchange_u32
GLOBAL atomic_exchange_i32
GLOBAL atomic_exchange_u64
GLOBAL atomic_exchange_i64

GLOBAL atomic_increment_u8
GLOBAL atomic_increment_i8
GLOBAL atomic_increment_u16
GLOBAL atomic_increment_i16
GLOBAL atomic_increment_u32
GLOBAL atomic_increment_i32
GLOBAL atomic_increment_u64
GLOBAL atomic_increment_i64

atomic_exchange_u8:
atomic_exchange_i8:
    mov ax, si
    xchg [rdi], al
    ret

atomic_exchange_u16:
atomic_exchange_i16:
    mov ax, si
    xchg [rdi], ax
    ret

atomic_exchange_u32:
atomic_exchange_i32:
    mov eax, esi
    xchg [rdi], eax
    ret

atomic_exchange_u64:
atomic_exchange_i64:
    mov rax, rsi
    xchg [rdi], rax
    ret

atomic_increment_u8:
atomic_increment_i8:
    mov ax, word 1
    lock xadd [rdi], al
    ret

atomic_increment_u16:
atomic_increment_i16:
    mov ax, word 1
    lock xadd [rdi], ax
    ret

atomic_increment_u32:
atomic_increment_i32:
    mov eax, dword 1
    lock xadd [rdi], eax
    ret

atomic_increment_u64:
atomic_increment_i64:
    mov rax, qword 1
    lock o64 xadd [rdi], al
    ret