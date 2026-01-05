.PHONY: clean

SRCS = \
arch/$(ARCH)/boot.o

kernel.img: $(SRCS)
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/linker.ld $^ -o $@

clean:
	rm -f $(SRCS) arch/$(ARCH)/bootstrap.o *.bin *.dump *.img