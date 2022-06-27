echo "Creating compilation dirs.."

mkdir -p Sysroot/
mkdir -p Sysroot/include/
mkdir -p Sysroot/include/kot/
mkdir -p Sysroot/lib/

echo "Compiling kot libraries.."

cd Sources/Libs/

# libc

cd libc/build/
make
cd ../../

# abi

cd abi/build/
make 
cd ../../

# kernel

cd ../Kernel/
echo "Building kernel..."

make all