

#versions
BinutilsVersion="2.35"
GccVersion="10.2.0"

#some variables
Prefix="$(realpath $(dirname "$0"))/Local"
Sysroot="$(dirname $(realpath $(dirname "$0")))/Sysroot"
export PATH="$PATH:$Prefix/bin"
Target="x86_64-kot"

#creat folder
mkdir $Prefix

#depencies

sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

sudo mkdir -m 777 "$Sysroot"
sudo mkdir -m 777 "$Sysroot/include"
sudo mkdir -m 777 "$Sysroot/include/kot"
sudo mkdir -m 777 "$Sysroot/lib"

cd "sources"

#install binutils
cd "binutils-$BinutilsVersion"
mkdir build 
cd build
sudo ../configure --prefix="$Prefix" --target="$Target" --with-sysroot="$Sysroot" --disable-nls --disable-werror
make all -j $(nproc)
make install -j $(nproc)

cd ../../
         
#install gcc
cd "gcc-$GccVersion"
mkdir build 
cd build
sudo ../configure --prefix="$Prefix" --target="$Target" --with-sysroot="$Sysroot" --disable-nls --enable-languages=c,c++ --with-newlib

make -j all-gcc 
make -j all-target-libgcc 
make -j install-gcc 
make -j install-target-libgcc