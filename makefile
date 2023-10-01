TARGET ?= liamd
BOOT_DEVICE ?= virtual

update-pkgs:
	sudo apt update
	sudo apt upgrade

install-ninja:
	sudo apt install wget
	sudo wget -qO /usr/local/bin/ninja.gz https://github.com/ninja-build/ninja/releases/latest/download/ninja-linux.zip
	sudo gunzip /usr/local/bin/ninja.gz
	sudo chmod a+x /usr/local/bin/ninja

deps-debian: update-pkgs install-ninja
	sudo apt install nasm build-essential parted qemu-utils qemu-system-x86 meson python3 python3-pip python3-setuptools python3-wheel ninja-build cmake sed m4 texinfo libgmp-dev bison flex curl -y
	sudo pip3 install meson
	sudo pip3 install xbstrap

init:
	@ xbstrap init .
	@ mkdir -m 777 -p sysroot

build:
	@ cd target && BOOT_DEVICE=$(BOOT_DEVICE) LC_ALL=C make $(TARGET)/build

configure:
	@ cd target && BOOT_DEVICE=$(BOOT_DEVICE) LC_ALL=C make $(TARGET)/configure

run:
	@ cd target && BOOT_DEVICE=$(BOOT_DEVICE) LC_ALL=C make $(TARGET)/run

all: init build run

github-action: deps-debian init build
	qemu-img convert -f raw -O vdi target/liamd/boot-kot.img target/liamd/boot-kot.vdi
	qemu-img convert -f raw -O vmdk target/liamd/boot-kot.img target/liamd/boot-kot.vmdk
