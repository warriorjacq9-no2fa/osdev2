#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/arch.h>
#include <kernel/kbd.h>

#define PROMPT_LEN 1

char shellbuf[256];
static uint8_t shellptr = 0;
uint8_t sy;

void shell_init() {
    memset(shellbuf, 0, 256);
    sy = ((getpos() & 0xFF00) >> 8);
    puts(">");
}

void shell_proc() {
    printf("You said %s!\n", shellbuf);
    memset(shellbuf, 0, 256);
    shellptr = 0;
}

// Keyboard callback
void kcallback(char c, unsigned char kstate) {
    if(c == '\n') {
        // process buffer, clear buffer
        setpos(0, ++sy);
        shell_proc();
        sy++;
        puts(">");
    } else {
        if(kstate & KBD_BCKSP) {
            if(shellptr > 0)
                shellptr--;
            shellbuf[shellptr] = 0;
            clrline(sy);
            setpos(0, sy);
            puts(">");
        } else {
            shellbuf[shellptr++] = c;
            setpos(PROMPT_LEN, sy);
        }
        puts(shellbuf);
    }
}