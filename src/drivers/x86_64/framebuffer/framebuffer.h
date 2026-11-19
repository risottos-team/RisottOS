#ifndef RISOTTOS_FRAMEBUFFER_H
#define RISOTTOS_FRAMEBUFFER_H

#include <limine.h>
#include <stdint.h>

void framebuffer_init(struct limine_framebuffer *fb);
void framebuffer_clear(uint32_t color);

#endif
