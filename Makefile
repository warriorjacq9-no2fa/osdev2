CFLAGS += -m32 -march=i386 -static -Os -fno-stack-protector -ffreestanding -Wall -Wextra $(INCLUDES)
CC = gcc

AR = ar

AFLAGS += -felf32
AS = nasm

LD = gcc
LDFLAGS = -Os -nostdlib -fno-stack-protector -ffreestanding -m32 -static

ARCH ?= i386
TARGET ?= dlx

# Export variables for subdirectories
export CFLAGS
export CC

export AR

export AFLAGS
export AS

export LD
export LDFLAGS

export ARCH
export TARGET

.PHONY: all build clean test
all: build os.iso

os.iso: libk/libk.a kernel/kernel.bin
	mkdir -p isodir/boot/grub
	cp  boot/grub.cfg isodir/boot/grub/grub.cfg
	cp kernel/kernel.bin isodir/boot/
	grub-mkrescue -o $@ isodir

build:
	$(MAKE) -C libk libk.a
	$(MAKE) -C kernel kernel.bin

clean:
	rm -rf *.iso isodir
	$(MAKE) -C kernel clean
	$(MAKE) -C libk clean

test: build os.iso
	qemu-system-i386 -cdrom os.iso -d int -D qemu.log -no-reboot -no-shutdown