all: gitclone setup build

gitclone:
	git clone https://gcc.gnu.org/git/gcc.git

setup:
	./gcc/contrib/download_prerequisites
	cd gcc && git checkout releases/gcc-12.2.0 && git apply ../gcc.diff

build:
	mkdir -m 777 -p "Bin"
	cd Bin && ../gcc/configure --target=x86_64-kot --prefix="$(shell pwd)/Bin" --disable-nls --enable-languages=c,c++ --without-headers && make all-gcc && make all-target-libgcc && make install-gcc && make install-target-libgcc