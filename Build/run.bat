cd D:\Data\users\Konect\1Documents\programmation\Kot\Build

set OSNAME=kot
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../OVMFbin

qemu-system-x86_64 -machine q35 -cpu qemu64 -smp 1 -drive file=../bin/debugDisk.vhd -drive file=../Data/debugDisk.vhd -m 4G -drive if=pflash,format=raw,unit=0,file=../OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=../OVMFbin/OVMF_VARS-pure-efi.fd 

pause