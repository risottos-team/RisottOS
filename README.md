# RisottOS

<p align="center">
  <strong>A hobby operating system built from scratch for 32-bit x86 systems.</strong>
</p>

<p align="center">
  <a href="https://github.com/risottos-team/risottos">GitHub Repository</a> ·
  <a href="https://github.com/risottos-team/risottos/issues">Issues</a> ·
  <a href="CONTRIBUTING.md">Contributing</a>
</p>

---

## About

**RisottOS** is an educational hobby operating system developed from scratch for 32-bit x86 PCs.

The project is developed collaboratively by two developers. Our goal is to understand how operating systems work internally by implementing low-level components ourselves, from the boot process to kernel functionality and userspace.

RisottOS currently boots through BIOS, enters 32-bit protected mode, and runs a freestanding C kernel in QEMU.

This is an experimental project under active development. It is not intended for daily use.

## Current Status

The following components have been implemented:

| Component | Status |
|---|---|
| Custom BIOS bootloader | Implemented |
| BIOS disk loading (INT 13h) | Implemented |
| 32-bit Protected Mode | Implemented |
| Global Descriptor Table (GDT) | Implemented |
| Interrupt Descriptor Table (IDT) | Implemented |
| Programmable Interrupt Controller (PIC) | Implemented |
| Task State Segment (TSS) | Implemented |
| VGA text output | Implemented |
| PIT timer | Implemented |
| PS/2 keyboard input | Implemented |
| Shift and Caps Lock support | Implemented |
| Real-Time Clock (RTC) | Implemented |
| ATA PIO support | Implemented |
| Basic memory manager | Implemented |
| Identity-mapped paging | Implemented |
| Page-fault reporting | Implemented |
| Ring 3 transition | Implemented |
| Basic system calls (`int 0x80`) | Implemented |
| Interactive userspace shell | Basic implementation |

### Current Userspace Implementation

The kernel currently launches a Ring 3 shell after boot.

The userspace program reads keyboard characters through syscall 2 and echoes them to the screen.

The current paging implementation provides a 16 MiB identity-mapped bootstrap address space:

- The first 4 MiB is supervisor-only.
- The Ring 3 program, its stack, and its dedicated user heap occupy user-accessible mappings.

**Current limitations:**

- Full process isolation is not implemented.
- A standard C library is not available.
- A complete userspace environment does not exist yet.
- The current memory layout is a bootstrap implementation, not a complete virtual memory system.

We are actively working to improve these areas.

## Roadmap

The following roadmap describes our planned development direction. Features may be adjusted as the project evolves.

| Feature | Status |
|---|---|
| BIOS bootloader | Completed |
| Protected mode | Completed |
| GDT and IDT | Completed |
| Hardware interrupt handling | Completed |
| Basic memory management | Completed |
| Paging initialization | Completed |
| Ring 3 transition | Completed |
| Basic syscall interface | Completed |
| Process Control Blocks (PCB) | Planned |
| Process creation and termination | Planned |
| Per-process address spaces | Planned |
| Context switching | Planned |
| Basic multitasking | Planned |
| Improved physical memory allocator | Planned |
| Dynamic kernel heap | Planned |
| Advanced virtual memory management | Planned |
| Memory protection improvements | Planned |
| Improved ATA driver | Planned |
| RFS improvements | Planned |
| Virtual Filesystem (VFS) | Planned |
| File and directory operations | Planned |
| ELF executable loader | Planned |
| Expanded syscall interface | Planned |
| Basic C library | Planned |
| Independent userspace programs | Planned |
| PCI device discovery | Planned |
| Improved hardware drivers | Planned |
| Graphics subsystem improvements | Planned |

For individual tasks and ongoing development, check our [GitHub Issues](https://github.com/risottos-team/risottos/issues).

## Building and Running

### Requirements

You will need:

- `i686-elf-gcc`
- `i686-elf-ld`
- NASM
- GNU Make
- QEMU (`qemu-system-i386`)

A working 32-bit cross-compilation toolchain is recommended.

### Clone the repository

```bash
git clone https://github.com/risottos-team/risottos.git
cd risottos
```

### Build

```bash
make
```

This generates `os.img` in the project root.

### Run with QEMU

```bash
make run
```

### Clean generated files

```bash
make clean
```

### VirtualBox

The Makefile also contains an optional `vdi` target for converting the raw disk image into VirtualBox VDI format.

The current VirtualBox conversion configuration uses Windows-specific paths and may need adjustments depending on your environment.

QEMU is the primary documented testing environment.

## Project Structure

```text
RisottOS/
├── config/              # Linker configuration
├── docs/                # Project documentation
├── scripts/             # Development scripts
├── src/
│   ├── boot.asm         # BIOS bootloader
│   ├── kernel.c         # Kernel initialization
│   ├── kernel_entry.asm # Kernel entry point
│   ├── arch/            # GDT, IDT, interrupts and syscalls
│   ├── drivers/         # Hardware and graphics drivers
│   ├── mem/             # Memory management and paging
│   ├── pci/             # PCI-related code
│   └── rfs/             # Filesystem implementation
├── init.c               # Userspace initialization program
├── Makefile
├── LICENSE
└── README.md
```

## Contributing

We welcome suggestions, bug reports, code reviews, and contributions!

If you are interested in operating system development, low-level programming, or kernel architecture, feel free to explore the project.

Before submitting code, please read [CONTRIBUTING.md](CONTRIBUTING.md).

You can also open an issue to discuss a bug, suggest a feature, or ask a technical question.

## Maintainers

RisottOS is currently maintained by:

- [Risotto](https://github.com/myakupozluk)
- [Dev10n](https://github.com/dev10n)

## License

RisottOS is licensed under the GNU General Public License v3.0.

See [LICENSE](LICENSE) for details.

---

**RisottOS is a learning project. We build it to understand what's happening beneath the software we use every day.**
