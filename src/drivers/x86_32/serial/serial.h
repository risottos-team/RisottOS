#ifndef R_SERIAL_H
#define R_SERIAL_H

#define COM1 0x3F8
#define BAUD_RATE 115200

int serial_init(void);
void serial_write(unsigned char a);
int is_serial_empty(void);
void serial_write_char(unsigned char a);
void serial_write_string(const char *str);
#endif
