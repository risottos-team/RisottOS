#include <limine.h>
#include <stdint.h>
#include <stddef.h>
#include "arch/x86_64/gdt.h"
#include "../../drivers/x86_64/serial/serial.h"
#include "drivers/x86_64/framebuffer/framebuffer.h"
LIMINE_BASE_REVISION(3);

LIMINE_REQUESTS_START_MARKER

static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

LIMINE_REQUESTS_END_MARKER

void kernel_main(void) {
    gdt_init();
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count == 0) {
    for (;;) {
        __asm__ volatile("hlt");
    }
  }

  struct limine_framebuffer *fb =
  framebuffer_request.response->framebuffers[0];

  if (fb->bpp != 32 || fb->memory_model != LIMINE_FRAMEBUFFER_RGB) {
    for (;;) {
      __asm__ volatile("hlt");
    }
  }

  framebuffer_init(fb);
  framebuffer_clear(0x003060B0);
  serial_init();
  serial_print_string("Hello RisottOS Serial X64\n");
  while (1) {
    char buffer[64];
    int i = 0;

    while (1) {
      uint8_t c = serial_read();

      if (c == '\r' || c == '\n') {
        buffer[i] = '\0';
        serial_print_string("\n");
        break;
      }
      if (c == 0x7F || c == 0x08) {
        if (i > 0) {
          i--;
          serial_print_string("\b \b");
        }
        continue;
      }
      if (c >= 32 && c < 127 && i < 63) {
        buffer[i++] = (char)c;
        serial_print(c);
      }
    }
    serial_print_string(buffer);
    serial_print_string("\n");
  }
}
