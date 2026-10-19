[bits 32]

; Global definitions for the kernel (both with and without underscores for compatibility)
global interrupt_handler
global _interrupt_handler
global timer_interrupt_handler
global _timer_interrupt_handler
global ata_interrupt_handler
global _ata_interrupt_handler
global syscall_interrupt_handler
global _syscall_interrupt_handler
global enter_usermode
global _enter_usermode
global tss_init
global _tss_init

extern ata_handler
extern keyboard_handler
extern timer_handler
extern syscall_dispatcher

; Standard Interrupt Handlers
interrupt_handler:
_interrupt_handler:
    pusha
    call keyboard_handler
    popa
    iretd

timer_interrupt_handler:
_timer_interrupt_handler:
    pusha
    call timer_handler
    popa
    iretd

ata_interrupt_handler:
_ata_interrupt_handler:
    pusha
    call ata_handler
    popa
    iretd

; Syscall Gate Handler (int 0x80)
syscall_interrupt_handler:
_syscall_interrupt_handler:
    pusha               ; Save registers (EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)
    push ds             ; Save data segments
    push es

    mov ax, 0x10        ; Load Kernel Data Segment descriptor
    mov ds, ax
    mov es, ax

    push esp            ; Pass registers pointer to C
    call syscall_dispatcher
    add esp, 4

    pop es
    pop ds
    popa
    iretd

; Hardware function to load the TR register safely
tss_init:
_tss_init:
    mov ax, 0x28        ; 0x28 is the exact GDT index for our TSS Descriptor
    ltr ax              ; FIRE THE HARDWARE TR REGISTER PIN!
    ret

; The legendary jump function that flings the CPU down to Ring 3 (EKSİK OLAN KISIM BUYDU)
enter_usermode:
_enter_usermode:
    cli                 ; Clear interrupts during switch

    mov ax, 0x23        ; User Data Segment descriptor (0x20 | 3 = 0x23)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Manually push the execution context that IRETD expects
    push 0x23           ; SS (User Data)
    push 0x8FFFF        ; ESP (User Stack top)
    pushfd              ; EFLAGS

    pop eax
    or eax, 0x200       ; Set IF bit to keep hardware interrupts active in User Mode
    push eax

    push 0x1B           ; CS (User Code 0x18 | 3 = 0x1B)
    push 0x500000       ; EIP (Target application entry point)

    iretd               ; FLING CPU TO RING 3 officially!
