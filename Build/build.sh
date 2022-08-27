BINDIR="./Bin"

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
cd "../"

echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -m 777 -p "Sysroot/Include/kot/"
mkdir -m 777 -p "Sysroot/Include/kot++/"
mkdir -m 777 -p "Sysroot/Lib/"
mkdir -m 777 -p $BINDIR"/Modules/"

cp -p ./Build/Bin/** $BINDIR"/Modules/"

cd "Sources/"

# Libraries
echo -e "\e[32mCompiling kot libraries...\e[0m"

sudo make -C "Libs/abi/Build"
sudo make -C "Libs/libc/Build"
sudo make -C "Libs/libc++/Build"
sudo make -C "Libs/kot-ui/Build"

# System
echo -e "\e[32mCompiling kot system...\e[0m"

sudo make -C "System/Build"

# Drivers
echo -e "\e[32mCompiling kot drivers...\e[0m"

sudo make -C "Modules/Drivers/bus/pci/Build"
sudo make -C "Modules/Drivers/bus/usb/uhci/Build"

sudo make -C "Modules/Drivers/ps2/Build"

#sudo make -C "Modules/Drivers/vga/Build"

sudo make -C "Modules/Drivers/sound/pcspk/Build"

sudo make -C "Modules/Drivers/timer/rtc/Build"

# Services
echo -e "\e[32mCompiling kot services...\e[0m"

sudo make -C "Modules/Services/shell/Build"

sudo make -C "Modules/Services/orb/Build"

sudo make -C "Modules/Services/jvm/Build"

# kernel
cd "../Sources/Kernel/"
echo -e "\e[32mBuilding kernel...\e[0m"

sudo make all