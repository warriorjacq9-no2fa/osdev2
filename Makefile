CFLAGS += -m32 -march=i386 -static -Os -fno-stack-protector -fno-pie -ffreestanding -Wall -Wextra
CC = gcc

AR = ar

AFLAGS += -felf32
AFLAGS_BIN += -fbin
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
export AFLAGS_BIN
export AS

export LD
export LDFLAGS

export ARCH
export TARGET

.PHONY: all build clean test
all: build os.img

os.img: libk/libk.a kernel/kernel.img
	cp $(lastword $^) $@

build:
	$(MAKE) -C libk libk.a
	$(MAKE) -C kernel kernel.img

clean:
	rm -rf *.log *.img isodir
	$(MAKE) -C kernel clean
	$(MAKE) -C libk clean

test: build os.img
	qemu-system-i386 -fda os.img -d int -D qemu.log -no-reboot -no-shutdown