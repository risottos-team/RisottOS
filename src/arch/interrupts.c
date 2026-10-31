#include "../drivers/ata.h"
#include "../drivers/keyboard.h"
#include "../drivers/vga.h"
#include "pic.h"

unsigned int tick_count = 0;

void keyboard_handler(void) {
  // Forward the IRQ to the keyboard driver.
  keyboard_irq_handler();

  pic_send_eoi(1);
}

void timer_handler(void) {
  tick_count++;

  pic_send_eoi(0);
}

void ata_handler(void) {
  ata_irq_handler();
  pic_send_eoi(14);
}

// ======================
// CPU EXCEPTION HANDLERS
// ======================

void division_error_handler(void) {
  print_string("KERNEL PANIC: Division Error\n”", 0x0F);

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}

void debug_error_handler(void) {
  print_string("KERNEL PANIC: Debug Error\n", 0x0F);

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}

void non_maskable_error_handler(void) {
  print_string("KERNEL PANIC: Non-Maskable Error\n", 0x0F);

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}

void breakpoint_error_handler(void) {
  print_string("KERNEL PANIC: Breakpoint Error\n", 0x0F);

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}

void overflow_error_handler(void) {
  print_string("KERNEL PANIC: Overflow Error\n", 0x0F);

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}

void bound_range_error_handler(void) {
  print_string("KERNEL PANIC: Bound Range Error\n", 0x0F);

  while (1) {
    __asm__ volatile("cli; hlt");
  }
}
