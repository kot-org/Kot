_HOME = ../../../../

all: configure main

main:
	cd ../Lib && ninja 
	
configure:
	cd ../Src && meson --cross-file ci/kot.cross-file  ../Lib
