#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "../drivers/rtc.h"
#include "../drivers/ata.h"
#include "../mem/mem.h"
#include "../rfs/rfs.h"
#include "../mem/paging.h"
#include "demos.h"

// Pseudo error codes the Ring 3 shell can pass to syscall 27 for errors that
// it detects itself (it does not know the RFS_E_* values).
#define U_ERR_INVAL  1000
#define U_ERR_NOTDIR 1001

typedef struct {
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed)) SyscallRegisters;

static void print_rfs_error(int code) {
    const char *msg;

    if (code == U_ERR_INVAL)  code = RFS_E_INVAL;
    if (code == U_ERR_NOTDIR) code = RFS_E_NOTDIR;

    switch (code) {
        case RFS_E_IO:       msg = "disk error"; break;
        case RFS_E_NOTFOUND: msg = "no such file or directory"; break;
        case RFS_E_EXISTS:   msg = "already exists"; break;
        case RFS_E_FULL:     msg = "file table is full"; break;
        case RFS_E_NOSPACE:  msg = "not enough contiguous free space"; break;
        case RFS_E_NOTEMPTY: msg = "directory is not empty"; break;
        case RFS_E_NOTDIR:   msg = "not a directory"; break;
        case RFS_E_ISDIR:    msg = "is a directory"; break;
        case RFS_E_INVAL:    msg = "invalid path or name"; break;
        case RFS_E_NOFS:     msg = "filesystem not mounted (run: rfs init or rfs format)"; break;
        default:             msg = "unknown error"; break;
    }
    print_string("Error: ", 0x0F);
    print_string(msg, 0x0F);
    print_char('\n', 0x0F);
}

// Syscall ABI: eax = number, ebx / ecx / edx = arguments, result in eax.
//
//   1  print_string(str)            2  getkey() (blocking)
//   3  sleep_ticks(ticks)           4  clear_screen()
//   5  print_char(c)                6  rfs_list(path)
//   7  rfs_mkdir(path)              9  rfs_free_sectors()
//  10  rfs_deldir(path)            11  rfs_create(path)
//  12  rfs_delete(path)            13  rfs_write(path, data, len)
//  14  rfs_size(path)              15  rfs_read(path, buf, size)
//  16  rfs_isdir(path)             17  rfs_info()
//  18  rfs_init()                  19  rfs_format()
//  20  ata_read_sector(lba, buf)   21  ata_write_sector(lba, buf)
//  22  ata_disc_space()            23  rtc print (0 time, 1 sec, 2 min, 3 hour)
//  25  r_malloc(size)              26  r_free(ptr)
//  27  print_rfs_error(code)
//  30  ui_demo  31  ui_colors  32  gfx_demo  33  gfx_ball
unsigned int syscall_dispatcher(SyscallRegisters *regs) {
    unsigned int ret = 0;

    switch (regs->eax) {
        case 1:
            if (user_string_valid((const char *)regs->ebx, 4096))
                print_string((const char *)regs->ebx, 0x0F);
            else ret = (unsigned int)-1;
            break;
        case 2:  ret = (unsigned int)(unsigned char)get_key(); break;
        case 3:  sleep_ticks(regs->ebx); break;
        case 4:  clear_screen(); break;
        case 5:  print_char((char)regs->ebx, 0x0F); break;

        case 6:  if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_list((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 7:  if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_mkdir((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 9:  ret = (unsigned int)rfs_free_sectors(); break;
        case 10: if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_deldir((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 11: if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_create((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 12: if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_delete((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 13: if (user_string_valid((const char *)regs->ebx, 128) && user_range_valid((const void *)regs->ecx, regs->edx)) ret = (unsigned int)rfs_write((const char *)regs->ebx, (const void *)regs->ecx, regs->edx); else ret = (unsigned int)-1; break;
        case 14: if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_size((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 15: if (user_string_valid((const char *)regs->ebx, 128) && user_range_valid((void *)regs->ecx, regs->edx)) ret = (unsigned int)rfs_read((const char *)regs->ebx, (void *)regs->ecx, regs->edx); else ret = (unsigned int)-1; break;
        case 16: if (user_string_valid((const char *)regs->ebx, 128)) ret = (unsigned int)rfs_isdir((const char *)regs->ebx); else ret = (unsigned int)-1; break;
        case 17: ret = (unsigned int)rfs_info(); break;
        case 18: ret = (unsigned int)rfs_init(); break;
        case 19: ret = (unsigned int)rfs_format(); break;

        case 20: if (user_range_valid((void *)regs->ecx, 512)) ret = (unsigned int)ata_read_sector(regs->ebx, (unsigned short *)regs->ecx); else ret = (unsigned int)-1; break;
        case 21: if (user_range_valid((void *)regs->ecx, 512)) ret = (unsigned int)ata_write_sector(regs->ebx, (unsigned short *)regs->ecx); else ret = (unsigned int)-1; break;
        case 22: ret = (unsigned int)ata_disc_space(); break;

        case 23:
            switch (regs->ebx) {
                case 0: print_string(rtc_get_time(), 0x0F); break;
                case 1: print_string(rtc_get_seconds(), 0x0F); break;
                case 2: print_string(rtc_get_minutes(), 0x0F); break;
                case 3: print_string(rtc_get_hours(), 0x0F); break;
            }
            break;

        case 25: ret = (unsigned int)user_malloc(regs->ebx); break;
        case 26: user_free((void *)regs->ebx); break;
        case 27: print_rfs_error((int)regs->ebx); break;

        case 30: ui_demo(); break;
        case 31: ui_colors(); break;
        case 32: gfx_demo(); break;
        case 33: gfx_ball(); break;

        default:
            print_string("Syscall: unknown number.\n", 0x0F);
            ret = (unsigned int)-1;
            break;
    }

    // Store the result in the saved eax too, so it works whether the asm stub
    // uses the return value or just pops the saved registers.
    regs->eax = ret;
    return ret;
}