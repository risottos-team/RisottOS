
bits 64
section .text

%macro PUSH_REGS 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POP_REGS 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%assign i 0
%rep 32
    global isr %+ i
    %assign i i + 1
%endrep

extern exception_error_handler

; Exceptions without CPU error code
%macro ISR_NO_ERROR 1
isr%1:
    push 0
    push %1
    jmp isr_common
%endmacro

; Exceptions with CPU error code
%macro ISR_ERROR 1
isr%1:
    push %1
    jmp isr_common
%endmacro

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7

ISR_ERROR 8

ISR_NO_ERROR 9

ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14

ISR_NO_ERROR 15
ISR_NO_ERROR 16

ISR_ERROR 17

ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20

ISR_ERROR 21

ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28

ISR_ERROR 29
ISR_ERROR 30

ISR_NO_ERROR 31

isr_common:
    PUSH_REGS

    mov r12, rsp

    ; Exception vector
    mov rdi, [r12 + 120]

    ; Error code
    mov rsi, [r12 + 128]

    cld
    and rsp, -16

    call exception_error_handler

    mov rsp, r12

    POP_REGS

    add rsp, 16
    iretq
