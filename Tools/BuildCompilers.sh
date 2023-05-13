SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH

mkdir -m 777 -p "../Compilers"
clang-14 -Wall -DKOT_BASE_PATH="\"$SCRIPTPATH/..\"" "compilers/kot-x86_64-clang.c" -o "../Compilers/kot-x86_64-clang"
sudo cp "../Compilers/kot-x86_64-clang" "/usr/bin"