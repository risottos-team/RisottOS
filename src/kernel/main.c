#include <limine.h>
#include <stdint.h>
#include <stddef.h>

#include "../drivers/framebuffer/framebuffer.h"

LIMINE_BASE_REVISION(3);

LIMINE_REQUESTS_START_MARKER

static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

LIMINE_REQUESTS_END_MARKER

void kernel_main(void) {
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

  for (;;) {
    __asm__ volatile("hlt");
  }
}
