CFLAGS += -m32 -static -Os -ffreestanding -Wall -Wextra $(INCLUDES)
CC = gcc

AFLAGS += -felf32
AS = nasm

LD = gcc
LDFLAGS = -O2 -nostdlib -ffreestanding -m32 -static

ARCH ?= i386
TARGET ?= dlx

# Export variables for subdirectories
export CFLAGS
export CC

export AFLAGS
export AS

export LD
export LDFLAGS

export ARCH
export TARGET

.PHONY: all build clean test
all: build os.iso

os.iso: kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp  boot/grub.cfg isodir/boot/grub/grub.cfg
	cp kernel/kernel.bin isodir/boot/
	grub-mkrescue -o $@ isodir

build:
	$(MAKE) -C kernel kernel.bin

clean:
	rm -rf *.iso isodir
	$(MAKE) -C kernel clean
	$(MAKE) -C libk clean

test: build os.iso
	qemu-system-i386 -cdrom os.iso