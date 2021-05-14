set OSNAME=borrrdex
set BUILDDIR=%0/../bin
set BUILDDIR=%BUILDDIR:"=%

set PATH=C:\Program Files\qemu;%PATH%
qemu-system-x86_64 -drive file=%BUILDDIR%/%OSNAME%.iso,if=ide,bus=0,media=cdrom^
    -drive file=%BUILDDIR%/store.img,format=raw,if=ide,bus=0^
    -m 512M^
    -cpu qemu64^
    -machine q35 ^
    -s -S^
    -monitor stdio^
    -rtc base=localtime,clock=host^
    -usb
    ::-smp cores=2,sockets=1
    ::-serial file:%BUILDDIR%/serial.log^
    ::-netdev user,id=en0 -nic user,model=e1000^
::pause
