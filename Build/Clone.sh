cd ../Bin

mmd -D s -i disk.img ::/EFI
mmd -D s -i disk.img ::/EFI/BOOT 

mcopy -D s -i disk.img disk/EFI/BOOT/BOOTX64.EFI ::/EFI/BOOT
mcopy -D s -i disk.img disk/zap-light16.psf :: 
mcopy -D s -i disk.img disk/startup.nsh :: 
mcopy -D s -i disk.img disk/kernel.elf :: 
mcopy -D s -i disk.img disk/ramfs.bin :: 
mcopy -D s -i disk.img disk/logo.bmp :: 