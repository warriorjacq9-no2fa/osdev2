#!/usr/bin/env bash
set -euo pipefail

ISO=os.iso
KERNEL=kernel/kernel.bin
UBOOT=u-boot-x86-16bit.bin
ISO_ROOT=isodir

echo "[*] Preparing ISO root"
rm -rf $ISO_ROOT
mkdir -p $ISO_ROOT

cp $KERNEL $ISO_ROOT/

echo "[*] Creating U-Boot boot script"
cat > $ISO_ROOT/boot.cmd <<EOF
fatload cdrom 0 \${kernel_addr_r} kernel.elf
bootelf \${kernel_addr_r}
EOF

mkimage -A x86 -T script -C none \
  -d $ISO_ROOT/boot.cmd $ISO_ROOT/boot.scr

rm $ISO_ROOT/boot.cmd

echo "[*] Building ISO image"
xorriso -as mkisofs \
  -b $UBOOT \
  -no-emul-boot \
  -boot-load-size 4 \
  -boot-info-table \
  -o $ISO \
  $ISO_ROOT

echo "[✓] Bootable ISO created: $ISO"
