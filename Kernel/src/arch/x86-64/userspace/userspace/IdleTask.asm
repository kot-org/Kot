[BITS 64]
ALIGN 4096

GLOBAL IdleTask

IdleTask:
    jmp IdleTask

times 4096 - ($ - $$) db 0