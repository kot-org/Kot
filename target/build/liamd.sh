#!/bin/bash

ACTION=$1
BOOT_DEVICE=$2
PACKAGE_NAME=$3
INSTALL=$4

if [ "$PACKAGE_NAME" = "all" ]; then
    PACKAGE_NAME="--all"
fi

if [ "$BOOT_DEVICE" = "virtual" ]; then
    BOOT_DEVICE="liamd/boot-kot.img"
fi

cd ../ # target

source boot-disk.sh mount liamd amd64 $BOOT_DEVICE boot_disk_kot_mount

cd ../ # root

if [ "$PACKAGE_NAME" = "" ]; then
    xbstrap install --$ACTION
else
    xbstrap install $PACKAGE_NAME --$ACTION
fi

ERROR_XBSTRAP=$?

cd target/ # target

if [ "$INSTALL" = "true" ]; then
    BOOT_DEVICE="liamd/boot-kot.img"
    source host-installer.sh boot_disk_kot_mount
fi

source boot-disk.sh unmount liamd amd64 $BOOT_DEVICE boot_disk_kot_mount

exit $ERROR_XBSTRAP
