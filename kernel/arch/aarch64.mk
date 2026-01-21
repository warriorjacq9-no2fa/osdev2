.PHONY: clean

INCLUDES := -I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/include

LIBS := -L$(CURDIR)/../libk -lk

HEADERS = \
arch/$(ARCH)/boot.h \
arch/$(ARCH)/include/kernel/arch.h

OBJS = \
kernel.o \
arch/$(ARCH)/arch.o \
arch/$(ARCH)/boot.o

kernel.img: $(OBJS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ $(LIBS) -o $@

clean:
	rm -f $(OBJS) *.bin *.dump *.img

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@
