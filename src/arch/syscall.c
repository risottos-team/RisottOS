#include "../drivers/vga.h"
#include "../mem/mem.h"

// Struct mapping the register layout we pushed in assembly
typedef struct {
    unsigned int es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed)) SyscallRegisters;

// The main dispatcher called from assembly
void syscall_dispatcher(SyscallRegisters *regs) {
    // EAX holds the syscall number
    switch (regs->eax) {
        case 1:
            // Syscall 1: print_string(char *str)
            // EBX holds the pointer to the string
            print_string((const char *)regs->ebx, 0x0F);
            break;

        case 2:
            // Syscall 2: r_malloc(size)
            // EBX holds the size requested. Return the pointer in EAX!
            regs->eax = (unsigned int)r_malloc(regs->ebx);
            break;

        case 3:
            // Syscall 3: r_free(ptr)
            // EBX holds the pointer to free
            r_free((void *)regs->ebx);
            break;

        default:
            print_string("Syscall Error: Unknown syscall number.\n", 0x0C);
            break;
    }
}
