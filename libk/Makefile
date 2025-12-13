.PHONY: clean

INCLUDES = \
-I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/include

SRCS = \
arch/$(ARCH)/ports.o

libk.a: $(SRCS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(SRCS) *.a
