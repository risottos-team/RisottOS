#include "paging.h"
#include "../drivers/vga.h"

#define PAGE_PRESENT 0x001
#define PAGE_WRITABLE 0x002
#define PAGE_USER 0x004
#define PAGE_SIZE 0x1000
#define IDENTITY_MAP_BYTES (16 * 1024 * 1024)

static unsigned int page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
static unsigned int page_tables[4][1024] __attribute__((aligned(PAGE_SIZE)));

static unsigned int read_cr2(void) {
    unsigned int address;
    __asm__ volatile("mov %%cr2, %0" : "=r"(address));
    return address;
}

static void print_hex(unsigned int value) {
    const char *digits = "0123456789ABCDEF";
    int shift;

    for (shift = 28; shift >= 0; shift -= 4) {
        print_char(digits[(value >> shift) & 0x0F], 0x0F);
    }
}

void paging_init(void) {
    unsigned int directory_index;
    unsigned int table_index;

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
