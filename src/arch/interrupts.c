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

void exception_error_handler(unsigned int vector, unsigned int error_code) {
  print_string("KERNEL PANIC: ", 0x0F);
  switch (vector) {
    case 0:
      print_string("Division Error\n", 0x0F);
      break;
    case 1:
      print_string("Debug Error\n", 0x0F);
      break;
    case 2:
      print_string("Non-Maskable Interrupt Error\n", 0x0F);
      break;
    case 3:
      print_string("Breakpoint Error\n", 0x0F);
      break;
    case 4:
      print_string("Overflow Error\n", 0x0F);
      break;
    case 5:
      print_string("Bound Range Exceeded Error\n", 0x0F);
      break;
    case 6:
      print_string("Invalid Opcode Error\n", 0x0F);
      break;
    case 9:
      print_string("Coprocessor Segment Overrun\n", 0x0F);
      break;
    case 10:
      print_string("Invalid TSS\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 11:
      print_string("Segment not present\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 12:
      print_string("Stack segment fault\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 13:
      print_string("General protection fault\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 15:
      print_string("Reserved\n", 0x0F);
      break;
    case 16:
      print_string("Floating Point Exception Error\n", 0x0F);
      break;
    case 17:
      print_string("Alignment Check\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 18:
      print_string("Machine Check\n", 0x0F);
      break;
    case 19:
      print_string("SIMD Floating-Point Exception\n", 0x0F);
      break;
    case 20:
      print_string("Virtualization Exception\n", 0x0F);
      break;
    case 21:
      print_string("Control Protection Exception\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 22:
      print_string("Reserved\n", 0x0F);
      break;
    case 23:
      print_string("Reserved\n", 0x0F);
      break;
    case 24:
      print_string("Reserved\n", 0x0F);
      break;
    case 25:
      print_string("Reserved\n", 0x0F);
      break;
    case 26:
      print_string("Reserved\n", 0x0F);
      break;
    case 27:
      print_string("Reserved\n", 0x0F);
      break;
    case 28:
      print_string("Hypervisor Injection Error\n", 0x0F);
      break;
    case 29:
      print_string("VMM Communication Exception\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
    case 30:
      print_string("Security Exception\n", 0x0F);
      print_hex(error_code);
      print_string("\n", 0x0F);
      break;
  }
  while (1) {
    __asm__ __volatile__ ("cli; hlt");
  }
}

void device_not_available_error_handler(void) {
  print_string("KERNEL PANIC: Device Not Available\n", 0x0F);
  while (1) {
    __asm__ __volatile__ ("cli; hlt");
  }
}

void double_fault_error_handler(void) {
  print_string("KERNEL PANIC: Double Fault\n", 0x0F);
  while (1) {
    __asm__ __volatile__ ("cli; hlt");
  }
}