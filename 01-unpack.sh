#!/bin/bash -e
if [ $# -ne 1 ]; then
  echo "Usage: $0 <firmware>"
  exit 1
fi

if [ ! -f "$1" ]; then
  echo "Error: file not found: $1"
  exit 1
fi

firmware=$1
pos=$(grep -a -b -m 1 "UBI#" "$firmware" | cut -d ":" -f 1)
mkdir -p tmp

dd if=$firmware of=tmp/header.bin bs=$pos count=1
dd if=$firmware of=tmp/ubi.img bs=$pos skip=1

cd tmp
ubireader_extract_images ubi.img

cd ..
find -name "*rootfs.ubifs" -exec unsquashfs {} \;

echo "Successfully unpacked rootfs to squashfs-root. Modify anything you want and repack the firmware"
