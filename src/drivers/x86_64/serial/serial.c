#include "serial.h"
#include <stdint.h>
#include "../../../arch/x86_64/io/io.h"
uint32_t PORT = COM1;

int serial_init(void) {
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x01);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(PORT + 4, 0x0F);
    return 0;
}
int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

void serial_print(uint8_t a) {
    while (is_transmit_empty() == 0);

    outb(PORT,a);
}

void serial_print_string(const char *str) {
    while (*str) {
        if(*str == 0x0A){
            serial_print('\r');
        }
        serial_print(*str);
        str++;
    }
}

static int serial_data_ready(void) {
    return inb(PORT + 5) & 0x01;
}


char serial_read(void) {
    while (!serial_data_ready());
    return inb(PORT);
}


int serial_try_read(void) {
    if (!serial_data_ready()) {
        return -1;
    }
    return inb(PORT);
}

