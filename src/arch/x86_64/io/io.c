#include "io.h"
#include <stdint.h>
// Write one byte to an I/O port.
void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Read one byte from an I/O port.
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}



uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t inl(uint16_t port){
    uint32_t result;
    __asm__ volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outl(uint16_t port, uint32_t value){
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}


uint8_t mmio_read8(uintptr_t address) { // read 8 bit mmio
    return *(volatile uint8_t *)address;
}
void mmio_write8(uintptr_t address, uint8_t value) { // write 8 bit mmio
    *(volatile uint8_t *)address = value;
}
uint16_t mmio_read16(uintptr_t address) { // read 16 bit mmio
    return *(volatile uint16_t *)address;
}
void mmio_write16(uintptr_t address, uint16_t value) { // write 16 bit mmio
    *(volatile uint16_t *)address = value;
}

uint32_t mmio_read32(uintptr_t address){ // read 32 bit mmio
    return *(volatile uint32_t *)address;
}
void mmio_write32(uintptr_t address, uint32_t value){ // write 32 bit mmio
    *(volatile uint32_t *)address = value;
}
uint64_t mmio_read64(uintptr_t address){ // read 64 bit mmio
    return *(volatile uint64_t *)address;
}
void mmio_write64(uintptr_t address, uint64_t value){ // write 64 bit mmio
    *(volatile uint64_t *)address = value;
}


