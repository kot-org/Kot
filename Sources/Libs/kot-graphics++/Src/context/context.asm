GLOBAL blendAlpha

blendAlpha:     ; 0AAxxxxxxx
    mov         ecx, esi
	shr	        ecx, 24

	pxor		mm5, mm5

	movd	    mm6, ecx
    pshufw	    mm6, mm6, 0

	add	        ecx, 256
	movd	    mm7, ecx
    pshufw	    mm7, mm7, 0


	movd		mm0, [rdi]
	movd		mm1, esi
	punpcklbw	mm0, mm5
	punpcklbw	mm1, mm5
	pmullw		mm0, mm6
	pmullw		mm1, mm7
	paddusw		mm0, mm1
	psrlw		mm0, 8
	packuswb	mm0, mm0
	movd		[rdi], mm0
	ret