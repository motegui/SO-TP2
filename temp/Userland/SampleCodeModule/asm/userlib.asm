GLOBAL divideByZero
GLOBAL invalidOpcode
GLOBAL fillRegs

divideByZero:
    mov rax, 3
    mov rbx, 0
    mov rcx, 4
    mov rdx, 5
    mov rsi, 6
    mov rdi, 7
    mov rbp, rsp
    mov r8, 9
    mov r9, 10
    mov r10, 11
    mov r11, 12
    mov r12, 13
    mov r13, 14
    mov r14, 15
    mov r15, 16
    div rbx
    ret

invalidOpcode:
    mov cr6, rax
    ret


%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

fillRegs:
    pushState
    mov rax, 0xFFFFFFFF
    mov rbx, 0xFF000000
    mov rcx, 0xEEEEEEEE
    mov rdx, 0xDDDDDDDD
    mov rsi, 0xCCCCCCCC
    mov rdi, 0xBBBBBBBB
    mov rbp, rsp
    mov r8,  0xAAAAAAAA
    mov r9,  0x99999999
    mov r10, 0x88888888
    mov r11, 0x77777777
    mov r12, 0x77770000
    mov r13, 0x77000000
    mov r14, 0x55566666
    mov r15, 0x26666666

.decr:
    dec r15
    jz .end
    jmp .decr

.end:
    popState
    ret