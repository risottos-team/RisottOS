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
global breakpoint_error_interrupt_handler
global _breakpoint_error_interrupt_handler
global overflow_error_interrupt_handler
global _overflow_error_interrupt_handler
global bound_range_exceeded_error_interrupt_handler
global _bound_range_exceeded_error_interrupt_handler
global invalid_opcode_error_interrupt_handler
global _invalid_opcode_error_interrupt_handler
global device_not_available_error_interrupt_handler
global _device_not_available_error_interrupt_handler
global double_fault_error_interrupt_handler
global _double_fault_error_interrupt_handler
global coprocessor_segment_overrun_error_interrupt_handler
global _coprocessor_segment_overrun_error_interrupt_handler
global invalid_tss_error_interrupt_handler
global _invalid_tss_error_interrupt_handler
global segment_not_present_error_interrupt_handler
global _segment_not_present_error_interrupt_handler
global stack_segment_fault_error_interrupt_handler
global _stack_segment_fault_error_interrupt_handler
global general_protection_fault_error_interrupt_handler
global _general_protection_fault_error_interrupt_handler
global reserved_exception_common
global _reserved_exception_common
global floating_point_exception_error_interrupt_handler
global _floating_point_exception_error_interrupt_handler
global alignment_check_error_interrupt_handler
global _alignment_check_error_interrupt_handler
global machine_check_error_interrupt_handler
global _machine_check_error_interrupt_handler
global simd_error_interrupt_handler
global _simd_error_interrupt_handler
global virtualization_exception_error_interrupt_handler
global _virtualization_exception_error_interrupt_handler
global control_protection_exception_error_interrupt_handler
global _control_protection_exception_error_interrupt_handler
global hypervisor_injection_error_interrupt_handler
global _hypervisor_injection_error_interrupt_handler
global vmm_comm_error_interrupt_handler
global _vmm_comm_error_interrupt_handler
global security_exception_error_interrupt_handler
global _security_exception_error_interrupt_handler

extern ata_handler
extern keyboard_handler
extern timer_handler
extern syscall_dispatcher
extern page_fault_handler
extern exception_error_handler
extern device_not_available_error_handler
extern double_fault_error_handler

; RESERVED EXCEPTION MACRO
%macro RESERVED_EXCEPTION 1
    global reserved_exception_%1

    reserved_exception_%1:
        pusha
        push dword 0       ; No error code
        push dword %1      ; Exception vector
        jmp reserved_exception_common
%endmacro

RESERVED_EXCEPTION 15
RESERVED_EXCEPTION 22
RESERVED_EXCEPTION 23
RESERVED_EXCEPTION 24
RESERVED_EXCEPTION 25
RESERVED_EXCEPTION 26
RESERVED_EXCEPTION 27
RESERVED_EXCEPTION 31

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

  push dword 0 ; No error code
  push dword 0 ; 0x00 - #DE
  call exception_error_handler
  add esp, 8 ; Cleaning stack
  popa
  iretd ; There is no add, esp 4 bcs it has not error code.

debug_error_interrupt_handler:
_debug_error_interrupt_handler:
  pusha

  push dword 0
  push dword 1 ; 0x01 - #DB
  call exception_error_handler
  add esp, 8
  popa
  iretd

non_maskable_error_interrupt_handler:
_non_maskable_error_interrupt_handler:
  pusha

  push dword 0
  push dword 2 ; 0x02 - There is no mnemonic bcs its an interrupt signal
  call exception_error_handler
  add esp, 8
  popa
  iretd

breakpoint_error_interrupt_handler:
_breakpoint_error_interrupt_handler:
  pusha

  push dword 0
  push dword 3 ; 0x03 - #BP
  call exception_error_handler
  add esp, 8
  popa
  iretd

overflow_error_interrupt_handler:
_overflow_error_interrupt_handler:
  pusha

  push dword 0
  push dword 4 ; 0x04 - #OF
  call exception_error_handler
  add esp, 8
  popa
  iretd

bound_range_exceeded_error_interrupt_handler:
_bound_range_exceeded_error_interrupt_handler:
    pusha

    push dword 0
    push dword 5 ; 0x05 - #BR
    call exception_error_handler
    add esp, 8
    popa
    iretd

