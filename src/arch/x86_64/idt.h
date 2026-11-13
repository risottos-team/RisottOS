#ifndef R_X86_64_IDT_H
#define R_X86_64_IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
}__attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

void idt_init(void);
void idt_set_gate(uint8_t number, uintptr_t addr, uint16_t selector, uint8_t ist, uint8_t attributes);
void idt_load(struct idt_ptr *ptr);

_Static_assert(sizeof(struct idt_entry) == 16,
               "IDT entry must be 16 bytes");

_Static_assert(sizeof(struct idt_ptr) == 10,
               "IDT pointer struct must be 10 bytes");

#endif
