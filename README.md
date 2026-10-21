# RisottOS

RisottOS is a small educational operating system for 32-bit x86 PCs. It
boots through BIOS, switches to protected mode, and runs a freestanding C
kernel in QEMU.

## Current state

- BIOS bootloader with INT 13h disk loading
- 32-bit protected mode with GDT, IDT, PIC, and TSS
- VGA text output, PIT timer, PS/2 keyboard, RTC, and ATA PIO support
- Basic memory manager and filesystem code
- Ring 3 transition with an interactive keyboard shell through `int 0x80`

The current kernel launches the Ring 3 test application after boot. The user
program polls keyboard characters through syscall 2 and echoes them
back to the screen. Paging,
process isolation, a standard library, and a complete userspace are not
implemented yet.

## Requirements

- `i686-elf-gcc`
- `i686-elf-ld`
- NASM
- QEMU (`qemu-system-i386`)

## Build and run

```sh
make
make run
```

The build creates `os.img` in the project root. To clean generated files:

```sh
make clean
```

## Project structure

```text
src/boot.asm              BIOS bootloader
src/kernel.c              Kernel initialization and Ring 3 test
src/kernel_entry.asm      Kernel entry point
src/arch/                 GDT, IDT, interrupts, syscalls, and shell code
src/drivers/              VGA, keyboard, timer, RTC, ATA, and graphics
src/mem/                  Memory manager
src/rfs/                  Filesystem code
```

RisottOS currently targets BIOS-based i386-compatible systems and is mainly
tested with QEMU. It is an educational project under active development.

## License

See [LICENSE](LICENSE).
