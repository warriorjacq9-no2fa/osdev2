.PHONY: build clean

SRCS = \
kernel.o \
arch/$(ARCH)/boot.o \


build: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ -o kernel.bin
	grub-file --is-x86-multiboot2 kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $^ -o $@

clean:
	rm -f *.o arch/*/*.o *.bin