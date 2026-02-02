#ifndef KBD_H
#define KBD_H 1

#define KBD_SHIFT   0x01
#define KBD_CAPS    0x02
#define KBD_BCKSP   0x04

#define KBD_UP      0x10
#define KBD_DOWN    0x20
#define KBD_LEFT    0x40
#define KBD_RIGHT   0x80

void keyboard_init(void (*kcall)(char, unsigned char));

#endif