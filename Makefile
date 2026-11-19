.RECIPEPREFIX := >

# ============================================================
# RisottOS - x86_32 BIOS + x86_64 Limine
# ============================================================

BUILD := build

# ============================================================
# x86_32 - BIOS
# ============================================================

CC      := gcc
LD      := ld
AS      := nasm
QEMU    := qemu-system-i386

X86_32_BUILD := $(BUILD)/x86_32
OS_IMG       := os.img

BOOT_SRC    := src/boot/bios/boot.asm
KERNEL_ADDR := 0x1000

X86_32_CFLAGS := -m32 -std=gnu99 -ffreestanding -fno-pic \
    -fno-stack-protector -fno-builtin \
    -fno-asynchronous-unwind-tables -fno-unwind-tables \
    -O0 -Wall -Wextra -Isrc

X86_32_C_SRC := \
    src/kernel/x86_32/kernel.c \
    $(wildcard src/arch/x86_32/*.c) \
    $(wildcard src/drivers/x86_32/*/*.c) \
    $(wildcard src/mm/x86_32/*.c) \
    $(wildcard src/fs/rfs/*.c)

X86_32_OBJ := $(patsubst src/%.c,$(X86_32_BUILD)/%.o,$(X86_32_C_SRC))

X86_32_ASM_SRC := \
    src/boot/bios/kernel_entry.asm \
    src/arch/x86_32/interrupts.asm

X86_32_ASM_OBJ := $(patsubst src/%.asm,$(X86_32_BUILD)/%.asm.o,$(X86_32_ASM_SRC))

X86_32_BOOT_BIN := $(X86_32_BUILD)/boot.bin
X86_32_KERNEL   := $(X86_32_BUILD)/kernel.bin
X86_32_INIT_OBJ := $(X86_32_BUILD)/init.o
X86_32_INIT_BIN := $(X86_32_BUILD)/init.bin


# ============================================================
# x86_64 - Limine
# ============================================================

X86_64_CC   := gcc
X86_64_LD   := ld
X86_64_QEMU := qemu-system-x86_64

X86_64_BUILD := $(BUILD)/x86_64
X86_64_ELF   := $(X86_64_BUILD)/risottos.elf
X86_64_ISO   := $(X86_64_BUILD)/risottos.iso

LIMINE_DIR := boot/limine
ISO_ROOT   := iso_root

X86_64_CFLAGS := -m64 -std=gnu11 -ffreestanding \
    -fno-stack-protector -fno-pic -fno-pie \
    -fno-toplevel-reorder -mno-red-zone \
    -mcmodel=kernel -Wall -Wextra -O2 \
    -Isrc -I$(LIMINE_DIR)

X86_64_C_SRC := \
    $(wildcard src/kernel/x86_64/*.c) \
    $(wildcard src/arch/x86_64/*.c) \
    $(wildcard src/arch/x86_64/io/*.c) \
    $(wildcard src/drivers/x86_64/*/*.c)

X86_64_OBJ := $(patsubst src/%.c,$(X86_64_BUILD)/%.o,$(X86_64_C_SRC))

