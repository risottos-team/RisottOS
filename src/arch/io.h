#ifndef R_IO_H
#define R_IO_H

void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);
void outw(unsigned short port, unsigned short value);
unsigned short inw(unsigned short port);
unsigned int inl(unsigned short port);
void outl(unsigned short port, unsigned int value);
void halt_cpu(void);

#endif
