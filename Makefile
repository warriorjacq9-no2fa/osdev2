INCLUDES := -I$(CURDIR)/kernel/arch/i386/include

CFLAGS += -m32 -Os -ffreestanding -Wall -Wextra $(INCLUDES)
CC = gcc

AFLAGS += -felf32
AS = nasm

LD = gcc
LDFLAGS = -O2 -nostdlib -ffreestanding -m32

ARCH = i386

# Export variables for subdirectories
export CFLAGS
export CC

export AFLAGS
export AS

export LD
export LDFLAGS

export ARCH

.PHONY: all build clean test
all: build

build:
	$(MAKE) -C kernel build
	$(MAKE) -C libk build

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C libk clean

test: build
	echo "Test!"