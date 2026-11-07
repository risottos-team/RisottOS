#include "framebuffer.h"

static volatile uint32_t *pixels;
static uint64_t pixels_per_row;
static uint64_t screen_width;
static uint64_t screen_height;

void framebuffer_init(struct limine_framebuffer *fb) {
    pixels = (volatile uint32_t *)fb->address;

    pixels_per_row = fb->pitch / sizeof(uint32_t);
    screen_width = fb->width;
    screen_height = fb->height;
}

void framebuffer_clear(uint32_t color) {
    for (uint64_t y = 0; y < screen_height; y++) {
        for (uint64_t x = 0; x < screen_width; x++) {
            pixels[y * pixels_per_row + x] = color;
        }
    }
}
