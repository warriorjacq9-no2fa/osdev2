.PHONY: build clean

SRCS = \
kernel.o \
arch/$(ARCH)/boot.o \
arch/$(ARCH)/arch.o \
arch/$(ARCH)/ports.o \
arch/$(ARCH)/vga.o


kernel.bin: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ -o kernel.bin
	objdump -d kernel.bin > kernel.dump
	grub-file --is-x86-multiboot2 kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@

clean:
	rm -f *.o arch/*/*.o *.bin