all:
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 14 all

	sudo apt update
	sudo apt install nasm xorriso 
	bash ./Build/build.sh
