[BITS 64]

GLOBAL IdleTask

IdleTask:
        int 0x40
        jmp IdleTask

times 4096 - ($ - $$) db 0