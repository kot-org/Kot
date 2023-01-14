GLOBAL blendAlpha

blendAlpha:     ; 0xRRGGBBAA
    movzx ecx, sil
    mov eax, [rdi]

    and eax, 0x00FF00FF
    and eax, 0x0000FF00

    mov edx, ecx
    shr edx, 8
    imul eax, edx

    mov edx, 255
    sub edx, ecx
    imul esi, edx
    add eax, esi

    mov edx, 0xff
    and eax, edx
    shr eax, 8

    mov [rdi], eax
    ret