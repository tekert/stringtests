bits 64
default rel

segment .data
    msg db "Hello ASM world!", 0xd, 0xa, 0

segment .text
global hello_from_asm

extern puts

hello_from_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    lea     rcx, [msg]
    call    puts

    xor     rax, rax
    leave
    ret