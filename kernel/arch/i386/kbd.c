#include <string.h>
#include <stdio.h>
#include "kbd.h"

static void (*kcallback)(char, unsigned char);
static unsigned char kstate = 0;

const char keymap[] = {
    '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0',
    '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0',
    '*', '\0', ' '
};
const char keymap_shift[] = {
    '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0',
    '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0',
    '*', '\0', ' '
};

void keyboard() {
    unsigned char sc = inb(0x60);

    switch (sc) {
        /* Shift pressed */
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            kstate |= KBD_SHIFT;
            break;

        /* Shift released */
        case 0xAA:
        case 0xB6:
            kstate &= ~KBD_SHIFT;
            break;

        /* Caps Lock pressed */
        case 0x3A:
            kstate ^= KBD_CAPS; // toggle
            break;

        default:
            if (!(sc & 0x80)) {
                char ch;

                if (kstate & KBD_SHIFT)
                    ch = keymap_shift[sc - 1];
                else
                    ch = keymap[sc - 1];

                if (ISALPHA(ch) && (kstate & KBD_CAPS)) {
                    if (ISLOWER(ch))
                        ch -= 32;
                    else
                        ch += 32;
                }

                if (ch) kcallback(ch, kstate);
            }
            break;
    }

    outb(0x20, 0x20); // PIC EOI
}

void keyboard_init(void (*kcall)(char, unsigned char)) {
    kcallback = kcall;
}