cd ../UEFI
start build.bat
cd ../build
diskpart /s %CD%\mount.txt
cd ../bin
copy %CD%\*.* e:
copy %CD%\EFI\BOOT\*.* e:\EFI\BOOT\
cd ../build
diskpart /s %CD%\dismount.txt
pause