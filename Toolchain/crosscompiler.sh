

#versions
BinutilsVersion="2.35"
GccVersion="10.2.0"

#some variables

Prefix="$(realpath $(dirname "$0"))/Local"
export PATH="$PATH:$Prefix/bin"
Target="x86_64-elf"

#creat folder
echo $Prefix
mkdir $Prefix

#depencies

sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

#install binutils

cd /tmp
wget "https://ftp.gnu.org/gnu/binutils/binutils-$BinutilsVersion.tar.xz"
tar -xf "binutils-$BinutilsVersion.tar.xz"
cd "binutils-$BinutilsVersion"
mkdir build 
cd build
../configure --prefix="$Prefix" --target="$Target" --with-sysroot --disable-nls --disable-werror
make all -j $(nproc)
make install -j $(nproc)

#install gcc
wget "http://ftp.gnu.org/gnu/gcc/gcc-$GccVersion/gcc-$GccVersion.tar.xz"
tar -xf gcc-$GccVersion.tar.xz

cd "gcc-$GccVersion"
mkdir build 
cd build
../configure --prefix="$Prefix" --target="$Target" --with-sysroot --disable-nls --enable-languages=c,c++ --with-newlib

make -j all-gcc 
make -j all-target-libgcc
make -j install-gcc 
make -j install-target-libgcc