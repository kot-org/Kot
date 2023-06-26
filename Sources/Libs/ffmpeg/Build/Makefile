_HOME = ../../../../
PATH_TO_CONFIG_FILE = ../Src/config.h

all: configure build

configure:
ifeq ("$(wildcard $(PATH_TO_CONFIG_FILE))","")
	cd ../Src && ./configure --disable-programs --enable-static --disable-shared --disable-doc --disable-avdevice --disable-mmx --disable-network --disable-demuxer=nistsphere --disable-libv4l2 --disable-v4l2-m2m --arch=x86_64 --enable-cross-compile --cc=kot-x86_64-clang --cxx=kot-x86_64-clang --ld=kot-x86_64-clang --target-os=none --prefix=$(_HOME)Sysroot --sysroot=$(_HOME)Sysroot
endif

build:
	cd ../Src && make -j4
	cd ../Src && make install -j4