[BITS 64]

GLOBAL IdleTask

IdleTask:
.wait:
        int 0x81
        jmp     .wait

times 4096 - ($ - $$) db 0