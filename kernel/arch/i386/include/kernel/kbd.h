#ifndef KBD_H
#define KBD_H 1

#define KBD_SHIFT   0x01
#define KBD_CAPS    0x02
#define KBD_BCKSP   0x04

void keyboard_init(void (*kcall)(char, unsigned char));

#endif