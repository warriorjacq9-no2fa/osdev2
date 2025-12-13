#include <stddef.h>
#include <kernel/arch.h>

int kmain(size_t* mbi) {
    arch_init();

    puts("Hello\nWorld\n");

    for(;;) wait();
}