// demos.c - text-mode UI and pixel-graphics demos.
// These need direct VGA / hardware access, so they stay in the kernel and the
// Ring 3 shell starts them through syscalls 30-33.
#include "demos.h"
#include "../drivers/vga.h"
#include "../drivers/gfx/gfx.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "io.h"

// Block until a key is pressed.
static void wait_for_key(void) {
    while (keyboard_read() == 0) {
        halt_cpu();
    }
}

// Print a number at a screen position (text mode).
static void ui_put_uint(int x, int y, unsigned int n, unsigned char color) {
    char tmp[11];
    int len = 0;

    if (n == 0) {
        vga_putc_at(x, y, '0', color);
        return;
    }
    while (n > 0) {
        tmp[len++] = (char)('0' + (n % 10));
        n /= 10;
    }
    for (int i = 0; len > 0; i++) {
        vga_putc_at(x + i, y, tmp[--len], color);
    }
}

// ui demo: a window with a box, a progress bar and a message
void ui_demo(void) {
    unsigned char win  = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    unsigned char edge = VGA_COLOR(VGA_YELLOW, VGA_BLUE);
    unsigned char hi   = VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLUE);

    vga_save_screen();
    vga_hide_cursor();

    vga_draw_window(15, 5, 50, 13, " RisottOS UI demo ", edge, win);
    vga_puts_at(18, 7, "Windows, boxes and progress bars", win);
    vga_draw_box(18, 9, 44, 3, hi, VGA_BOX_SINGLE);
    vga_puts_at(20, 10, "Loading, please wait...", win);

    for (unsigned int p = 0; p <= 100; p += 2) {
        vga_progress_bar(18, 13, 44, p,
                         VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLUE),
                         VGA_COLOR(VGA_DARK_GRAY, VGA_BLUE));
        ui_put_uint(18, 14, p, win);
        vga_puts_at(18 + (p >= 100 ? 3 : (p >= 10 ? 2 : 1)), 14, "%  ", win);
        sleep_ticks(3);
    }

    vga_puts_at(18, 16, "Done! Press any key...", edge);
    wait_for_key();

    vga_restore_screen();
    vga_show_cursor();
}

// ui colors: all 16 x 16 foreground/background combinations
void ui_colors(void) {
    vga_save_screen();
    vga_hide_cursor();
    vga_set_blink(0);                  // so backgrounds 8-15 don't blink

    clear_screen();
    vga_puts_at(2, 1, "All 16 x 16 text colors (rows = background, columns = foreground)", 0x0F);
    for (int bg = 0; bg < 16; bg++) {
        for (int fg = 0; fg < 16; fg++) {
            vga_putc_at(10 + fg * 4,     4 + bg, 'A', VGA_COLOR(fg, bg));
            vga_putc_at(10 + fg * 4 + 1, 4 + bg, 'a', VGA_COLOR(fg, bg));
        }
    }
    vga_puts_at(2, 22, "Press any key...", 0x0F);
    wait_for_key();

    vga_set_blink(1);
    vga_restore_screen();
    vga_show_cursor();
}

// gfx demo: a still scene in 320x200 pixel mode
void gfx_demo(void) {
    gfx_enter();

    // sky: 20 bands from dark to light blue
    for (int i = 0; i < 20; i++) {
        gfx_fill_rect(0, i * 8, GFX_W, 8, GFX_RGB(0, i / 8, 2 + i * 3 / 20));
    }
    // ground
    gfx_fill_rect(0, 150, GFX_W, 50, GFX_RGB(0, 3, 0));

    // sun
    gfx_fill_circle(250, 45, 22, GFX_YELLOW);
    gfx_circle(250, 45, 26, GFX_BROWN);

    // mountains
    gfx_line(0, 150, 70, 90, GFX_DARK_GRAY);
    gfx_line(70, 90, 140, 150, GFX_DARK_GRAY);
    gfx_line(110, 150, 180, 100, GFX_LIGHT_GRAY);
    gfx_line(180, 100, 250, 150, GFX_LIGHT_GRAY);

    // a house
    gfx_fill_rect(20, 160, 50, 30, GFX_RGB(4, 2, 0));
    gfx_line(15, 160, 45, 140, GFX_RED);
    gfx_line(45, 140, 75, 160, GFX_RED);
    gfx_fill_rect(40, 172, 10, 18, GFX_RGB(2, 1, 0));
    gfx_rect(54, 166, 10, 10, GFX_WHITE);

    // gray ramp and color cube strips
    for (int i = 0; i < 16; i++) gfx_fill_rect(100 + i * 12, 160, 12, 8, GFX_GRAY(i));
    for (int i = 0; i < 36; i++) gfx_fill_rect(100 + i * 5, 172, 5, 8, (unsigned char)(32 + i * 6));

    // text on a title bar
    gfx_fill_rect(0, 0, GFX_W, 20, GFX_BLACK);
    gfx_text(8, 2, "RisottOS 320x200x256", GFX_WHITE, -1);
    gfx_text(8, 182, "Press any key to return", GFX_YELLOW, -1);

    wait_for_key();
    gfx_exit();
}

// gfx ball: a bouncing ball, runs until a key is pressed
void gfx_ball(void) {
    int x = 60, y = 50, dx = 1, dy = 1;     // pixels per frame (was 3 and 2)
    const int r = 10;

    gfx_enter();
    gfx_fill_rect(0, 0, GFX_W, GFX_H, GFX_GRAY(1));
    gfx_rect(0, 0, GFX_W, GFX_H, GFX_WHITE);
    gfx_text(8, 4, "Bouncing ball - press any key", GFX_WHITE, -1);

    while (keyboard_read() == 0) {
        // erase a square slightly larger than the ball so the outline can't leave dots
        gfx_fill_rect(x - r - 1, y - r - 1, 2 * r + 3, 2 * r + 3, GFX_GRAY(1));

        x += dx;
        y += dy;
        if (x < r + 1)          { x = r + 1;          dx = -dx; }
        if (x > GFX_W - r - 2)  { x = GFX_W - r - 2;  dx = -dx; }
        if (y < r + 22)         { y = r + 22;         dy = -dy; }
        if (y > GFX_H - r - 2)  { y = GFX_H - r - 2;  dy = -dy; }

        gfx_fill_circle(x, y, r, GFX_LIGHT_RED);
        gfx_circle(x, y, r, GFX_WHITE);
        gfx_wait_vsync();
    }
    gfx_exit();
}