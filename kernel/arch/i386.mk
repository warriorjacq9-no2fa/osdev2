CFLAGS += -m32 -march=i386 -static -Os -fno-stack-protector -fno-pie -ffreestanding -Wall -Wextra
CC = gcc

AR = ar

AFLAGS += -felf32
AS = nasm

LD = gcc
LDFLAGS = -Os -nostdlib -fno-stack-protector -ffreestanding -m32 -static

QEMU = qemu-system-i386