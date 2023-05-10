#!/bin/bash -e
if [ $# -ne 1 ]; then
  echo "Usage: $0 <new firmware filename>"
  exit 1
fi

if [ -f "$1" ]; then
  echo "Error: file already exists: $1"
  exit 1
fi
firmware=$1

mksquashfs squashfs-root/ tmp/rootfs.bin -comp xz -b 256K -noappend
kernel=$(find -name "*kernel.ubifs")
[[ -f tmp/kernel.itb ]] && rm -f tmp/kernel.itb
cp -f $kernel tmp/kernel.itb
ubinize -m 2048 -p 128KiB -o tmp/ubi-new.img etc/ubinize-ipq50xx.cfg
cat tmp/header.bin tmp/ubi-new.img >$firmware
printf '\x50\x72' | dd of=$firmware bs=1 seek=28 count=2 conv=notrunc
bin/md5-fix $firmware
rm -rf tmp
