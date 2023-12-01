#!/bin/bash

LIMINE_DEVICE=$(realpath $(df . | grep '^/' | cut -d'p' -f1))
MOUNT_DIR=$1

function install_limine {
    cd ../sources/extern/limine
    sudo make install
    sudo limine bios-install $LIMINE_DEVICE
    cd ../../../target
    
    sudo cp limine.cfg /boot/efi/EFI/BOOT
    sudo cp /usr/local/share/limine/limine-bios.sys /boot/efi/EFI/BOOT
    sudo mkdir -m 777 -p /boot/efi/EFI/limine
    sudo cp /usr/local/share/limine/BOOTX64.EFI /boot/efi/EFI/limine

    sudo rsync -av --progress $MOUNT_DIR/. /boot/efi/. --exclude limine --exclude EFI 
}

install_limine