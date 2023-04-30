SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
python3 keyboard.py
python3 Initrd.py "../Bin/Modules/" "../Bin/BootFiles/initrd.bin" "System.elf"
python3 VirtualDisk.py  "../Bin/kot.img" 1G "Storage.json"