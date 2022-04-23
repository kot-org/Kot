D:
cd D:\\Data\\users\\Konect\\1Documents\\programmation\\Kot\\Build

set OSNAME=kot
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../OVMFbin

qemu-system-x86_64 -bios ../ovmf/ovmf.fd -no-reboot -no-shutdown -M smm=off -serial stdio -machine q35 -cpu qemu64 -smp 1 -cdrom ../Bin/kot.iso -s -S

pause