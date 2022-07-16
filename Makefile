build:
	sudo bash ./Build/build.sh

run:
	sudo bash ./Build/run.sh

deps-llvm:
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 14 all
	rm -f llvm.sh

deps-debian: deps-llvm
	sudo apt update
	sudo apt install nasm xorriso build-essential qemu-system-x86 -y

clean:
	sudo rm -rf ./Bin ./Sysroot ./Sources/Kernel/Lib ./Sources/Libs/*/Lib ./Sources/Modules/Drivers/*/Lib ./Sources/Modules/Services/*/Lib

github-action: deps-llvm deps-debian build

.PHONY: build run llvm deps-debian github-action
