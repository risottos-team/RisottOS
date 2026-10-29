#include "idt.h"
#include "../drivers/vga.h"

extern void interrupt_handler(void);
extern void timer_interrupt_handler(void);
extern void ata_interrupt_handler(void);
extern void syscall_interrupt_handler(void);
extern void page_fault_interrupt_handler(void);

// ---------------------------------------------------------------------------
// IDT Structures
// ---------------------------------------------------------------------------
struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attributes;
    unsigned short base_high;
} __attribute__((packed));

struct idt_pointer {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_pointer idtr;

// ---------------------------------------------------------------------------
// GDT & TSS Structures (NEW MODULAR C-BASED ARCHITECTURE)
// ---------------------------------------------------------------------------
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_pointer {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

typedef struct {
    unsigned int link;
    unsigned int esp0;       // Kernel Stack Pointer for Ring 0 landing
    unsigned int ss0;        // Kernel Stack Segment (0x10)
    unsigned int esp1, ss1, esp2, ss2;
    unsigned int cr3;
    unsigned int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    unsigned int es, cs, ss, ds, fs, gs;
    unsigned int ldt;
    unsigned short trap;
    unsigned short iomap_base;
} __attribute__((packed)) TSS_Structure;

// Create clean, aligned tables in safe memory
static struct gdt_entry gdt[6];
static struct gdt_pointer gdtr;
static TSS_Structure risottos_tss __attribute__((aligned(4)));

// Helper to write flat, precise GDT entries in C
static void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = ((base >> 16) & 0xFF);
    gdt[num].base_high   = ((base >> 24) & 0xFF);

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access      = access;
}

// Global accessor so Assembly can grab the TSS address easily
unsigned int get_tss_address(void) {
    return (unsigned int)&risottos_tss;
}

void idt_init (void) {
    int i;

    // Clear IDT gates
    for (i = 0; i < 256; i++) {
        idt[i].base_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attributes = 0;
        idt[i].base_high = 0;
    }

    // Install standard hardware interrupt handlers
    idt_set_gate(0x21, (unsigned int)interrupt_handler);
    idt_set_gate(0x20, (unsigned int)timer_interrupt_handler);
    idt_set_gate(0x2E, (unsigned int)ata_interrupt_handler);
    idt_set_gate(0x0E, (unsigned int)page_fault_interrupt_handler);

    // Install the 0x80 Syscall Gate.
    // 0xEF = present, DPL 3, 32-bit TRAP gate. A trap gate leaves IF set, so
    // IRQs (notably IRQ14 from the ATA driver, which RFS depends on) keep
    // being delivered while a syscall runs. 0xEE (interrupt gate) cleared IF
    // and made every disk access inside a syscall time out.
    idt_set_gate(0x80, (unsigned int)syscall_interrupt_handler);
    idt[0x80].type_attributes = 0xEF;

    // Load IDT into CPU
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (unsigned int)idt;
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}

void idt_set_gate(unsigned char number, unsigned int addr) {
    idt[number].base_low = addr & 0xFFFF;
    idt[number].selector = 0x08; // Kernel Code Segment is at index 1 (0x08)
    idt[number].zero = 0;
    idt[number].type_attributes = 0x8E;
    idt[number].base_high = (addr >> 16) & 0xFFFF;
}

// Build the global descriptor table completely in C to avoid any compiler/boot assembler gaps
void c_tss_init(void) {
    // 1. Clear and configure the physical TSS structure
    unsigned char *tss_bytes = (unsigned char *)&risottos_tss;
    for (unsigned int i = 0; i < sizeof(TSS_Structure); i++) {
        tss_bytes[i] = 0;
    }

    risottos_tss.ss0  = 0x10;       // Points to Kernel Data Segment
    risottos_tss.esp0 = 0x90000;    // Points to the safe 32-bit kernel stack top
    risottos_tss.iomap_base = sizeof(TSS_Structure);

    // 2. Build a brand new flat GDT right here in C!
    gdt_set_gate(0, 0, 0, 0, 0);                // 0x00: Null Descriptor
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // 0x08: Kernel Code Segment (Ring 0)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // 0x10: Kernel Data Segment (Ring 0)
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // 0x18: User Code Segment (Ring 3)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 0x20: User Data Segment (Ring 3)

    // 0x28: Task State Segment (TSS Descriptor mapped precisely to risottos_tss physical memory address)
    unsigned int tss_base = (unsigned int)&risottos_tss;
    gdt_set_gate(5, tss_base, sizeof(TSS_Structure) - 1, 0x89, 0x40);

    // 3. Load this clean GDT into the CPU directly, completely overwriting the old boot.asm GDT zone
    gdtr.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gdtr.base  = (unsigned int)gdt;
    __asm__ volatile("lgdt %0" : : "m"(gdtr));
}