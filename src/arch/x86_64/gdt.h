#ifndef R_X86_64_GDT_H
#define R_X86_64_GDT_H

#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags_limit_high;
    uint8_t base_high;
}__attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_init(void);
void gdt_load(struct gdt_ptr *ptr);

_Static_assert(sizeof(struct gdt_entry) == 8,
               "GDT entry must be 8 bytes");

_Static_assert(sizeof(struct gdt_ptr) == 10,
               "GDT pointer must be 10 bytes");

#endif
