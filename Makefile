.RECIPEPREFIX := >

# ============================================================
# RisottOS - 32-bit BIOS + 64-bit Limine
# ============================================================

# 32-bit configuration
CC      := i686-elf-gcc
LD      := i686-elf-ld
AS      := nasm
QEMU    := qemu-system-i386

BUILD   := build
OS_IMG  := os.img

VBOXMANAGE := /c/Program Files/Oracle/VirtualBox/VBoxManage.exe
VDI_DIR    := /d/VirtualBox
VDI_IMG    := $(VDI_DIR)/RisottOS.vdi

BOOT_SRC    := src/boot.asm
KERNEL_ADDR := 0x1000

CFLAGS := -m32 -std=gnu99 -ffreestanding -fno-pic \
          -fno-stack-protector -fno-builtin \
          -fno-asynchronous-unwind-tables -fno-unwind-tables \
          -O0 -Wall -Wextra -Isrc

C_SRC := $(wildcard \
    src/*.c \
    src/arch/*.c \
    src/drivers/*.c \
    src/drivers/gfx/*.c \
    src/mem/*.c \
    src/pci/*.c \
    src/rfs/*.c \
    src/serial/*.c \
)

C_OBJ := $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRC))

ASM_OBJ := \
    $(BUILD)/kernel_entry.asm.o \
    $(BUILD)/arch/interrupts.asm.o


# 64-bit configuration
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
    -I$(LIMINE_DIR)

X86_64_LDFLAGS := -m elf_x86_64 -nostdlib -static \
    -z max-page-size=0x1000 \
    -T config/x86_64.ld

X86_64_C_SRC := $(wildcard \
    src/kernel/*.c \
    src/arch/x86_64/*.c \
    src/drivers/framebuffer/*.c \
)

X86_64_OBJ := $(patsubst src/%.c,$(X86_64_BUILD)/%.o,$(X86_64_C_SRC))


# ============================================================
# Default target: original 32-bit system
# ============================================================

all: $(OS_IMG)


# ============================================================
# 32-bit build rules
# ============================================================

$(BUILD)/boot.bin: $(BOOT_SRC)
>@mkdir -p $(dir $@)
>$(AS) -f bin $< -o $@

$(BUILD)/%.o: src/%.c
>@mkdir -p $(dir $@)
>$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.asm.o: src/%.asm
>@mkdir -p $(dir $@)
>$(AS) -f elf32 $< -o $@

$(BUILD)/kernel.bin: $(ASM_OBJ) $(C_OBJ)
>$(LD) -m elf_i386 -Ttext $(KERNEL_ADDR) \
>    --oformat binary -o $@ \
>    $(BUILD)/kernel_entry.asm.o \
>    $(filter-out $(BUILD)/kernel_entry.asm.o,$^)

$(BUILD)/init.bin: init.c
>@mkdir -p $(dir $@)
>$(CC) $(CFLAGS) -c init.c -o $(BUILD)/init.o
>$(LD) -m elf_i386 -T config/init.ld $(BUILD)/init.o -o $@

$(OS_IMG): $(BUILD)/boot.bin $(BUILD)/kernel.bin $(BUILD)/init.bin
>cat $(BUILD)/boot.bin $(BUILD)/kernel.bin > $@
>truncate -s 64M $@
>dd if=$(BUILD)/init.bin of=$@ bs=512 seek=200 conv=notrunc

run: all
>$(QEMU) -drive file=$(OS_IMG),format=raw,if=ide,index=0,media=disk

mem: $(filter $(BUILD)/mem/%.o,$(C_OBJ))
>@echo "Memory manager components compiled successfully."

vdi: $(OS_IMG)
>@test -f "$(VBOXMANAGE)" || (echo "ERROR: VBoxManage.exe not found"; exit 1)
>mkdir -p "$(VDI_DIR)"
>"$(VBOXMANAGE)" convertfromraw "$(OS_IMG)" "$(VDI_IMG)" --format VDI


# ============================================================
# 64-bit build rules
# ============================================================

$(X86_64_BUILD)/%.o: src/%.c
>@mkdir -p $(dir $@)
>$(X86_64_CC) $(X86_64_CFLAGS) -c $< -o $@

$(X86_64_ELF): $(X86_64_OBJ) config/x86_64.ld
>@mkdir -p $(dir $@)
>$(X86_64_LD) $(X86_64_LDFLAGS) $(X86_64_OBJ) -o $@

$(LIMINE_DIR)/limine:
>$(MAKE) -C $(LIMINE_DIR)

$(ISO_ROOT)/boot/risottos.elf: $(X86_64_ELF) \
    $(LIMINE_DIR)/limine-bios.sys \
    $(LIMINE_DIR)/limine-bios-cd.bin \
    $(LIMINE_DIR)/limine-uefi-cd.bin \
    $(LIMINE_DIR)/BOOTX64.EFI \
    iso_root/boot/limine/limine.conf
>@mkdir -p $(ISO_ROOT)/boot/limine
>@mkdir -p $(ISO_ROOT)/EFI/BOOT
>cp $(X86_64_ELF) $@
>cp $(LIMINE_DIR)/limine-bios.sys $(ISO_ROOT)/boot/limine/
>cp $(LIMINE_DIR)/limine-bios-cd.bin $(ISO_ROOT)/boot/limine/
>cp $(LIMINE_DIR)/limine-uefi-cd.bin $(ISO_ROOT)/boot/limine/
>cp $(LIMINE_DIR)/BOOTX64.EFI $(ISO_ROOT)/EFI/BOOT/

$(X86_64_ISO): $(ISO_ROOT)/boot/risottos.elf \
    iso_root/boot/limine/limine.conf \
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

.PHONY: all run clean mem vdi x86_64 run-x86_64
