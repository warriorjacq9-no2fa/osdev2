.PHONY: clean

SRCS = \
arch/$(ARCH)/boot.o

kernel.img: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ -o $@.o
	$(OBJCOPY) -O binary $@.o $@

clean:
	rm -f $(SRCS) arch/$(ARCH)/bootstrap.o *.bin *.dump *.img