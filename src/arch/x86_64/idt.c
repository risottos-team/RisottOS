#include "idt.h"
#include <stdint.h>

static struct idt_entry idt[256];
struct idt_ptr idtr;

void idt_init(void) {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)(uintptr_t)&idt[0];
}

void idt_set_gate(uint8_t number, uintptr_t addr, uint16_t selector, uint8_t ist, uint8_t attributes) {
    struct idt_entry *entry = &idt[number];
    entry->offset_low = (uint16_t)(addr & 0xFFFF);
    entry->offset_middle = (uint16_t)((addr >> 16) & 0xFFFF);
    entry->offset_high = (uint32_t)(addr >> 32);

    entry->selector = selector;
    entry->ist = ist & 0x07;
    entry->type_attributes = attributes;
    entry->reserved = 0;
}
