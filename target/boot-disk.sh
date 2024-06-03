#!/bin/bash

EXTERN=../sources/extern
COMMAND=$1
TARGET_NAME=$2
TARGET_ARCH=$3
BOOT_DISK=$4
MOUNT_DIR=$5
CURRENT_UID=$(id -u)
CURRENT_GID=$(id -g)

function mount_boot_disk {
    if ! [[ -f $BOOT_DISK ]] || [[ -b $BOOT_DISK ]]; then
        mkdir -p $(dirname ${BOOT_DISK})

        if ! [[ -b $BOOT_DISK ]]; then
          sudo dd if=/dev/zero bs=5M count=0 seek=128 of=${BOOT_DISK}
        fi

        # Configure GPT partition table
        sudo parted -s ${BOOT_DISK} mklabel gpt
        sudo parted -s ${BOOT_DISK} mkpart ESP fat32 2048s 100%
        sudo parted -s ${BOOT_DISK} set 1 esp on

        # Set up loopback device
        LOOPBACK=$(sudo losetup -Pf --show ${BOOT_DISK})
            
        # Format partition as FAT32 filesystem
        sudo mkfs.fat -F 32 ${LOOPBACK}p1
                
        # Mount the partition
        mkdir -p ${MOUNT_DIR}
        sudo mount -o uid=${CURRENT_UID},gid=${CURRENT_GID} ${LOOPBACK}p1 ${MOUNT_DIR}

        if [[ $TARGET_NAME == li* ]]; then
            sudo mkdir -p ${MOUNT_DIR}/limine
            sudo mkdir -p ${MOUNT_DIR}/initrd
            sudo mkdir -p ${MOUNT_DIR}/system/disk
            sudo mkdir -p ${MOUNT_DIR}/EFI/BOOT

            cp disk.cfg disktmp.cfg
            echo "DISK_SYSTEM_UUID=$(uuidgen)" > tmp.txt
            cat disktmp.cfg >> tmp.txt
            mv tmp.txt disktmp.cfg
            sudo cp disktmp.cfg ${MOUNT_DIR}/system/disk/disk.cfg
            rm tmp.txt
            rm disktmp.cfg

            make -C "${EXTERN}" limine
            cp limine.cfg ${EXTERN}/limine/limine-bios.sys ${EXTERN}/limine/limine-bios-cd.bin ${EXTERN}/limine/limine-uefi-cd.bin ${MOUNT_DIR}/limine
            cp ${EXTERN}/limine/BOOTX64.EFI ${MOUNT_DIR}/EFI/BOOT/
            cp -r initrd/. ${MOUNT_DIR}/.
            sudo ${EXTERN}/limine/limine bios-install ${BOOT_DISK}
        fi
    else
        # Set up loopback device
        LOOPBACK=$(sudo losetup -Pf --show ${BOOT_DISK})

        # Mount the partition
        mkdir -p ${MOUNT_DIR}
        sudo mount -o uid=${CURRENT_UID},gid=${CURRENT_GID} ${LOOPBACK}p1 ${MOUNT_DIR}
    fi

}

function unmount_boot_disk {
    # update sysroot directory
    cp -r -f ../sysroot/. ${MOUNT_DIR}/. 2>/dev/null

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
