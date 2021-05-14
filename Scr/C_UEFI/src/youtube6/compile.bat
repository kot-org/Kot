gcc -Wall -m64 -mabi=ms -ffreestanding -c main.c -o main.o

gcc -Wall -m64 -mabi=ms -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI main.o

@echo off
del *.o
@echo on

pause