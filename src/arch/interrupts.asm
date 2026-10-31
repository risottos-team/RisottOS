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
global page_fault_interrupt_handler
global _page_fault_interrupt_handler
global enter_usermode
global _enter_usermode
global tss_init
global _tss_init
global division_error_interrupt_handler
global _division_error_interrupt_handler
global debug_error_interrupt_handler
global _debug_error_interrupt_handler
global non_maskable_error_interrupt_handler
global _non_maskable_error_interrupt_handler
global non_maskable_error_handler
global breakpoint_error_interrupt_handler
global _breakpoint_error_interrupt_handler
global overflow_error_interrupt_handler
global _overflow_error_interrupt_handler
global bound_range_exceeded_error_interrupt_handler
global _bound_range_exceeded_error_interrupt_handler

extern ata_handler
extern keyboard_handler
extern timer_handler
extern syscall_dispatcher
extern page_fault_handler
extern division_error_handler
extern debug_error_handler
extern non_maskable_error_handler
extern breakpoint_error_handler
extern overflow_error_handler
extern bound_range_exceeded_error_handler

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

; Exception Handlers

division_error_interrupt_handler:
_division_error_interrupt_handler:
  pusha
  call division_error_handler
  popa
  iretd ; There is no add, esp 4 bcs it has not error code.

debug_error_interrupt_handler:
_debug_error_interrupt_handler:
  pusha
  call debug_error_handler
  popa
  iretd

non_maskable_error_interrupt_handler:
_non_maskable_error_interrupt_handler:
  pusha
  call debug_error_handler
  popa
  iretd

breakpoint_error_interrupt_handler:
_breakpoint_error_interrupt_handler:
  pusha
  call breakpoint_error_handler
  popa
  iretd

overflow_error_interrupt_handler:
_overflow_error_interrupt_handler:
  pusha
  call overflow_error_handler
  popa
  iretd

bound_range_error_interrupt_handler:
_bound_range_error_interrupt_handler:
    pusha
    call bound_range_error_handler
    popa
    iretd

page_fault_interrupt_handler:
_page_fault_interrupt_handler:
    pusha
    call page_fault_handler
    popa
    add esp, 4
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

    lea eax, [esp + 8]  ; Pass the PUSHA frame, not the segment saves
    push eax
    call syscall_dispatcher
    add esp, 4

    ; Return the C dispatcher result through the saved PUSHA EAX slot.
    mov [esp + 36], eax

    pop es
    pop ds
    popa
    iretd

; Hardware function to load the TR register safely
tss_init:
_tss_init:
    mov ax, 0x28        ; 0x28 is the exact GDT index for our TSS Descriptor
    ltr ax              ; Load the task register.
    ret

; Enter the Ring 3 user application.
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
    push 0x800000       ; ESP (User Stack top, separate from kernel stack)
    pushfd              ; EFLAGS

    pop eax
    or eax, 0x200       ; Set IF bit to keep hardware interrupts active in User Mode
    push eax

    push 0x1B           ; CS (User Code 0x18 | 3 = 0x1B)
    push 0x500000       ; EIP (Target application entry point)

    iretd               ; Switch to Ring 3.
