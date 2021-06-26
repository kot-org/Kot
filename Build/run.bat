  
set OSNAME=kot
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../OVMFbin

qemu-system-x86_64 -cpu qemu64 -smp 3 -serial stdio -usb -machine q35 -drive file=../bin/kot.vhd -drive file=../bin/sataDisk.vhd -m 1G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=../OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=../OVMFbin/OVMF_VARS-pure-efi.fd -net none

pause