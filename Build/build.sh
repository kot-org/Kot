BINDIR="Bin"

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
cd "../"

echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -p "Sysroot/Include/kot/"
mkdir -p "Sysroot/Lib/"
mkdir -p $BINDIR"/Modules/"

cp -p "Build/Starter.cfg" $BINDIR"/Modules/Starter.cfg"

cd "Sources/"

# Libraries
echo -e "\e[32mCompiling kot libraries...\e[0m"

sudo make -C "Libs/libc/Build"
sudo make -C "Libs/abi/Build"

# Drivers
echo -e "\e[32mCompiling kot drivers...\e[0m"

sudo make -C "Modules/Drivers/ps2/Build"
sudo make -C "Modules/Drivers/vga/Build"

# Services
echo -e "\e[32mCompiling kot services...\e[0m"

sudo make -C "Modules/Services/system/Build"
sudo make -C "Modules/Services/orb/Build"
sudo make -C "Modules/Services/uisd/Build"

# kernel
cd "../Sources/Kernel/"
echo -e "\e[32mBuilding kernel...\e[0m"

make all