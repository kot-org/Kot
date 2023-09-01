#!/bin/bash

ACTION=$1
PACKAGE_NAME=$2

if [ "$PACKAGE_NAME" = "all" ]; then
    PACKAGE_NAME="--all"
fi

cd ../ # target

source boot-disk.sh mount liamd amd64 liamd/boot-kot.img boot_disk_kot_mount

cd ../ # root

if [ "$PACKAGE_NAME" = "" ]; then
    xbstrap install --$ACTION
else
    xbstrap install $PACKAGE_NAME --$ACTION
fi

cd target/ # target
source boot-disk.sh unmount liamd amd64 liamd/boot-kot.img boot_disk_kot_mount
