# osdev2
This is my second OSDev project, this time without using any templates. This code will use the GRUB bootloader and the Multiboot boot protocol. This code is hosted under the GNU GPLv2 license.

The purpose of this operating system is to provide blazing fast low-level emulation. Currently, I am targeting DLX, but more may follow

This project will be structured to hopefully support easy porting to different computer architectures. All architecture-specific code in subdirs is placed in arch/<architecture>.
Please note, however, that some limitations are present. For example, this operating system cannot be ported to an interrupt-less architecture, or one with a bit size under 32 bits, as those features are essential to basic operation.