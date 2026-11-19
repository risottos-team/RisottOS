#include "paging.h"
#include "../drivers/vga.h"

#define PAGE_PRESENT 0x001
#define PAGE_WRITABLE 0x002
#define PAGE_PWT 0x008 // Page Write-Through
#define PAGE_PCD 0x010 // Page Cache Disable
#define PAGE_USER 0x004
#define PAGE_SIZE 0x1000
#define IDENTITY_MAP_BYTES (16 * 1024 * 1024)
#define USER_START 0x00400000
#define USER_END 0x01000000

static unsigned int page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
static unsigned int page_tables[4][1024] __attribute__((aligned(PAGE_SIZE)));
static unsigned int mmio_tables[2][1024] __attribute__((aligned(PAGE_SIZE)));
unsigned int mmio_tables_used = 0;


static unsigned int read_cr2(void) {
    unsigned int address;
    __asm__ volatile("mov %%cr2, %0" : "=r"(address));
    return address;
}

int paging_map_mmio(unsigned int physical, unsigned int size) {
    unsigned int last_byte, start, pages, i, j;

    if (size == 0) return -1;
    last_byte = physical + size - 1;
    if (last_byte < physical) return -1;            /* wrapped past 4 GB */

    start = physical & ~0xFFFu;
    if (start < IDENTITY_MAP_BYTES) return -1;      /* already identity mapped */

    pages = (((last_byte & ~0xFFFu) - start) / PAGE_SIZE) + 1;

    for (i = 0; i < pages; i++) {
        unsigned int addr = start + i * PAGE_SIZE;
        unsigned int dir = addr >> 22;
        unsigned int idx = (addr >> 12) & 0x3FF;
        unsigned int *table;

        if (!(page_directory[dir] & PAGE_PRESENT)) {
            if (mmio_tables_used >= 2) return -1;   /* out of tables */
            table = mmio_tables[mmio_tables_used++];
            for (j = 0; j < 1024; j++) table[j] = 0;
            page_directory[dir] = (unsigned int)table | PAGE_PRESENT | PAGE_WRITABLE;
        } else {
            table = (unsigned int *)(page_directory[dir] & ~0xFFFu);
        }

        if (table[idx] & PAGE_PRESENT) {
            if ((table[idx] & ~0xFFFu) != addr) return -1;  /* conflicting mapping */
            continue;
        }

        table[idx] = addr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_PCD | PAGE_PWT;
        __asm__ volatile("invlpg (%0)" : : "r"(addr) : "memory");
    }
    return 0;
}
void paging_init(void) {
    unsigned int directory_index;
    unsigned int table_index;
    mmio_tables_used = 0;
    for (directory_index = 0; directory_index < 1024; directory_index++) {
        page_directory[directory_index] = 0;
    }

    for (directory_index = 0; directory_index < 4; directory_index++) {
        unsigned int page_flags = PAGE_PRESENT | PAGE_WRITABLE;

        if (directory_index != 0) {
            page_flags |= PAGE_USER;
        }

        page_directory[directory_index] =
            (unsigned int)page_tables[directory_index] | page_flags;

        for (table_index = 0; table_index < 1024; table_index++) {
            unsigned int physical_address =
                (directory_index * 1024 + table_index) * PAGE_SIZE;
            page_tables[directory_index][table_index] =
                physical_address | page_flags;
        }
    }

    __asm__ volatile(
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0"
        :
        : "r"(page_directory)
        : "eax", "memory");
}

int user_range_valid(const void *address, unsigned int size) {
    unsigned int start = (unsigned int)address;
    unsigned int end;

    if (size == 0) return start >= USER_START && start < USER_END;
    if (start < USER_START || start >= USER_END) return 0;
    end = start + size;
    if (end < start || end > USER_END) return 0;
    return 1;
}

int user_string_valid(const char *string, unsigned int max_length) {
    unsigned int i;

    if (!user_range_valid(string, 1)) return 0;
    for (i = 0; i < max_length; i++) {
        if (!user_range_valid(string + i, 1)) return 0;
        if (string[i] == '\0') return 1;
    }
    return 0;
}

void page_fault_handler(void) {
    unsigned int fault_address = read_cr2();

    clear_screen();
    print_string("Page fault: unmapped or protected memory access.\n", 0x0C);
    print_string("Fault address: 0x", 0x0F);
    print_hex(fault_address);
    print_string("\nSystem halted.\n", 0x0C);

    __asm__ volatile("cli");
    while (1) {
        __asm__ volatile("hlt");
    }
}
