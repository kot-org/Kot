#!/bin/bash

EXTERN=../sources/extern
COMMAND=$1
TARGET_NAME=$2
TARGET_ARCH=$3
BOOT_DISK=$4
MOUNT_DIR=$5

function mount_boot_disk {
    if ! [[ -f $BOOT_DISK ]] && ! [[ -e $BOOT_DISK ]]; then
        mkdir -p $(dirname ${BOOT_DISK})

        dd if=/dev/zero bs=1M count=0 seek=64 of=${BOOT_DISK}

        # Configure GPT partition table
        parted -s ${BOOT_DISK} mklabel gpt
        parted -s ${BOOT_DISK} mkpart ESP fat32 2048s 100%
        parted -s ${BOOT_DISK} set 1 esp on

        # Set up loopback device
        LOOPBACK=$(sudo losetup -Pf --show ${BOOT_DISK})
            
        # Format partition as FAT32 filesystem
        sudo mkfs.fat -F 32 ${LOOPBACK}p1
                
        # Mount the partition
        mkdir -p ${MOUNT_DIR}
        sudo mount -o uid=1000,gid=1000 ${LOOPBACK}p1 ${MOUNT_DIR}

        if [[ $TARGET_NAME == li* ]]; then
            mkdir -p ${MOUNT_DIR}/limine
            mkdir -p ${MOUNT_DIR}/initrd
            mkdir -p ${MOUNT_DIR}/EFI/BOOT
            make -C "${EXTERN}" limine
            cp limine.cfg ${EXTERN}/limine/limine-bios.sys ${EXTERN}/limine/limine-bios-cd.bin ${EXTERN}/limine/limine-uefi-cd.bin ${MOUNT_DIR}/limine
            cp ${EXTERN}/limine/BOOTX64.EFI ${MOUNT_DIR}/EFI/BOOT/
            cp -r initrd/. ${MOUNT_DIR}/.
            ${EXTERN}/limine/limine bios-install ${BOOT_DISK}
        fi
    else
        # Set up loopback device
        LOOPBACK=$(sudo losetup -Pf --show ${BOOT_DISK})

        # Mount the partition
        mkdir -p ${MOUNT_DIR}
        sudo mount -o uid=1000,gid=1000 ${LOOPBACK}p1 ${MOUNT_DIR}
    fi

}

function unmount_boot_disk {
    # Unmount the partition
    sudo umount ${MOUNT_DIR}

    # Release loopback device
    sudo losetup -d ${LOOPBACK}

    # Remove mount folder
    sudo rm -rf ${MOUNT_DIR}
}

case $COMMAND in

  "mount")
    mount_boot_disk
    ;;

  "unmount")
    unmount_boot_disk
    ;;

  *)
    echo -n "unknown command, usage : <command> <target_name> <target_arch> <boot_disk> <mount_dir>"
    ;;
esac