X86_64_ASM_SRC := $(wildcard src/arch/x86_64/*.asm)
X86_64_ASM_OBJ := $(patsubst src/%.asm,$(X86_64_BUILD)/%.asm.o,$(X86_64_ASM_SRC))

X86_64_LDFLAGS := -m elf_x86_64 -nostdlib -static \
    -z max-page-size=0x1000 \
    -T config/x86_64.ld


# ============================================================
# Default target
# ============================================================

all: $(OS_IMG)


# ============================================================
# x86_32 build rules
# ============================================================

$(X86_32_BUILD)/%.o: src/%.c
>@mkdir -p $(dir $@)
>$(CC) $(X86_32_CFLAGS) -c $< -o $@

$(X86_32_BUILD)/%.asm.o: src/%.asm
>@mkdir -p $(dir $@)
>$(AS) -f elf32 $< -o $@

$(X86_32_BOOT_BIN): $(BOOT_SRC)
>@mkdir -p $(dir $@)
>$(AS) -f bin $< -o $@

$(X86_32_KERNEL): $(X86_32_ASM_OBJ) $(X86_32_OBJ)
>@mkdir -p $(dir $@)
>$(LD) -m elf_i386 -Ttext $(KERNEL_ADDR) \
>    --oformat binary -o $@ \
>    $(X86_32_BUILD)/boot/bios/kernel_entry.asm.o \
>    $(filter-out $(X86_32_BUILD)/boot/bios/kernel_entry.asm.o,$^)

$(X86_32_INIT_OBJ): init.c
>@mkdir -p $(dir $@)
>$(CC) $(X86_32_CFLAGS) -c $< -o $@

$(X86_32_INIT_BIN): $(X86_32_INIT_OBJ)
>$(LD) -m elf_i386 -T config/init.ld $< -o $@

$(OS_IMG): $(X86_32_BOOT_BIN) $(X86_32_KERNEL) $(X86_32_INIT_BIN)
>cat $(X86_32_BOOT_BIN) $(X86_32_KERNEL) > $@
>truncate -s 64M $@
>dd if=$(X86_32_INIT_BIN) of=$@ bs=512 seek=200 conv=notrunc

run: all
>$(QEMU) -drive file=$(OS_IMG),format=raw,if=ide,index=0,media=disk

mem:
>@echo "x86_32 memory manager sources:"
>@printf '%s\n' $(filter %/mm/x86_32/%.o,$(X86_32_OBJ))

VBOXMANAGE := /c/Program Files/Oracle/VirtualBox/VBoxManage.exe
VDI_DIR    := /d/VirtualBox
VDI_IMG    := $(VDI_DIR)/RisottOS.vdi

vdi: $(OS_IMG)
>@test -f "$(VBOXMANAGE)" || (echo "ERROR: VBoxManage.exe not found"; exit 1)
>mkdir -p "$(VDI_DIR)"
>"$(VBOXMANAGE)" convertfromraw "$(OS_IMG)" "$(VDI_IMG)" --format VDI


# ============================================================
# x86_64 build rules
# ============================================================

$(X86_64_BUILD)/%.o: src/%.c
>@mkdir -p $(dir $@)
>$(X86_64_CC) $(X86_64_CFLAGS) -c $< -o $@

$(X86_64_BUILD)/%.asm.o: src/%.asm
>@mkdir -p $(dir $@)
>$(AS) -f elf64 $< -o $@

$(X86_64_ELF): $(X86_64_OBJ) $(X86_64_ASM_OBJ) config/x86_64.ld
>@mkdir -p $(dir $@)
>$(X86_64_LD) $(X86_64_LDFLAGS) $(X86_64_OBJ) $(X86_64_ASM_OBJ) -o $@

$(LIMINE_DIR)/limine:
>$(MAKE) -C $(LIMINE_DIR)

$(ISO_ROOT)/boot/risottos.elf: $(X86_64_ELF) \
    $(LIMINE_DIR)/limine-bios.sys \
    $(LIMINE_DIR)/limine-bios-cd.bin \
    $(LIMINE_DIR)/limine-uefi-cd.bin \
    $(LIMINE_DIR)/BOOTX64.EFI \
    $(ISO_ROOT)/boot/limine/limine.conf
>@mkdir -p $(ISO_ROOT)/boot/limine
>@mkdir -p $(ISO_ROOT)/EFI/BOOT
>cp $(X86_64_ELF) $@
>cp $(LIMINE_DIR)/limine-bios.sys $(ISO_ROOT)/boot/limine/
>cp $(LIMINE_DIR)/limine-bios-cd.bin $(ISO_ROOT)/boot/limine/
>cp $(LIMINE_DIR)/limine-uefi-cd.bin $(ISO_ROOT)/boot/limine/
>cp $(LIMINE_DIR)/BOOTX64.EFI $(ISO_ROOT)/EFI/BOOT/

$(X86_64_ISO): $(ISO_ROOT)/boot/risottos.elf \
    $(ISO_ROOT)/boot/limine/limine.conf \
    $(LIMINE_DIR)/limine
>xorriso -as mkisofs \
>    -R -r -J \
>    -b boot/limine/limine-bios-cd.bin \
>    -no-emul-boot \
>    -boot-load-size 4 \
>    -boot-info-table \
>    -hfsplus \
>    -apm-block-size 2048 \
>    --efi-boot boot/limine/limine-uefi-cd.bin \
>    -efi-boot-part \
>    --efi-boot-image \
>    --protective-msdos-label \
>    $(ISO_ROOT) \
>    -o $@
>$(LIMINE_DIR)/limine bios-install $@

x86_64: $(X86_64_ISO)
>@echo "RisottOS x86-64 ISO built successfully."

run-x86_64: x86_64
>$(X86_64_QEMU) \
>    -M q35 \
>    -m 256M \
>    -cdrom $(X86_64_ISO) \
>    -boot d


# ============================================================
# Clean
# ============================================================

clean:
>rm -rf $(BUILD) $(OS_IMG)

clean-x86_64:
>rm -rf $(X86_64_BUILD) $(X86_64_ISO) \
>    $(ISO_ROOT)/boot/risottos.elf

clean-all: clean clean-x86_64

.PHONY: all run mem vdi x86_64 run-x86_64 clean clean-x86_64 clean-all
