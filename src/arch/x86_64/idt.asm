bits 64
section .text

global idt_load

idt_load:
    lidt [rdi]
    ret
