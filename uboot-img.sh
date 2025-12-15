#!/usr/bin/env bash
set -euo pipefail

IMG=os.img
SIZE_MB=64
KERNEL=kernel.elf
UBOOT=u-boot.bin
MNT=isodir/os-usb-mnt

echo "[*] Creating disk image"
dd if=/dev/zero of=$IMG bs=1M count=$SIZE_MB status=progress

echo "[*] Partitioning image (MBR + FAT32)"
parted -s $IMG \
  mklabel msdos \
  mkpart primary fat32 1MiB 100% \
  set 1 boot on

echo "[*] Attaching loop device"
LOOP=$(losetup --find --show --partscan $IMG)

echo "[*] Formatting FAT filesystem"
mkfs.vfat ${LOOP}p1

mkdir -p $MNT
mount ${LOOP}p1 $MNT

echo "[*] Copying kernel"
cp $KERNEL $MNT/

echo "[*] Creating U-Boot boot script"
cat > $MNT/boot.cmd <<EOF
fatload ide 0:1 \${kernel_addr_r} kernel.elf
bootelf \${kernel_addr_r}
EOF

mkimage -A x86 -T script -C none \
  -d $MNT/boot.cmd $MNT/boot.scr

rm $MNT/boot.cmd

sync
umount $MNT

echo "[*] Installing U-Boot MBR"
dd if=$UBOOT of=$LOOP bs=440 count=1 conv=notrunc

losetup -d $LOOP

echo "[✓] Bootable USB image created: $IMG"
