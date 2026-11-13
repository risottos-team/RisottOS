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

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13

isr0:
    push 0 ; Fake error code
    push 0 ; Exception num 0
    jmp isr_common

isr1:
    push 0 ; Fake error code
    push 1 ; Exception num 1
    jmp isr_common

isr2:
    push 0 ; Fake error code
    push 2 ; Exception num 2
    jmp isr_common

isr3:
    push 0 ; Fake error code
    push 3 ; Exception num 3
    jmp isr_common

isr4:
    push 0 ; Fake error code
    push 4 ; Exception num 4
    jmp isr_common

isr5:
    push 0 ; Fake error code
    push 5 ; Exception num 5
    jmp isr_common

isr6:
    push 0 ; Fake error code
    push 6 ; Exception num 6
    jmp isr_common

isr7:
    push 0 ; Fake error code
    push 7 ; Exception num 7
    jmp isr_common

isr8:
    push 0 ; Fake error code
    push 8 ; Exception num 8
    jmp isr_common

isr9:
    push 0 ; Fake error code
    push 9 ; Exception num 9
    jmp isr_common

isr10:
    push 0 ; Fake error code
    push 10 ; Exception num 10
    jmp isr_common

isr11:
    push 0 ; Fake error code
    push 11 ; Exception num 11
    jmp isr_common

isr12:
    push 0 ; Fake error code
    push 12 ; Exception num 12
    jmp isr_common

isr13:
    push 0 ; Fake error code
    push 13 ; Exception num 13
    jmp isr_common

isr_common:
    PUSH_REGS
    mov r12, rsp

    ; Pass exception arguments to the C handler
    mov rdi, [r12 + 120]    ; Exception vector
    mov rsi, [r12 + 128]    ; Error code

    cld
    and rsp, -16

    call exception_error_handler

    mov rsp, r12
    POP_REGS
    add rsp, 16

    iretq
