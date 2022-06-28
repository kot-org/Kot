File="Tools"
BinFile="Bin"
mkdir -m 777 "$BinFile/Modules"
python3 $File/ramfs.py "$BinFile/Modules/" "$BinFile/Limine/ramfs.bin" "System.elf"