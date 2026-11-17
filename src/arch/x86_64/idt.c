
#include "idt.h"
#include <stdint.h>
#include <stddef.h>

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
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

static struct idt_entry idt[256];
struct idt_ptr idtr;

static void (* const exception_stubs[32])(void) = {
    isr0,  isr1,  isr2,  isr3,
    isr4,  isr5,  isr6,  isr7,
    isr8,  isr9,  isr10, isr11,
    isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19,
    isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27,
    isr28, isr29, isr30, isr31
};

void idt_set_gate(
    uint8_t number,
    uintptr_t addr,
    uint16_t selector,
    uint8_t ist,
    uint8_t attributes
) {
    struct idt_entry *entry = &idt[number];

    entry->offset_low = (uint16_t)(addr & 0xFFFF);
    entry->offset_middle = (uint16_t)((addr >> 16) & 0xFFFF);
    entry->offset_high = (uint32_t)(addr >> 32);

    entry->selector = selector;
    entry->ist = ist & 0x07;
    entry->type_attributes = attributes;
    entry->reserved = 0;
}

void idt_init(void) {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)(uintptr_t)&idt[0];

    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(
            i,
            (uintptr_t)exception_stubs[i],
            0x08,
            0,
            0x8E
        );
    }

    idt_load(&idtr);
}
