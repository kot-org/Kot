echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -p Sysroot/include/kot/
mkdir -p Sysroot/lib/
mkdir -p Bin/Modules/

echo -e "\e[32mCompiling kot libraries...\e[0m"

cd Sources/Libs/

# libc

cd libc/build/
make
cd ../../

# abi

cd abi/build/
make 


# ramfs

cd ../../../../
sudo bash ./Tools/BuildRamFS.sh

# kernel

cd ./Sources/Kernel/
echo -e "\e[32mBuilding kernel...\e[0m"

make all