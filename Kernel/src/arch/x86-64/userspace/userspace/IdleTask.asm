[BITS 64]

GLOBAL IdleTask

IdleTask:
    jmp IdleTask

times 4096 - ($ - $$) db 0