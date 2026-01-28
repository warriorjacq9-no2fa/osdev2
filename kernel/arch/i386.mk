.PHONY: clean

INCLUDES := -I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/include 

LIBS := -L$(CURDIR)/../libk -lk

HEADERS = \
arch/$(ARCH)/vga.h \
arch/$(ARCH)/include/kernel/kbd.h \
arch/$(ARCH)/include/kernel/arch.h

OBJS = \
kernel.o \
shell.o \
arch/$(ARCH)/boot.o \
arch/$(ARCH)/arch.o \
arch/$(ARCH)/vga.o \
arch/$(ARCH)/kbd.o

kernel.img: arch/$(ARCH)/bootstrap.o kernel.bin
	cat $^ > kernel.img

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ $(LIBS) -o $@

arch/$(ARCH)/bootstrap.o: arch/$(ARCH)/bootstrap.asm
	$(AS) $(AF_BIN) $< -o $@

clean:
	rm -f $(OBJS) arch/$(ARCH)/bootstrap.o *.bin *.dump *.img

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@
