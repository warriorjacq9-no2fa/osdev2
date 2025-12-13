.PHONY: clean

INCLUDES = \
-I$(CURDIR)/arch/$(ARCH)/include \
-I$(CURDIR)/include \
-I$(CURDIR)/../kernel/arch/$(ARCH)/include

SRCS = \
arch/$(ARCH)/ports.o \
stdio/printf.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \
string/strcat.o \
string/strcpy.o \
string/strchr.o \
string/strcmp.o \
string/strpbrk.o \
string/strspn.o \
string/memchr.o \
string/strtok.o \
string/strtoul.o \

libk.a: $(SRCS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(SRCS) *.a
