#!/bin/env bash

# Versions
BinUtilsVersion="2.35"
GCCVersion="11.2.0"

# Some variables
Prefix="$(realpath $(dirname "$0"))/Local"
Sysroot="$(dirname $(realpath $(dirname "$0")))/Sysroot"
Target="x86_64-kot"
export PATH="$PATH:$Prefix/bin"

# Create folder
mkdir $Prefix

# Depencies
echo -e "\e[32mInstalling depedencies...\e[0m"
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

sudo mkdir -m 777 "$Sysroot/include/kot"
sudo mkdir -m 777 "$Sysroot/lib"

cd "sources"

rm -r *

# Downloading Binutils & GCC
echo -e "\e[32mDownloading Binutils...\e[0m"
wget https://ftp.gnu.org/gnu/binutils/binutils-$BinUtilsVersion.tar.xz
echo -e "\e[32mDownloading GCC...\e[0m"
wget ftp://ftp.gnu.org/gnu/gcc/gcc-$GCCVersion/gcc-$GCCVersion.tar.xz

# Unpacking
echo -e "\e[32mUnpacking...\e[0m"
tar -xvf binutils-$BinUtilsVersion.tar.xz
tar -xvf gcc-$GCCVersion.tar.xz
rm -r binutils-$BinUtilsVersion.tar.xz gcc-$GCCVersion.tar.xz

# Install binutils
echo -e "\e[32mBuilding binutils\e[0m"
mkdir bin-binutils-$BinUtilsVersion
cd bin-binutils-$BinUtilsVersion
sudo ../binutils-$BinUtilsVersion/configure --prefix="$Prefix" --target="$Target" --with-sysroot="$Sysroot" --disable-nls --disable-werror --enable-shared
make all -j $(nproc)
make install -j $(nproc)

# Back to source folder
cd ../
         
# Install GCC
echo -e "\e[32mBuilding GCC\e[0m"
mkdir bin-gcc-$GCCVersion
cd bin-gcc-$GCCVersion
sudo ../gcc-$GCCVersion/configure --prefix="$Prefix" --target="$Target" --with-sysroot="$Sysroot" --disable-nls --enable-languages=c,c++ --with-newlib --enable-shared

make -j all-gcc 
make -j all-target-libgcc 
make -j install-gcc 
make -j install-target-libgcc

rm -r binutils-$BinUtilsVersion gcc-$GCCVersion

echo "Done."