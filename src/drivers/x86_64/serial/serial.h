#ifndef R_SERIAL_H
#define R_SERIAL_H
#include <stdint.h>
#define COM1 0x3F8
#define BAUD_RATE 115200

int serial_init(void);
void serial_print(uint8_t a);
int is_serial_empty(void);

void serial_print_string(const char *str);

char serial_read(void);
int serial_try_read(void);
#endif
