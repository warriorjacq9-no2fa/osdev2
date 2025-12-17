#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/arch.h>
#include <kernel/kbd.h>

#define PROMPT_LEN 1

#define CMD_PEEK    1
#define CMD_POKE    2

char shellbuf[256];
static uint8_t shellptr = 0;
uint8_t sy;

void shell_init() {
    memset(shellbuf, 0, 256);
    sy = ((getpos() & 0xFF00) >> 8);
    puts(">");
}

uint8_t shell_hash(char* cmd) {
    if(strcmp(cmd, "peek") == 0)    return CMD_PEEK;
    if(strcmp(cmd, "poke") == 0)    return CMD_POKE;
    return 0;
}

void shell_proc() {
    char* word = strtok(shellbuf, " ");
    switch(shell_hash(word)) {
        case CMD_PEEK:
            char* arg = strtok(NULL, " ");
            if(arg == NULL) {
                printf("Usage: peek <location>");
                break;
            }
            char* end;
            int mem = strtoul(arg, &end, 16);
            printf("%08X: %02X", mem, *(uint8_t*)mem);
            break;
        case CMD_POKE:
            arg = strtok(NULL, " ");
            if(arg == NULL) {
                printf("Usage: poke <location> <value>");
                break;
            }
            mem = strtoul(arg, &end, 16);

            arg = strtok(NULL, " ");
            if(arg == NULL) {
                printf("Usage: poke <location> <value>");
                break;
            }
            int val = strtoul(arg, &end, 16);

            printf("%08X = %02X", mem, val);
            *(uint8_t*)mem = val;
            break;
        default:
            printf("Unknown command: %s", word);
            break;
    }
    printf("\n");
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