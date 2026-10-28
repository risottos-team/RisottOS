#include "pci.h"
#include "../arch/io.h"
#define ENABLE_BIT 0x80000000u

unsigned int pci_read32(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset){
    unsigned int address = 0;
    address = (ENABLE_BIT) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (offset & 0xFC);
    outl(PCI_CONFIG_ADRESS, address);
    unsigned int data = inl(PCI_CONFIG_DATA);
    return data;
}
