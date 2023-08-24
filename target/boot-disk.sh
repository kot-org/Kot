#!/bin/bash

EXTERN=../sources/extern
TARGET_NAME=$1
TARGET_ARCH=$2
BOOT_DISK=$3

if ! [[ -f $BOOT_DISK ]] && ! [[ -e $BOOT_DISK ]]; then
    dd if=/dev/zero bs=1M count=0 seek=64 of=${BOOT_DISK}

    # Configure GPT partition table
    parted -s ${BOOT_DISK} mklabel gpt
    parted -s ${BOOT_DISK} mkpart ESP fat32 2048s 100%
    parted -s ${BOOT_DISK} set 1 esp on

    # Set up loopback device
    LOOPBACK=$(sudo losetup -Pf --show ${BOOT_DISK})
        
    # Format partition as FAT32 filesystem
    sudo mkfs.fat -F 32 ${LOOPBACK}p1
else
    # Set up loopback device
    LOOPBACK=$(sudo losetup -Pf --show ${BOOT_DISK})
fi


if [[ $TARGET_NAME == li* ]]; then
    ${EXTERN}/limine/limine bios-install ${BOOT_DISK}
fi

# Mount the partition
mkdir -p ${TARGET_NAME}/boot_disk_kot_mount
sudo mount ${LOOPBACK}p1 ${TARGET_NAME}/boot_disk_kot_mount

# Copy necessary files
sudo mkdir -p ${TARGET_NAME}/boot_disk_kot_mount/EFI/BOOT
sudo cp -r ${TARGET_NAME}/flash/. ${TARGET_NAME}/boot_disk_kot_mount/.

# Sync data
sync

# Unmount the partition
sudo umount ${TARGET_NAME}/boot_disk_kot_mount

# Release loopback device
sudo losetup -d ${LOOPBACK}

# Remove mount folder
sudo rm -rf ${TARGET_NAME}/boot_disk_kot_mount
