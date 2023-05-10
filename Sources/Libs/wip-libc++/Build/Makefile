_HOME = ../../../../

all: configure main

main:
	cd ../Lib && ninja 
	
configure:
	cd ../Src && meson --wipe --cross-file ci/kot.cross-file  ../Lib
# cd ../Lib && meson configure -Dc_link_args=-L$(_HOME)Sysroot/lib -Dc_link_args=-I$(_HOME)Sysroot/include
