#include "io.h"

// Write one byte to an I/O port.
void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Read one byte from an I/O port.
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Halt the CPU until the next interrupt arrives to save power.
void halt_cpu(void) {
    __asm__ volatile ("hlt");
}


unsigned short inw(unsigned short port) {
    unsigned short result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(unsigned short port, unsigned short value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

unsigned int inl(unsigned short port){
    unsigned int result;
    __asm__ volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outl(unsigned short port, unsigned int value){
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}
unsigned int mmio_read32(unsigned int address){
    return *(volatile unsigned int *)address;
}
void mmio_write32(unsigned int address, unsigned int value){
    *(volatile unsigned int *)address = value;
}