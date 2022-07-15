SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd $SCRIPTPATH
cd ../

if [[ ! -f "./llvm.sh" ]] 
then
    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    sudo ./llvm.sh 14 all
    sudo apt update

    sudo rm -r ./llvm.sh
fi