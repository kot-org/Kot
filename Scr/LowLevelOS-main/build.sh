cd gnu-efi
make -s -j8 bootloader
cd ..
rm -rf kernel/lib
mkdir kernel/lib
cd kernel
make -j8 -s kernel
rm -rf bin/LLOS.iso
make -s -j8 buildimg