#!/bin/bash

PACKAGE_NAME=$1

if [ "$PACKAGE_NAME" = "all" ] || [ "$PACKAGE_NAME" = "" ]; then
    PACKAGE_NAME="--all"
fi

cd ../ # target

source boot-disk.sh mount liamd amd64 liamd/boot-kot.img boot_disk_kot_mount

cd ../ # root

xbstrap install $PACKAGE_NAME --rebuild

cd target/ # target
source boot-disk.sh unmount liamd amd64 liamd/boot-kot.img boot_disk_kot_mount
