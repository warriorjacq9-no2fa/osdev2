.PHONY: clean

INCLUDES := -I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/arch/$(ARCH)/include \
-I$(CURDIR)/../libk/include 

LIBS := -L$(CURDIR)/../libk -lk

HEADERS = \
arch/$(ARCH)/vga.h \
arch/$(ARCH)/include/kernel/arch.h

SRCS = \
kernel.o \
shell.o \
arch/$(ARCH)/boot.o \
arch/$(ARCH)/arch.o \
arch/$(ARCH)/vga.o

kernel.img: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ $(LIBS) -o $@

clean:
	rm -f $(SRCS) arch/$(ARCH)/bootstrap.o *.bin *.dump *.img