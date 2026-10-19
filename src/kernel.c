#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "arch/idt.h"
#include "arch/pic.h"
#include "arch/io.h"
#include "arch/shell.h"
#include "drivers/timer.h"
#include "drivers/ata.h"
#include "mem/mem.h"

// Introduce our hardware setup and jump functions to the kernel
extern void c_tss_init(void);
extern void enter_usermode(void);
extern void tss_init(void);

// A simple temporary user space application function to test Ring 3 execution
// It will be loaded at 0x500000 as configured in our assembly iretd stack layout
void create_test_user_app(void) {
    unsigned char *user_ram = (unsigned char *)0x500000;

    // We inject raw x86 machine code instructions (opcodes) directly into user memory space:
    // This code triggers our newly created Syscall 1 (print_string) using "int 0x80"

    // 1. We want to print a string, so we need to put the string itself somewhere in user RAM first
    const char *test_msg = "Hello from Ring 3 User Mode via Syscall 1!\n";
    int i = 0;
    while (test_msg[i] != '\0') {
        user_ram[100 + i] = test_msg[i]; // Store string content at 0x500000 + 100 bytes offset
        i++;
    }
    user_ram[100 + i] = '\0';

    // 2. Write the executable assembly opcodes into the very beginning of 0x500000:
    user_ram[0] = 0xB8; // 0xB8 = 'mov eax, immediate dword'
    user_ram[1] = 0x01; // Syscall Number 1 (print_string) -> EAX = 1
    user_ram[2] = 0x00;
    user_ram[3] = 0x00;
    user_ram[4] = 0x00;

    user_ram[5] = 0xBB; // 0xBB = 'mov ebx, immediate dword'
    // Pass the exact RAM pointer of our text string (0x500000 + 100 = 0x500064) -> EBX = 0x00500064
    user_ram[6] = 0x64;
    user_ram[7] = 0x00;
    user_ram[8] = 0x50;
    user_ram[9] = 0x00;

    user_ram[10] = 0xCD; // 0xCD 0x80 = 'int 0x80' (Trigger Software Interrupt Gate)
    user_ram[11] = 0x80;

    // HLT is privileged and would raise #GP at CPL 3. Keep the test
    // application alive with a user-mode infinite loop instead.
    user_ram[12] = 0xEB; // JMP short
    user_ram[13] = 0xFE; // Jump back to itself
}

// Kernel Entry Point
void kmain() {
    // Initialize core hardware components
    clear_screen();
    idt_init();         // Set up the interrupt vector desks
    c_tss_init();         // CRITICAL: Activate the CPU Task Register and setup Ring 0 landing pad
    tss_init();

    pic_remap();
    __asm__ volatile ("sti"); // Enable interrupts safely after IDT and TSS are bound

    timer_init(100);
    ata_init();

    // Initialize memory allocator (4MB starting at 1MB boundary)
    mem_init((void *)0x100000, 4 * 1024 * 1024);

    // Boot message
    print_string("RisottOS booted successfully!\n", 0x0F);
    sleep_ticks(100);

    print_string("Preparing to launch RisottOS User Space...\n", 0x0F);
    sleep_ticks(50);

    // Build our crude machine code program at the target user memory zone
    create_test_user_app();

    // Fling the CPU out of Kernel Space into Ring 3!
    enter_usermode();

    // The CPU will NEVER reach this line because it is now running down at Ring 3
    // The old start_shell() function is commented out for now since Ring 3 takes over
    // start_shell();

    while(1) { halt_cpu(); }
}
