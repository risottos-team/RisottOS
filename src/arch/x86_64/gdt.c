#include "gdt.h"
#include <stdint.h>

struct gdt_entry gdt[5];
struct gdt_ptr gdtr;

void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].flags_limit_high = (flags << 4) | ((limit >> 16) & 0x0F);
    gdt[index].access = access;
}

void gdt_init(void) {
    gdt_set_entry(0, 0, 0, 0x00, 0x0);
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA);
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC);
    gdt_set_entry(3, 0, 0xFFFFF, 0xF2, 0xC);
    gdt_set_entry(4, 0, 0xFFFFF, 0xFA, 0xA);

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt[0];

    gdt_load(&gdtr);
}
