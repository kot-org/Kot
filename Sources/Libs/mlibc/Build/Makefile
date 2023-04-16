_HOME = ../../../../../

all: configure main

main:
	cd ../Src/build && DESTDIR=$(_HOME)Sysroot ninja all install
	
configure:
	cd ../Src && meson setup build --cross-file ci/kot.cross-file -Ddefault_library=static --prefix=/

reconfigure:
	cd ../Src && meson setup build --cross-file ci/kot.cross-file -Ddefault_library=static --prefix=/ --wipe