invalid_opcode_error_interrupt_handler:
_invalid_opcode_error_interrupt_handler:
    pusha

    push dword 0
    push dword 6 ; 0x06 - #UD
    call exception_error_handler
    add esp, 8
    popa
    iretd

device_not_available_error_interrupt_handler:
_device_not_available_error_interrupt_handler:
    pusha
    call device_not_available_error_handler
    popa
    iretd

double_fault_error_interrupt_handler:
_double_fault_error_interrupt_handler:
    pusha
    call double_fault_error_handler
    popa
    add esp, 4
    iretd

coprocessor_segment_overrun_error_interrupt_handler:
_coprocessor_segment_overrun_error_interrupt_handler:
    ; its from era of prehistorical cpus, but i added anyway xD
    pusha

    push dword 0
    push dword 9 ; 0x09 - There is no mnemonic
    call exception_error_handler
    add esp, 8
    popa
    iretd

invalid_tss_error_interrupt_handler:
_invalid_tss_error_interrupt_handler:
    pusha

    push dword [esp+32] ; There is an error code
    push dword 10 ; 0x0A - #TS
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4 ; Cleaning error code
    iretd

segment_not_present_error_interrupt_handler:
_segment_not_present_error_interrupt_handler:
    pusha
    push dword [esp+32]
    push dword 11
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4
    iretd

stack_segment_fault_error_interrupt_handler:
_stack_segment_fault_error_interrupt_handler:
    pusha

    push dword [esp+32]
    push dword 12 ; 0x0C - #SS
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4
    iretd

general_protection_fault_error_interrupt_handler:
_general_protection_fault_error_interrupt_handler:
    pusha

    push dword [esp+32]
    push dword 13 ; 0x0D - #GP
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4
    iretd

page_fault_interrupt_handler:
_page_fault_interrupt_handler:
    pusha
    call page_fault_handler
    popa
    add esp, 4
    iretd

; COMMON RESERVED EXCEPTION HANDLER
reserved_exception_common:
_reserved_exception_common:
    call exception_error_handler
    add esp, 8
    popa
    iretd

floating_point_exception_error_interrupt_handler:
_floating_point_exception_error_interrupt_handler:
    pusha

    push dword 0
    push dword 16 ; 0x10 - #MF
    call exception_error_handler
    add esp, 8
    popa
    iretd

alignment_check_error_interrupt_handler:
_alignment_check_error_interrupt_handler:
    pusha

    push dword [esp+32]
    push dword 17 ; 0x11 - #AC
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4
    iretd

machine_check_error_interrupt_handler:
_machine_check_error_interrupt_handler:
    pusha

    push dword 0
    push dword 18 ; 0x12 - #MC
    call exception_error_handler
    add esp, 8
    popa
    iretd

simd_error_interrupt_handler:
_simd_error_interrupt_handler:
    pusha

    push dword 0
    push dword 19 ; 0x13 - #XM / #XF
    call exception_error_handler
    add esp, 8
    popa
    iretd

virtualization_exception_error_interrupt_handler:
_virtualization_exception_error_interrupt_handler:
    pusha

    push dword 0
    push dword 20 ; 0x14 - #VE
    call exception_error_handler
    add esp, 8
    popa
    iretd

control_protection_exception_error_interrupt_handler:
_control_protection_exception_error_interrupt_handler:
    pusha

    push dword [esp+32]
    push dword 21 ; 0x15 - #CP
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4
    iretd

hypervisor_injection_error_interrupt_handler:
_hypervisor_injection_error_interrupt_handler:
    pusha

    push dword 0
    push dword 28 ; 0x1C - #HV
    call exception_error_handler
    add esp, 8
    popa
    iretd

vmm_comm_error_interrupt_handler:
_vmm_comm_error_interrupt_handler:
    pusha

    push dword [esp+32]
    push dword 29 ; 0x1D - #VC
    call exception_error_handler
    add esp, 8
    popa

    add esp, 4
    iretd

security_exception_error_interrupt_handler:
_security_exception_error_interrupt_handler:
    pusha

    push dword [esp+32]
    push dword 30 ; 0x1E - #SX
    call exception_error_handler
    add esp, 8
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
