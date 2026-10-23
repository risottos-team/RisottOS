#include "arch/idt.h"
#include "arch/io.h"
#include "arch/pic.h"
#include "drivers/ata.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "drivers/vga.h"
#include "mem/mem.h"
#include "rfs/rfs.h"

// Size of the init.bin seed area on disk (LBA 200..). 20 sectors = 10 KB.
// Must be >= the size of build/init.bin, or its tail (strings!) is cut off.
#define INIT_SECTORS 20

// Introduce our hardware setup and jump functions to the kernel
extern void c_tss_init(void);
extern void enter_usermode(void);
extern void tss_init(void);

// handles the user input
static void chc_handler(void) {
  while (1) {
    char choice = get_key();

    if (choice == 'f' || choice == 'F') {
      rfs_format();
    } else if (choice != 'n' && choice != 'N') {
      continue;
    }

    return;
  }
}

// checks if the drive is formatted
static void format_check(void) {
  unsigned char win = VGA_COLOR(VGA_WHITE, VGA_BLUE);
  unsigned char edge = VGA_COLOR(VGA_YELLOW, VGA_BLUE);

  vga_save_screen();
  vga_hide_cursor();

  vga_draw_window(0, 0, 80, 25, " RisottOS FileSystem Setup ", edge, win);
  vga_puts_at(
    1, 2, "Your Drive has not been formatted yet, Do you wish to format it?",
    win);
  vga_puts_at(3, 23, "F-Format", edge);
  vga_puts_at(16, 23, "N-No", edge);
  chc_handler();

  vga_restore_screen();
  vga_show_cursor();
}

void load_and_run_app(const char *filename) {
  int file_size = rfs_size(filename);
  if (file_size <= 0) {
    print_string("v1.0 Exec Error: Application not found or empty!\n", 0x0C);
    return;
  }

  unsigned char *user_space_buffer = (unsigned char *)0x500000;

  int bytes_read =
  rfs_read(filename, user_space_buffer, (unsigned int)file_size);
  if (bytes_read < 0) {
    print_string("v1.0 Exec Error: Failed to read application from RFS!\n", 0x0C);
    return;
  }

  print_string("Successfully Loaded ", 0x0F);
  print_string(filename, 0x0A);
  print_string(" from disk into Ring 3 space. Launching...\n", 0x0F);
  sleep_ticks(100);

  enter_usermode();
}

// Kernel Entry Point
void kmain() {
  clear_screen();
  idt_init();
  c_tss_init();
  tss_init();

  pic_remap();
  __asm__ volatile("sti");

  timer_init(100);
  ata_init();

  mem_init((void *)0x100000, 4 * 1024 * 1024);

  print_string("RisottOS booted successfully!\n", 0x0F);
  sleep_ticks(100);

  // Mount an existing filesystem first. Only show the setup wizard when the
  // disk has no valid RFS filesystem yet.
  int filesystem_ready = (rfs_init() == 0);
  if (!filesystem_ready) {
    format_check();
  }

  keyboard_clear_buffer();

  if (!filesystem_ready) {
    filesystem_ready = (rfs_init() == 0);
  }

  if (!filesystem_ready) {
    print_string("v1.0 Error: RFS could not be mounted after setup.\n", 0x0C);
    while (1) {
      halt_cpu();
    }
  }

  // If /init.bin does not exist in the RFS file table, register it from sector 100
  if (rfs_size("/init.bin") < 0) {
    print_string("v1.0 Setup: Registering /init.bin in RFS file table...\n", 0x0E);

    unsigned short *temp_load_zone = (unsigned short *)0x400000;
    unsigned int init_sector;
    int init_read_failed = 0;

    // The seed image is stored outside the RFS area by the image builder.
    // Load INIT_SECTORS sectors so the complete flat binary is registered.
    for (init_sector = 0; init_sector < INIT_SECTORS; init_sector++) {
      if (ata_read_sector(200 + init_sector,
                          temp_load_zone + (init_sector * 256)) != 0) {
        init_read_failed = 1;
        break;
      }
    }

    int rfs_status = init_read_failed
      ? -1
      : rfs_write("/init.bin", (const void *)0x400000, INIT_SECTORS * 512);
    if (rfs_status < 0) {
      print_string("v1.0 Error: RFS deployment index write failed!\n", 0x0C);
      sleep_ticks(200);
    } else {
      print_string("v1.0 Setup: /init.bin successfully mounted and indexed!\n", 0x02);
    }
    sleep_ticks(50);
  }

  print_string("Preparing to launch RisottOS Real Userspace Shell...\n", 0x0F);
  sleep_ticks(50);

  // Load and execute the system application binary from disk
  load_and_run_app("/init.bin");

  // Infinite safe halt catch in case entry fails
  while (1) {
    halt_cpu();
  }
}