all:
	mkdir -m 777 -p "../Bin"
	cd ../Src && ./configure && make tcc
	cp ../Src/tcc ../Bin/Tcc.elf
clean:
	cd ../Src && make clean