BINDIR="./Bin"

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH
cd "../"

echo -e "\e[32mCreating disk...\e[0m"
mkdir -m 777 -p "Disk"
git clone "https://github.com/kot-org/Disk"
mkdir -m 777 -p "Disk/user/root/Home"
mkdir -m 777 -p "Disk/user/root/Desktop"
mkdir -m 777 -p "Disk/user/root/Documents"
mkdir -m 777 -p "Disk/user/root/Downloads"
mkdir -m 777 -p "Disk/user/root/Music"
mkdir -m 777 -p "Disk/user/root/Pictures"
mkdir -m 777 -p "Disk/user/root/Videos"
mkdir -m 777 -p "Disk/user/root/Trash"

echo -e "\e[32mCreating compilation dirs...\e[0m"

mkdir -m 777 -p "Sysroot/usr/include/"
mkdir -m 777 -p "Sysroot/usr/lib/"
mkdir -m 777 -p $BINDIR"/Modules/"
mkdir -m 777 -p $BINDIR"/Firmwares/"

cp -p -R ./Build/Bin/Modules/** $BINDIR"/Modules/"
cp -p -R ./Build/Bin/Firmwares/** $BINDIR"/Firmwares/"