# RisottOS

<p align="center">
  <strong>A hobby operating system built from scratch for x86-64 systems.</strong>
</p>

<p align="center">
  <a href="https://github.com/risottos-team/risottos">GitHub Repository</a> ·
  <a href="https://github.com/risottos-team/risottos/issues">Issues</a> ·
  <a href="CONTRIBUTING.md">Contributing</a>
</p>

---
## About RisottOS
RisottOS is a hobby OS project from scratch that aims to make its developers learn about x86-64 architecture, as well as low-level systems, how modern Operating Systems work etc. If you want to be a part of this project, check out [Contributing](#Contributing).

## Get Started
To boot RisottOS, you have 2 options:
- 64-bit version(in Development)
- 32-bit version

The 64-bit version uses Limine to boot from, but after boot its just a blank screen.

The 32-bit version, however, boots from BIOS and is booting from .img file instead and is the functionable version of RisottOS.

## Installation

To clone the repository:
`git clone https://github.com/risottos-team/RisottOS`

Submodules:
`git submodule update --init --recursive --depth 1`

## Tools Needed
In General:
- NASM
- QEMU
- GNU MAKE


For x86:
- `i686-elf-gcc` - Compiler
- `i686-elf-ld` - Linker

For x86-64:
- `x86_64-elf-gcc` - Compiler for x86-64 code
- `x86_64-elf-ld` - Linker

> [!NOTE]
> Windows users may need to install these compilers through MSYS2 or a similar environment, since they aren't native to Windows and have to be downloaded from separate build platforms.

After you download the repository, you can compile it with either:

- `make run-x86_64` - Compiles 64-bit version
- `make run` - Compiles 32-bit version

After that, QEMU window should pop up with RisottOS running.

## Contributing

RisottOS is developed by two people, which makes its development very slow. We, as the RisottOS team, would love your additions to our hobby OS as well as it helps this project grow bigger.

Before submitting code, please read [CONTRIBUTING.md](CONTRIBUTING.md).

## As for the weird VirtualBox vdi maker line you would think about:

It is optional but we recommend you that you just simply dont touch it unless you need .vdi of 32-bit version.

## Maintainers

RisottOS is currently maintained by:

- [Risotto](https://github.com/myakupozluk)
- [Dev10n](https://github.com/dev10n)

## License

RisottOS is licensed under the GNU General Public License v3.0.

See [LICENSE](LICENSE) for details.

