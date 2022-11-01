BINDIR="./Bin"

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
cd "../"

echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -m 777 -p "Sysroot/Include/kot/"
mkdir -m 777 -p "Sysroot/Include/kot++/"
mkdir -m 777 -p "Sysroot/Lib/"
mkdir -m 777 -p $BINDIR"/Modules/"

cp -p -R ./Build/Bin/** $BINDIR"/Modules/"

cd "Sources/"

# Libraries
echo -e "\e[32mCompiling kot libraries...\e[0m"

make -C "Libs/abi/Build"
make -C "Libs/libc/Build"
make -C "Libs/libc++/Build"
make -C "Libs/kot-graphics/Build"
make -C "Libs/kot-graphics++/Build"
make -C "Libs/kot-ui/Build"
make -C "Libs/kot-ui++/Build"

# System
echo -e "\e[32mCompiling kot system...\e[0m"

make -C "System/Build"

# Drivers
echo -e "\e[32mCompiling kot drivers...\e[0m"

make -C "Modules/Drivers/bus/pci/Build"
make -C "Modules/Drivers/bus/ps2/Build"
make -C "Modules/Drivers/bus/usb/ehci/Build"

make -C "Modules/Drivers/net/e1000/Build"

make -C "Modules/Drivers/storage/ahci/Build"

make -C "Modules/Drivers/sound/pcspk/Build"

make -C "Modules/Drivers/timer/rtc/Build"

make -C "Modules/Drivers/graphics/orb/Build"

make -C "Modules/Drivers/fs/ext/Build"

# Services
echo -e "\e[32mCompiling kot services...\e[0m"

make -C "Modules/Services/shell/Build"
make -C "Modules/Services/hid/Build"
make -C "Modules/Services/storage/Build"

# Apps
echo -e "\e[32mCompiling kot apps...\e[0m"

make -C "Apps/test/Build"

# kernel
echo -e "\e[32mBuilding kernel...\e[0m"
make -C  "../Sources/Kernel/"

# Ukl
echo -e "\e[32mBuilding UKL...\e[0m"
make -C "../Sources/Ukl"