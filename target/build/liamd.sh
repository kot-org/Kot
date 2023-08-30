#!/bin/bash

PACKAGE_NAME=$1

if [ "$PACKAGE_NAME" = "all" ]; then
    PACKAGE_NAME="--all"
fi

cd ../ # target

source boot-disk.sh mount liamd amd64 liamd/boot-kot.img boot_disk_kot_mount

cd ../ # root


if [ "$PACKAGE_NAME" = "" ]; then
    xbstrap install $PACKAGE_NAME
else
    xbstrap install $PACKAGE_NAME --rebuild
fi

cd target/ # target
source boot-disk.sh unmount liamd amd64 liamd/boot-kot.img boot_disk_kot_mount
