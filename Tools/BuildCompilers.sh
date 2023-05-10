SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH

mkdir -m 777 -p "../Compilers"
clang-14 -Wall -DKOT_BASE_PATH="\"$SCRIPTPATH/..\"" "compilers/kot-clang.c" -o "../Compilers/kot-clang"
sudo cp "../Compilers/kot-clang" "/usr/bin"