set OSNAME=borrrdex
set BUILDDIR=%0/../bin
set OVMFDIR=C:\OVMF
set BUILDDIR=%BUILDDIR:"=%
set OVMFDIR=%OVMFDIR:"=%

set PATH=C:\Program Files\qemu;%PATH%
qemu-system-x86_64 -drive file=%BUILDDIR%/%OSNAME%.img^
    -drive file=%BUILDDIR%/store.img^
    -m 512M^
    -cpu qemu64^
    -drive if=pflash,format=raw,unit=0,file=%OVMFDIR%/OVMF_CODE-pure-efi.fd,readonly=on^
    -drive if=pflash,format=raw,unit=1,file=%OVMFDIR%/OVMF_VARS-pure-efi.fd^
    -machine q35 ^
    -s -S^
    -monitor stdio^
    -rtc base=localtime,clock=host^
    -usb^
    -device pci-ohci,id=ohci
    ::-device usb-kbd,bus=ohci.0
    ::-smp cores=2,sockets=1
    ::-serial file:%BUILDDIR%/serial.log^
    ::-netdev user,id=en0 -nic user,model=e1000^
::pause
