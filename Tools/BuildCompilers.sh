SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH

if [ -f ../Toolchain/local/bin/clang ]; then
    mkdir -m 777 -p "../Compilers"
    clang-14 -Wall -DKOT_BASE_PATH="\"$SCRIPTPATH/..\"" "compilers/kot-x86_64-clang.c" -o "../Compilers/kot-x86_64-clang"
    sudo cp "../Compilers/kot-x86_64-clang" "/usr/bin"
else
    printf "\033[0;31m /!\\ \033[0m kot-x86_64-clang cross compiler does not exist.\n"
    printf "  ↳ \033[0;36m ⓘ \033[0m Run \x1B[1mmake install-llvm-toolchain\x1B[0m\n"
    kill -15 $PPID
fi