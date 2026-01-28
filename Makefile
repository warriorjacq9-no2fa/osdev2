MAKEFLAGS += -r

LASTARCH := $(shell cat .lastarch)

ifeq ($(wildcard .lastarch),)
ARCH ?= i386
else
ARCH ?= $(LASTARCH)
endif

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
	dd if=/dev/zero of=$@ bs=1M count=$(IMG_SIZE)

	# Create GPT + ESP (works on plain files)
	sgdisk \
	  -o \
	  -n 1:2048: \
	  -t 1:ef00 \
	  -c 1:"EFI System Partition" \
	  $@

	# Format ESP at offset (no loop device!)
	mkfs.vfat \
	  -F 32 \
	  --offset=$$(( $(ESP_OFFSET) / $(SECTOR) )) \
	  $@

	# Create directories
	mmd   -i $@@@$(ESP_OFFSET) ::/EFI
	mmd   -i $@@@$(ESP_OFFSET) ::/EFI/BOOT

	# Copy bootloader
	mcopy -i $@@@$(ESP_OFFSET) \
	  efi/main.efi ::/EFI/BOOT/$(EFI_FILE)

	# Optional kernel
	mcopy -i $@@@$(ESP_OFFSET) \
	  kernel/kernel.img ::/kernel.elf

efi/main.efi:
	$(MAKE) -C efi main.efi
else
os.img: libk/libk.a kernel/kernel.img
	cp $(lastword $^) $@
endif

build:
	@echo Building for $(ARCH)...
	@sleep 0.5
	$(MAKE) -C libk libk.a
	$(MAKE) -C kernel kernel.img
	echo $(ARCH) > .lastarch

clean:
	rm -rf *.log *.img *.efi *.dump isodir
	$(MAKE) -C kernel clean
	$(MAKE) -C libk clean

test: build os.img
	$(QEMU) -d int -D qemu.log -no-reboot -no-shutdown