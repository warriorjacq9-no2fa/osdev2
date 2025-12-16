.PHONY: clean

INCLUDES := -I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/include 

LIBS := -L$(CURDIR)/../libk -lk

HEADERS = \
arch/$(ARCH)/vga.h \
arch/$(ARCH)/include/kernel/kbd.h \
arch/$(ARCH)/include/kernel/arch.h

SRCS = \
kernel.o \
shell.o \
arch/$(ARCH)/boot.o \
arch/$(ARCH)/arch.o \
arch/$(ARCH)/vga.o \
arch/$(ARCH)/kbd.o

kernel.img: arch/$(ARCH)/bootstrap.o kernel.bin
	cat $^ > kernel.img

kernel.bin: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ $(LIBS) -o kernel.bin
	objdump -b binary -m i386 --adjust-vma=0x10000 -D $@ > kernel.dump

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@

arch/$(ARCH)/bootstrap.o: arch/$(ARCH)/bootstrap.asm
	$(AS) -fbin $< -o $@
	objdump -b binary -m i8086 --adjust-vma=0x7c00 -D $@ > bootstrap.dump

clean:
	rm -f $(SRCS) arch/$(ARCH)/bootstrap.o *.bin *.dump