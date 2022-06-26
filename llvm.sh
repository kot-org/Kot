
export LLVM_TARGETS="x86_64"

# je garde ça juste pour rappeler qu'il ne faut pas obulier les dépendances

which git || (echo "Install git: brew install git"; exit)
which cmake || (echo "Install cmake: brew install cmake"; exit)
which ninja || (echo "Install ninja: brew install ninja"; exit)

# là faut créer les fichiers
mkdir Toolchain/Build
mkdir Toolchain/Build/llvm
mkdir Toolchain/Sources
mkdir Toolchain/Sources/llvm


cd Toolchain/

# pour pwd jsp si ça existe en js 
export TOOLCHAIN_DIR=`pwd`


# clone le repos
git clone https://github.com/llvm/llvm-project.git $TOOLCHAIN_DIR/Sources/llvm

echo "Configuring llvm..."

pushd build/llvm
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$TOOLCHAIN_DIR/llvm \
    -DLLVM_ENABLE_PROJECTS="clang;lldb" \
    -DLLVM_TARGETS_TO_BUILD=$LLVM_TARGETS \
    -DLLVM_USE_SPLIT_DWARF=True -DLLVM_OPTIMIZED_TABLEGEN=True \
    -DLLVM_BUILD_TESTS=False -DLLVM_INCLUDE_TESTS=False -DLLDB_INCLUDE_TESTS=False \
    -DLLVM_BUILD_DOCS=False -DLLVM_INCLUDE_DOCS=False \
    -DLLVM_ENABLE_OCAMLDOC=False -DLLVM_ENABLE_BINDINGS=False \
    -DLLDB_USE_SYSTEM_DEBUGSERVER=True \ 

echo "Building llvm... this may take a long while"

cmake --build .

echo "Installing llvm and all tools..."

cmake --build . --target install
popd


export EXTRA_OPTIONS="-I$TOOLCHAIN_DIR/Sysroot/include"
export EXTRA_LD_OPTIONS="-L$TOOLCHAIN_DIR/Sysroot/lib -lc"

cd build/llvm2
CC=$TOOLCHAIN_DIR/clang/bin/clang CXX=$TOOLCHAIN_DIR/clang/bin/clang++ \
../../sources/llvm/configure --prefix=$TOOLCHAIN_DIR/clang/ --enable-jit --enable-optimized \
--enable-libcpp --disable-docs \
--with-binutils-include=$TOOLCHAIN_DIR/sources/binutils-${BINUTILS_VER}/include/ --enable-pic \
--enable-targets=$LLVM_TARGETS

echo "Building llvm... this may take a long while"

make -j EXTRA_OPTIONS="$EXTRA_OPTIONS" EXTRA_LD_OPTIONS="$EXTRA_LD_OPTIONS"
make check

echo "Installing llvm & clang..."

make install EXTRA_OPTIONS="$EXTRA_OPTIONS" EXTRA_LD_OPTIONS="$EXTRA_LD_OPTIONS"
cd ../..
