CFLAGS += -m32 -march=i386 -static -Os -fno-stack-protector -fno-pie -ffreestanding -Wall -Wextra
CC = aarch64-none-elf-gcc

AR = aarch64-none-elf-ar

AFLAGS += --32
AS = aarch64-none-elf-as

LD = aarch64-none-elf-gcc
LDFLAGS = -Os -nostdlib -fno-stack-protector -ffreestanding -m32 -static

QEMU = qemu-system-aarch64