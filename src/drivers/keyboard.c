#include "keyboard.h"
#include "../arch/io.h"

#define KEYBOARD_BUFFER_SIZE 128

// Store characters received from keyboard IRQs using a circular buffer.
static volatile char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile unsigned char buffer_head = 0;
static volatile unsigned char buffer_tail = 0;
static volatile char pending_key = 0;
static volatile unsigned char shift_down = 0;
static volatile unsigned char caps_lock = 0;

static const char scancode_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_shifted[] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

static char translate_scancode(unsigned char scancode) {
    char character;

    if (scancode >= sizeof(scancode_to_ascii)) return 0;
    character = (shift_down || caps_lock)
        ? scancode_to_shifted[scancode]
        : scancode_to_ascii[scancode];

    if (caps_lock && shift_down &&
        scancode >= 0x10 && scancode <= 0x32 &&
        ((scancode_to_ascii[scancode] >= 'a' &&
          scancode_to_ascii[scancode] <= 'z'))) {
        character = scancode_to_ascii[scancode];
    }
    return character;
}

static char process_scancode(unsigned char scancode) {
    if (scancode == 0x2A || scancode == 0x36) {
        shift_down = 1;
        return 0;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_down = 0;
        return 0;
    }
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return 0;
    }
    if (scancode & 0x80) return 0;
    return translate_scancode(scancode);
}

// Poll keyboard directly for a single key press.
void keyboard_clear_buffer(void) {
    // Discard all queued characters.
    buffer_tail = buffer_head;
    pending_key = 0;
    shift_down = 0;
    caps_lock = 0;
}
char get_key(void) {
    while (1) {
        // Wait until keyboard buffer has data available on port 0x64.
        while (!(inb(0x64) & 1)) {
        }

        unsigned char scancode = inb(0x60);

        char character = process_scancode(scancode);
        if (character) return character;
    }
}

// Interrupt handler for IRQ1 (keyboard input).
void keyboard_irq_handler(void) {
    // Read the scancode from the data port before acknowledging.
    unsigned char scancode = inb(0x60);

    char character = process_scancode(scancode);
    if (character) {
        pending_key = character;
        unsigned char next_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

        // Store character in circular buffer if space permits.
        if (character && next_head != buffer_tail) {
            keyboard_buffer[buffer_head] = character;
            buffer_head = next_head;
        }
    }
}

// Retrieve the next character from the keyboard input buffer.
char keyboard_read(void) {
    char character;

    if (pending_key != 0) {
        character = pending_key;
        pending_key = 0;
        return character;
    }

    // Return 0 if the buffer is empty.
    if (buffer_head == buffer_tail) {
        return 0;
    }

    // Fetch character from tail and advance pointer.
    character = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return character;
}