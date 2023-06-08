BINDIR="./Bin"

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
cd "../"

echo -e "\e[32mCreating disk...\e[0m"
mkdir -m 777 -p "Disk"
git clone "https://github.com/kot-org/Disk"

echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -m 777 -p "Sysroot/include/kot/"
mkdir -m 777 -p "Sysroot/include/kot++/"
mkdir -m 777 -p "Sysroot/lib/"
mkdir -m 777 -p $BINDIR"/Modules/"
mkdir -m 777 -p $BINDIR"/Firmwares/"

cp -p -R ./Build/Bin/Modules/** $BINDIR"/Modules/"
cp -p -R ./Build/Bin/Firmwares/** $BINDIR"/Firmwares/"

# Compiler
echo -e "\e[32mCompiling kot compilers...\e[0m"
bash "Tools/BuildCompilers.sh"

cd "Sources/"

# Libraries
echo -e "\e[32mCompiling kot libraries...\e[0m"

make -C "Libs/abi/Build"
make -C "Libs/mlibc/Build"
make -C "Libs/libc++/Build"
make -C "Libs/wip-libc++/Build"
make -C "Libs/freetype/Build"
make -C "Libs/kot-graphics/Build"
make -C "Libs/kot-ui/Build"
make -C "Libs/kot-ui++/Build"
make -C "Libs/kot-audio++/Build"

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

make -C "Modules/Drivers/audio/hda/Build"

make -C "Modules/Drivers/time/hpet/Build"
make -C "Modules/Drivers/time/rtc/Build"

make -C "Modules/Drivers/graphics/orb/Build"

make -C "Modules/Drivers/fs/ext2/Build"

# Services
echo -e "\e[32mCompiling kot services...\e[0m"

make -C "Modules/Services/hid/Build"
make -C "Modules/Services/audio/Build"
make -C "Modules/Services/time/Build"
make -C "Modules/Services/storage/Build"
make -C "Modules/Services/shell/Build"

# Apps
echo -e "\e[32mCompiling kot apps...\e[0m"

make -C "Apps/lua/Build"
make -C "Apps/tcc/Build"
make -C "Apps/test/Build"
make -C "Apps/calculator/Build"
make -C "Apps/explorer/Build"
make -C "Apps/shell/Build"

# kernel
echo -e "\e[32mBuilding kernel...\e[0m"
make -C  "../Sources/Kernel/"

# Ukl
echo -e "\e[32mBuilding UKL...\e[0m"
make -C "../Sources/Ukl"