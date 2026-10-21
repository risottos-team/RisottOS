#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../mem/mem.h"
#include "../rfs/rfs.h"

typedef struct {
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed)) SyscallRegisters;

unsigned int syscall_dispatcher(SyscallRegisters *regs) {
    switch (regs->eax) {
        case 1: // print_string(char *str)
            print_string((const char *)regs->ebx, 0x0F);
            return 0;

        case 2: // blocking keyboard input
            return (unsigned int)(unsigned char)get_key();

        case 5: // print_char(char c)
            print_char((char)regs->ebx, 0x0F);
            return 0;

        case 6: // rfs_list(char *path)
            regs->eax = (unsigned int)rfs_list((const char *)regs->ebx);
            return regs->eax;

        case 7: // rfs_mkdir(char *path)
            regs->eax = (unsigned int)rfs_mkdir((const char *)regs->ebx);
            return regs->eax;

        case 8: // Print a file from RFS.
        {
            const char *path = (const char *)regs->ebx;
            int size = rfs_size(path);
            if (size >= 0) {
                unsigned char *buf = (unsigned char *)r_malloc(size);
                if (buf) {
                    int got = rfs_read(path, buf, size);
                    for (int i = 0; i < got; i++) print_char((char)buf[i], 0x0F);
                    r_free(buf);
                }
            }
        }
        return 0;

        case 9: // Return the number of free RFS sectors.
            regs->eax = rfs_free_sectors();
            return regs->eax;

        default:
            print_string("Syscall: Active.\n", 0x0F);
            return 0;
    }
}
