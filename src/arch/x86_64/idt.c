#include "idt.h"

static struct idt_entry idt[256];
struct idt_ptr idtr;

void idt_init() {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (&idt[0]);
}

void idt_set_gate(uint8_t number, uintptr_t addr, uint16_t selector, uint8_t ist, uint8_t attributes) {
    idt[number] 
}
