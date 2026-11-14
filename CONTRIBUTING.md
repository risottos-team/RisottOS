# Contributing to RisottOS

## First of All:

Thank you for the interest in making **RisottOS** better with us developers, as we would love to see our project get attention by other developers that are experienced enough, or even beginner ones as we aim to build RisottOS with every possible resource we got such as:

- OSDev Wiki
- r/OSDev
- And anyone who is willing to share their knowledge with us RisottOS community.

## Contributing

To contribute to RisottOS and make it a better OS than it is now, you have to have these installed:

- QEMU
- NASM
- GNU Make
- `x86_64-elf-gcc`
- `x86_64-elf-ld`
- `i686-elf-gcc` for 32-bit
- `i686-elf-ld`

## Things you should be aware of/Guidelines:
Be respectful to other developers, as they can also be fellow devs trying to learn low-level systems and just don't know where to start.


- Report any bugs, issues you encounter, or even optimization ideas, we will try to fix it as soon as we find free time to fix.
- Write readable and understandable code.
- Use meaningful variable and function names.
- Follow the existing project structure.
- Keep kernel code freestanding.
- Avoid unnecessary external dependencies.
- We recommend using less AI on code as we try to keep it clean. Use it when you are truly struggling and have no absolute choice.
- Document complex or architecture-specific implementations.
- Be careful when modifying interrupt handlers, paging, and memory management.
- Avoid unrelated changes in your Pull Request.

## How to Contribute:
Check [Issues](https://github.com/risottos-team/RisottOS/issues) to find problems to be fixed. Most of the time we are working on issues but with only two devs to work on, it becomes impossible to maintain all at once.

Create a branch for each contribution you will make/have made as it keeps project clean.

Example:

```bash
git checkout -b feature/process-management
```

Try to be as clean as possible when committing, explain what your addition does so it's easier for other developers/maintainers to understand.

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

## Lastly

In the way of learning how low-level systems work, how modern OSes handle different things, and bunch of different how's , Good Luck! We wish you best in this way and hope that you will soon or late understand how any of those work!
