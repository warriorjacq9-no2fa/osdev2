# OSDev 2
This is my second operating system project, and it will probably be my only one from now on. I expect it to target any major architecture that is used for mass storage or other I/O-bound server applications.
Currently, it is targeting x86 and ARM SystemReady SR, and I might expand RISC-V support as well.

## Building

This project uses `make`. By default, it targets i386, but other targets can be set by setting the ARCH environment variable. Possible values are:
 - i386
 - arm-sr
 - riscv64