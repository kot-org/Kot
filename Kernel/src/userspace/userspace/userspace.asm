[bits 64]

GLOBAL SaveTSS
EXTERN TSSSetStack

SaveTSS:
	mov		rsi, rsp
	call	TSSSetStack
	ret