#ifndef R_IO_H
#define R_IO_H
#include <stdint.h>
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t value);

uint8_t  mmio_read8(uintptr_t address);
void     mmio_write8(uintptr_t address, uint8_t value);
uint16_t mmio_read16(uintptr_t address);
void  mmio_write16(uintptr_t address, uint16_t value);
uint32_t mmio_read32(uintptr_t address);
void mmio_write32(uintptr_t address , uint32_t value);
uint64_t mmio_read64(uintptr_t address);
void mmio_write64(uintptr_t address , uint64_t value);
#endif
