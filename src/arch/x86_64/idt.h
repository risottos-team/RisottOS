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
    struct idt_entry *base;
}__attribute__((packed));

_Static_assert(sizeof(struct idt_entry) == 16,
               "IDT entry must be 16 bytes");

_Static_assert(sizeof(struct idt_ptr) == 10,
               "IDT pointer struct must be 10 bytes");

#endif
