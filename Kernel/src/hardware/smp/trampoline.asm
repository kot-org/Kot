BITS 16
GLOBAL ap_trampoline

ap_trampoline: 
    mov bx, 0x0
	mov [bx], byte 0x2 ; just a test to see if we are actualy alive
	hlt

times 4096 - ($ - $$) db 0