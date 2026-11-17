#include <stdint.h>

void exception_error_handler(uint64_t vector, uint64_t error_code) {
    __asm__ volatile("cli;");
    while (1) {
        __asm__ volatile("hlt;");
    }
}
