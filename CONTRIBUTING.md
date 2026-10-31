# Contributing to RisottOS

First of all, thank you for your interest in contributing to **RisottOS**!

RisottOS is an educational hobby operating system developed collaboratively by two developers. Our goal is to learn about operating system internals by building components from scratch.

We welcome contributions from developers, OSDev enthusiasts, students, and anyone interested in low-level programming.

You don't need to be an operating system expert to contribute!

## Ways to Contribute

There are several ways you can help the project:

- Report bugs and unexpected behavior.
- Suggest new features and improvements.
- Review existing kernel code.
- Improve memory management.
- Work on hardware drivers.
- Help improve the bootloader.
- Contribute to filesystem development.
- Improve documentation.
- Share architectural suggestions and technical feedback.

Even small contributions are appreciated.

## Before You Start

Before making changes:

1. Read the project's [README](README.md).
2. Check existing [Issues](https://github.com/risottos-team/risottos/issues).
3. Check existing Pull Requests to avoid duplicate work.
4. For major changes, open an issue first to discuss your proposal with the maintainers.

We recommend discussing significant architectural changes before implementing them.

## Development Environment

RisottOS currently targets **32-bit x86 (i386)**.

The development environment uses:

- C (freestanding)
- x86 Assembly (NASM)
- `i686-elf-gcc`
- `i686-elf-ld`
- GNU Make
- QEMU (`qemu-system-i386`)

A working cross-compilation toolchain is recommended.

### Building the Project

Clone your fork:

```bash
git clone https://github.com/YOUR_USERNAME/risottos.git
cd risottos
```

Build the project:

```bash
make
```

Run it using QEMU:

```bash
make run
```

Clean generated files:

```bash
make clean
```

Before submitting a Pull Request, make sure the project builds successfully and test your changes whenever possible.

## Code Guidelines

To keep the codebase maintainable:

- Write readable and understandable code.
- Use meaningful variable and function names.
- Follow the existing project structure.
- Keep kernel code freestanding.
- Avoid unnecessary external dependencies.
- Document complex or architecture-specific implementations.
- Be careful when modifying interrupt handlers, paging, and memory management.
- Avoid unrelated changes in your Pull Request.

When working with low-level code, explain important assumptions and hardware-specific behavior in comments.

## Branching and Commit Messages

Create a separate branch for each contribution.

Example:

```bash
git checkout -b feature/process-management
```

Use descriptive commit messages.

Examples:

```text
feat: add process management
fix: correct page fault handling
docs: improve memory management documentation
refactor: reorganize interrupt handlers
```

## Pull Request Process

1. Fork the repository.
2. Create a new branch.
3. Implement your changes.
4. Build the project.
5. Test your changes in QEMU when applicable.
6. Commit your changes with a descriptive message.
7. Push your branch to your fork.
8. Open a Pull Request against the main RisottOS repository.

### Pull Request Description

Please explain:

- What you changed.
- Why the change is necessary.
- How you implemented it.
- How you tested it.
- Whether it introduces any known limitations.

For changes involving booting, interrupts, or kernel output, screenshots and logs are helpful.

## Bug Reports

When reporting a bug, please include:

- A clear description of the issue.
- Steps to reproduce it.
- Expected behavior.
- Actual behavior.
- Host operating system.
- QEMU version or other relevant environment information.
- Build output, logs, or screenshots when available.

Please check existing issues before opening a new one.

## Feature Requests

We are open to new ideas and improvements.

When suggesting a feature, please describe:

- What the feature should do.
- Why it would be useful.
- How it might fit into the current architecture.
- Any possible limitations or design considerations.

Major features may require discussion before implementation.

## Discussions and Feedback

You are welcome to share:

- Kernel architecture suggestions.
- Memory management ideas.
- Driver implementation approaches.
- OSDev resources and references.
- Performance and reliability improvements.

Constructive criticism is always welcome. We are still learning and improving the project.

## Code of Conduct

Please be respectful and constructive when interacting with maintainers and other contributors.

- Be welcoming to developers of all experience levels.
- Focus on technical discussion.
- Respect different approaches and opinions.
- Provide constructive feedback.
- Avoid personal attacks or disrespectful behavior.

## Important Notes

RisottOS is an experimental educational project.

It is not intended for production use, and its architecture may change significantly during development.

Submitting a contribution does not guarantee that it will be merged. Contributions are reviewed based on their relevance, correctness, maintainability, and compatibility with the project's direction.

## Thank You!

Every contribution helps RisottOS improve.

Whether you report a bug, suggest an idea, review code, improve documentation, or implement a new feature, we appreciate your interest in the project.

**Thank you for helping us build RisottOS!**
