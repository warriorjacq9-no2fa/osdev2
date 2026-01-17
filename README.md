# OSDev 2
This is my second operating system project, and it will probably be my only one from now on. I expect it to target any major architecture that is used for mass storage or other I/O-bound server applications.
Currently, it is targeting x86 and ARM SystemReady SR, and I might expand RISC-V support as well.

## Building

This project uses `make`. By default, it targets i386, but other targets can be set by setting the ARCH environment variable. Possible values are:
 - i386
 - aarch64
 - riscv64

Additionally, this project has several end targets. The default is nfs4, but others can by set by setting the TARGET environment variable to one of the following:
 - nfs4

The programs required to build this project are:
| Program | Debian/Ubuntu | Arch |
|---------|---------------|------|
| aarch64-linux-gnu | gcc-aarch64-linux-gnu | aarch64-linux-gnu-gcc (AUR) <br/> aarch64-linux-gnu-binutils (AUR) <br/> aarch64-linux-gnu-glibc (AUR) |
| aarch64-none-elf | [Download from here](https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-aarch64-none-elf.tar.xz), and add to PATH | aarch64-none-elf-gcc-bin (AUR) |
| qemu-system | qemu-system | qemu-desktop <br/> qemu-system-aarch64 (for aarch64 target) |
| sgdisk | gdisk | gdisk |
| mcopy, mdir, mmd | mtools | mtools |
| mkfs.vfat | dosfstools | dosfstools |
| AAVMF | qemu-efi-aarch64 | edk2-aarch64 |

## Developing
The job of the kernel, regardless of architecture, is to provide a bare-metal to TCP/IP stack, and pass data to the target subsystem. The target subsystem then has to read the data and take the appropriate action, and pass requests back to the kernel for operations such as disk I/O, text display, etc.