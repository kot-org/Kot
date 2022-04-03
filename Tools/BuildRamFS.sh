BinFile="$(dirname $(realpath $(dirname "$0")))/Bin"
mkdir -m 777 "$BinFile/Modules"
python3 ramfs.py "$BinFile/Modules/" "$BinFile/Disk/RamFS.bin" "System.elf"