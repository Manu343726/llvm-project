#/bin/sh
pushd leg_examples
../build_linux_gcc/bin/clang --target=leg -mllvm -debug main.cpp -ffreestanding -O2 -S -o main.s
popd