SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
python3 keyboard/Keyboard.py
python3 initrd/Initrd.py "../Bin/Modules/" "../Bin/BootFiles/initrd.bin" "System.elf"
python3 disk/VirtualDisk.py  "../Bin/kot.img" 1G "disk/Storage.json"