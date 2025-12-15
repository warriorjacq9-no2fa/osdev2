#include <stdint.h>
#include <string.h>
#include <kernel/arch.h>

#define PROMPT_LEN 1

char shellbuf[256];
static uint8_t shellptr = 0;
uint8_t sy;

void shell_init() {
    memset(shellbuf, 0, 256);
    sy = ((getpos() & 0xFF00) >> 8);
    puts(">");
}

// Keyboard callback
void kcallback(char c, unsigned char kstate) {
    if(c == '\n') {
        // process buffer, clear buffer
        setpos(PROMPT_LEN, sy);
        printf("\nYou said %s\n", shellbuf);
        sy += 2;
        memset(shellbuf, 0, 256);
        shellptr = 0;
        puts(">");
    } else {
        shellbuf[shellptr++] = c;
        setpos(PROMPT_LEN, sy);
        puts(shellbuf);
    }
}