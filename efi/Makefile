CC=aarch64-linux-gnu-gcc
LD=aarch64-linux-gnu-ld
OBJCOPY=aarch64-linux-gnu-objcopy
ARCH=aarch64

CFLAGS= -Iinclude -I$(GNU_EFI)/inc -I$(GNU_EFI)/inc/$(ARCH) -fpic -ffreestanding -fno-stack-protector \
		-fno-stack-check -fshort-wchar
LDFLAGS= -shared -Bsymbolic -L$(GNU_EFI)/$(ARCH)/lib -L$(GNU_EFI)/$(ARCH)/gnuefi \
		-T$(GNU_EFI)/gnuefi/elf_$(ARCH)_efi.lds $(GNU_EFI)/$(ARCH)/gnuefi/crt0-efi-$(ARCH).o 

GNU_EFI=../../gnu-efi

main.efi: main.c acpi.c
	$(CC) $(CFLAGS)\
		-c main.c -o main.o
	$(CC) $(CFLAGS) \
		-c acpi.c -o acpi.o
	$(LD) $(LDFLAGS)\
		main.o acpi.o -o main.so -lgnuefi -lefi
	$(OBJCOPY) -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym \
		-j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-$(ARCH) \
		--subsystem=10 main.so main.efi

clean:
	rm -f *.efi *.so *.o