cd ../Bin

sudo dd if=/dev/zero of=disk.img bs=1440K count=1200
sudo mformat -i disk.img -f 720
