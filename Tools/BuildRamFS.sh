SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
python3 ramfs.py "../Bin/Modules/" "../Bin/Limine/ramfs.bin" "System.elf"