section .text
GLOBAL sys_call

sys_call:
    push rbp
    mov rbp, rsp
    int 80h

    mov rsp, rbp
    pop rbp
    ret