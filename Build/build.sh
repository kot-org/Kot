SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
cd "../"

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 14 all
sudo apt update
sudo apt install nasm xorriso 

echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -p "Sysroot/Include/kot/"
mkdir -p "Sysroot/Lib/"
mkdir -p "Bin/Modules/"

cp -p "Build/Starter.cfg" "Bin/Modules/Starter.cfg"

cd "Sources/"

# Libraries
echo -e "\e[32mCompiling kot libraries...\e[0m"

sudo make -C "Libs/libc/build"
sudo make -C "Libs/abi/build"

# Drivers
echo -e "\e[32mCompiling kot drivers...\e[0m"

sudo make -C "Modules/Drivers/ps2/build"
sudo make -C "Modules/Drivers/vga/build"

# Services
echo -e "\e[32mCompiling kot services...\e[0m"

sudo make -C "Modules/Services/system/build"
sudo make -C "Modules/Services/wm/build"
sudo make -C "Modules/Services/uisd/build"

# ramfs
cd "../"
sudo bash "./Tools/BuildRamFS.sh"

# kernel
cd "./Sources/Kernel/"
echo -e "\e[32mBuilding kernel...\e[0m"

make all
