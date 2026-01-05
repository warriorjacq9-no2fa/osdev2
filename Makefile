ARCH ?= i386

include arch/$(ARCH).conf

# Export variables for subdirectories
export CFLAGS CC AR AFLAGS AFLAGS_BIN AS LD LDFLAGS LDFLAGS_BIN OBJCOPY QEMU ARCH EFI EFI_ARCH EFI_NAME

.PHONY: all build clean test
all: build os.img

ifeq ($(EFI),yes)

IMG        := os.img
IMG_SIZE   := 64
ESP_OFFSET := 1048576   # 1 MiB
SECTOR     := 512
EFI_BOOT   := EFI/BOOT
EFI_FILE   := $(shell echo "$(EFI_NAME)" | tr '[:lower:]' '[:upper:]')

os.img: kernel/kernel.img efi/main.efi
	dd if=/dev/zero of=$(IMG) bs=1M count=$(IMG_SIZE)

	# Create GPT + ESP (works on plain files)
	sgdisk \
	  -o \
	  -n 1:2048: \
	  -t 1:ef00 \
	  -c 1:"EFI System Partition" \
	  $(IMG)

	# Format ESP at offset (no loop device!)
	mkfs.vfat \
	  -F 32 \
	  --offset=$$(( $(ESP_OFFSET) / $(SECTOR) )) \
	  $(IMG)

	# Create directories
	mmd   -i $(IMG)@@$(ESP_OFFSET) ::/EFI
	mmd   -i $(IMG)@@$(ESP_OFFSET) ::/EFI/BOOT

	# Copy bootloader
	mcopy -i $(IMG)@@$(ESP_OFFSET) \
	  efi/main.efi ::/EFI/BOOT/$(EFI_FILE)

	# Copy GRUB config
	mcopy -i $(IMG)@@$(ESP_OFFSET) \
	  arch/$(ARCH).grub ::/EFI/BOOT/grub.cfg

	# Optional kernel
	mcopy -i $(IMG)@@$(ESP_OFFSET) \
	  kernel/kernel.img ::/kernel.elf

efi/main.efi:
	$(MAKE) -C efi main.efi
else
os.img: libk/libk.a kernel/kernel.img
	cp $(lastword $^) $@
endif

build:
	$(MAKE) -C libk libk.a
	$(MAKE) -C kernel kernel.img

clean:
	rm -rf *.log *.img *.efi *.dump isodir
	$(MAKE) -C kernel clean
	$(MAKE) -C libk clean

test: build os.img
	$(QEMU) -d int -D qemu.log -no-reboot -no-shutdown