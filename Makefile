ARCH ?= i386

include kernel/arch/$(ARCH).mk

# Export variables for subdirectories
export CFLAGS CC AR AFLAGS AFLAGS_BIN AS LD LDFLAGS LDFLAGS_BIN QEMU ARCH

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