TARGET ?= liamd

update-pkgs:
	sudo apt update
	sudo apt upgrade

install-ninja:
	sudo apt install wget
	sudo wget -qO /usr/local/bin/ninja.gz https://github.com/ninja-build/ninja/releases/latest/download/ninja-linux.zip
	sudo gunzip /usr/local/bin/ninja.gz
	sudo chmod a+x /usr/local/bin/ninja

deps-debian: update-pkgs install-ninja
	sudo apt install nasm build-essential parted qemu-system-x86 meson python3 python3-pip python3-setuptools python3-wheel ninja-build cmake -y
	sudo pip3 install meson
	sudo pip3 install xbstrap

init:
	@ xbstrap init .
	@ xbstrap install-tool --all

build:
	@ cd target && make $(TARGET)/build

run:
	@ cd target && make $(TARGET)/run

all: init build run

github-action: deps-debian init build