_HOME = ../../../../

all: configure main

main:
	cd ../Lib && DESTDIR=$(_HOME)Sysroot ninja all install

build:
	cd ../Lib && DESTDIR=$(_HOME)Sysroot ninja all
	
configure:
	cd ../Src && meson setup ../Lib --cross-file ci/kot.cross-file -Ddefault_library=static

reconfigure:
	cd ../Src && meson setup ../Lib --cross-file ci/kot.cross-file -Ddefault_library=static --wipe
