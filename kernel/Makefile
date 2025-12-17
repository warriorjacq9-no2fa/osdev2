include arch/$(ARCH).mk

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(AS) $(AFLAGS) $< -o $@
