; ============================================
; boot.asm - RisottOS Bootloader
; Loaded by BIOS at 0x7C00, runs in 16-bit real mode
; ============================================

; The BIOS loads us at 0x7C00, but the kernel is loaded at 0x1000 and is
; bigger than 0x6C00 bytes (54 sectors), so it would overwrite this very
; code (and the real-mode stack) while being read. So the first thing we do
; is copy this sector down to 0x0600 and keep running from there.
; The whole file is assembled for 0x0600; the first few instructions below
; only use absolute immediates, so they work while still sitting at 0x7C00.
[org 0x0600]
bits 16

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x0600          ; stack grows down below the relocated code
    sti
    mov si, 0x7C00          ; source: where the BIOS put us
    mov di, 0x0600          ; destination
    mov cx, 256             ; 256 words = 512 bytes
    cld
    rep movsw
    jmp 0x0000:relocated    ; continue in the copy (dl is still the boot drive)

relocated:

    mov [bootdisk], dl

    mov ax, 0x2401
    int 0x15

    mov si, bootmsg
    call print

    mov si, dap
    mov ah, 0x42
    mov dl, [bootdisk]
    int 0x13
    jc disk_error

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode_start

dap:
    db 0x10
    db 0
    dw 127                   ; sectors 1..127 (the RFS filesystem starts at LBA 256 now)
    dw 0x1000
    dw 0x0000
    dd 1
    dd 0


; ============================================
; print - prints a null-terminated string
; input: si = address of string
; ============================================
print:
.loop:
    lodsb                ; al = [si], si++
    or al, al             ; check for null terminator
    jz .done

    mov ah, 0x0E            ; BIOS teletype output
    int 0x10

    jmp .loop

.done:
    ret

; ============================================
; disk_error - prints error message and halts
; ============================================
disk_error:
    mov si, disk_error_msg
    call print
    jmp $                    ; halt forever

; ============================================
; GDT (Global Descriptor Table)
;
; Purpose: Protected mode no longer uses segment:offset math like real
; mode did. Instead, segment registers become indexes into this table,
; which tells the CPU where each segment starts, how big it is, and
; what it's allowed to do (executable, writable, etc).
;
; We use a "flat model": both segments start at 0 and cover the full
; 4GB address space, so in practice segmentation doesn't restrict us
; and we rely on paging later instead.
; ============================================
gdt_start:
    dq 0                      ; 0x00: Null descriptor - required by CPU

gdt_code:                     ; 0x08: Kernel Code Segment (Ring 0)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b              ; Present, Ring 0, Code, Readable
    db 11001111b
    db 0x00

gdt_data:                     ; 0x10: Kernel Data Segment (Ring 0)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b              ; Present, Ring 0, Data, Writable
    db 11001111b
    db 0x00

gdt_user_code:                ; 0x18: User Code Segment (Ring 3) - NEW!
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 11111010b              ; Present, RING 3, Code, Readable
    db 11001111b
    db 0x00

gdt_user_data:                ; 0x20: User Data Segment (Ring 3) - NEW!
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 11110010b              ; Present, RING 3, Data, Writable
    db 11001111b
    db 0x00

gdt_tss:                      ; 0x28: Task State Segment Descriptor
    dw 103                    ; Size of TSS minus 1 (104 bytes total)
    dw 0x0000                 ; Base address (bits 0-15) - Will be set in C code dynamically
    db 0x00                   ; Base address (bits 16-23)
    db 10001001b              ; Present, Ring 0, Executable, Type: 32-bit TSS (Available)
    db 0x00                   ; Flags
    db 0x00                   ; Base address (bits 24-31)

gdt_end:


; This structure tells the CPU where the GDT is and how big it is.
; The lgdt instruction (next step) will load this into the CPU.
gdt_descriptor:
    dw gdt_end - gdt_start - 1    ; size of GDT minus 1 (CPU convention)
    dd gdt_start                    ; address of the GDT itself


[bits 32]
protected_mode_start:
    mov ax, 0x10          ; gdt_data selector
    mov ds, ax               ; point data segments to it
    mov es, ax
    mov ss, ax

    mov esp, 0x90000          ; set up a fresh 32-bit stack

    mov byte [0xB8780], 'P'      ; write near middle of screen instead of top-left
    mov byte [0xB8782], 'M'

    jmp 0x1000                      ; placeholder, kernel jump comes here later



; ============================================
; Data
; ============================================
bootmsg:         db 13, 10, "Booting RisottOS...", 13, 10, 0
bootdisk:         db 0
disk_error_msg: db "Disk read failed!", 13, 10, 0

; ============================================
; Padding + boot signature
; ============================================
times 510-($-$$) db 0
dw 0xAA55