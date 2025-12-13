.PHONY: build clean

INCLUDES := -I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/include 

LIBS := -L$(CURDIR)/../libk -lk

SRCS = \
kernel.o \
arch/$(ARCH)/boot.o \
arch/$(ARCH)/arch.o \
arch/$(ARCH)/vga.o


kernel.bin: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ $(LIBS) -o kernel.bin
	objdump -s -d kernel.bin > kernel.dump
	grub-file --is-x86-multiboot2 kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@

clean:
	rm -f $(SRCS) *.bin