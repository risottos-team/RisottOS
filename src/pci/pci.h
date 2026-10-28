#ifndef PCI_H
#define PCH_H
// Defines PCI control variables
#define PCI_CONFIG_ADRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC
// Defines PCI registers
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0A
#define PCI_CLASS_CODE 0x0B
#define PCI_HEADER_TYPE 0x0E
#define PCI_BAR0 0x10
#define PCI_INTERRUPT_LINE 0x3C
// Defines PCI Limits
#define PCI_MAX_BUS 256
#define PCI_MAX_DEVICE 32
#define PCI_MAX_FUNCTION 8
#define PCI_VENDOR_NONE 0xFFFF
// Defines PCI classes
#define PCI_CLASS_STORAGE 0x01
#define PCI_CLASS_NETWORK 0x02
#define PCI_CLASS_DISPLAY 0x03
#define PCI_CLASS_MULTIMEDIA 0x04
#define PCI_CLASS_BRIDGE 0x06
#define PCI_CLASS_SERIAL 0x0C
#define PCI_SUBCLASS_USB 0x03

typedef struct {
    unsigned short vendor_id;
    unsigned short device_id;
    unsigned char class_type;
    unsigned char subclass;
    unsigned char bus;
    unsigned char device;
    unsigned char function;
    unsigned char prog_if;
} pci_device;



unsigned int pci_read32(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset);




#endif