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
void pci_write32(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned int value){
    unsigned int address = 0;
    address = (ENABLE_BIT) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (offset & 0xFC);
    outl(PCI_CONFIG_ADRESS, address);
    outl(PCI_CONFIG_DATA, value);
}
int pci_get_bar(unsigned int bus, unsigned int device, unsigned int function,
                unsigned int bar_index, pci_bar *out){
    if(out == 0 || bar_index > 5){
        return -1;
    }
    unsigned int offset = PCI_BAR0 + 4 * bar_index;
    unsigned int original = pci_read32(bus, device, function, offset);
    if(original & 1){
        return -1;                      // I/O BAR
    }
    if(((original >> 1) & 3) == 2){
        return -1;                      // 64-bit BAR
    }

    unsigned int command = pci_read32(bus, device, function, 0x04) & 0xFFFF;
    pci_write32(bus, device, function, 0x04, command & ~0x2u);
    pci_write32(bus, device, function, offset, 0xFFFFFFFF);
    unsigned int readback = pci_read32(bus, device, function, offset);
    pci_write32(bus, device, function, offset, original);
    pci_write32(bus, device, function, 0x04, command);

    unsigned int mask = readback & ~0xFu;
    unsigned int base = original & ~0xFu;
    if(mask == 0 || base == 0){
        return -1;
    }

    out->bar  = original;               // or bar_index, whichever you chose
    out->base = base;
    out->size = ~mask + 1;
    return 0;
}