#include "idt.h"
#include <stdint.h>

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);

static struct idt_entry idt[256];
struct idt_ptr idtr;

void idt_init(void) {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)(uintptr_t)&idt[0];

    idt_set_gate(0, 0x00, selector, t ist, uint8_t attributes)
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

