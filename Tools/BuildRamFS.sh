File="$(dirname $(realpath $(dirname "$0")))/Tools"
BinFile="$(dirname $(realpath $(dirname "$0")))/Bin"
mkdir -m 777 "$BinFile/Modules"
python3 $File/ramfs.py "$BinFile/Modules/" "$BinFile/Limine/RamFS.bin" "System.elf"