D:
cd D:\\Data\\users\\Konect\\1Documents\\programmation\\Kot\\Build

set OSNAME=kot
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../OVMFbin

qemu-system-x86_64 -bios ../Meta/ovmf/ovmf.fd -no-reboot -no-shutdown -M smm=off -serial stdio -machine q35 -cpu qemu64 -smp 8 -cdrom ../Bin/kot.iso -m 256M -s -S

pause