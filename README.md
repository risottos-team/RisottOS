# RisottOS

<p align="center">
  <strong>A hobby operating system built from scratch for 64-bit AMD64 systems.</strong>
</p>

<p align="center">
  <a href="https://github.com/risottos-team/risottos">GitHub Repository</a> ·
  <a href="https://github.com/risottos-team/risottos/issues">Issues</a> ·
  <a href="CONTRIBUTING.md">Contributing</a>
</p>

---

## About

**RisottOS** is an educational hobby operating system developed from scratch for 64-bit systems as the project itself targets x86-64.

The project is developed collaboratively by two developers. Our goal is to understand how operating systems work internally by implementing low-level components ourselves, from the boot process to kernel functionality and userspace.

RisottOS now boots through Limine in the latest update and enters 64-bit long mode, but the screen stays blank for now (under development).

You can still use the old 32-bit version via `make run`

This is an experimental project under active development. It is not intended for daily use.

## Current Status

The following components have been implemented in the **32-bit version** (`make run`):

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

The **x86-64 version** (`make run-x86_64`) is still in progress:

| Component | Status |
|---|---|
| Boot through Limine | Implemented |
| 64-bit long mode | Implemented |
| Graphics output | In progress |

### Current Userspace Implementation

The x64 kernel currently launches into a blank screen, but you can still access the old 32-bit one using `make run`.

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
| RFS (our custom filesystem) improvements | Planned |
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
- `x86_64-elf-gcc` for x86-64
- `x86_64-elf-ld`
- NASM
- GNU Make
- QEMU (`qemu-system-i386`)

A working 32-bit cross-compilation toolchain is recommended.

For the x86-64 version you will also need an x86-64 compiler/toolchain, `xorriso`, Limine and `qemu-system-x86_64`.

### Clone the repository

```bash
git clone --recursive https://github.com/risottos-team/risottos.git
cd risottos
```

Limine is included as a git submodule. If you cloned without `--recursive`, run `git submodule update --init` inside the repo.

### Build

```bash
make
```

This generates `os.img` in the project root.

### Run with QEMU

```bash
make run
```
### Run the x64 Version

```bash
make run-x86_64
```


### Clean generated files

```bash
make clean
```

### VirtualBox (optional)

This is completely optional, We recommend you to not touch it unless you need it, it's just there and it doesn't mean you have to touch it.

The Makefile also contains an optional `vdi` target for converting the raw disk image into VirtualBox VDI format.

The current VirtualBox conversion configuration uses Windows-specific paths and may need adjustments depending on your environment.

QEMU is the primary documented testing environment.

## Project Structure

```text
RisottOS/
├── boot/                # boot folder that is needed for limine
├── config/              # Linker configuration
├── docs/                # Project documentation
├── iso_root/            # iso root for limine
│   └── boot/
│       └── limine
├── src/
│   ├── arch/            # GDT, IDT, interrupts and syscalls
│   │   └── x86_64/      # x86-64 (Limine) code
│   ├── boot.asm         # BIOS bootloader
│   ├── kernel.c         # Kernel initialization
│   ├── kernel_entry.asm # Kernel entry point
│   ├── drivers/         # Hardware and graphics drivers
│   ├── mem/             # Memory management and paging
│   ├── pci/             # PCI-related code
│   └── rfs/             # Filesystem implementation
├── init.c               # Userspace initialization program
├── Makefile
├── CONTRIBUTING.md      # A simple guide on how to contribute to our project.
├── compile_flags.txt    # A text file containing some flags about how compiler should behave.
├── LICENSE
└── README.md
```

## Contributing

We are open to suggestions, bug reports, code reviews, and contributions!

Since we are a team of two, we do our best to improve this project as much as we can, and your feedback helps us a lot!

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
