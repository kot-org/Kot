SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
python3 initrd.py "../Bin/Modules/" "../Bin/BootFiles/initrd.bin" "System.elf"