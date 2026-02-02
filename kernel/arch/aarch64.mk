.PHONY: clean

INCLUDES := -I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/include

LIBS := -L$(CURDIR)/../libk -lk

HEADERS = \
shell.h \
arch/$(ARCH)/boot.h \
arch/$(ARCH)/uart.h \
arch/$(ARCH)/include/kernel/arch.h

OBJS = \
arch/$(ARCH)/arch.o \
kernel.o \
shell.o \
arch/$(ARCH)/uart.o \
arch/$(ARCH)/boot.o

kernel.img: $(OBJS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ $(LIBS) -o $@

clean:
	rm -f $(OBJS) *.bin *.dump *.img

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@
