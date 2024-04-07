#/bin/sh
pushd leg_examples
../build_linux_gcc/bin/clang \
    --target=leg \
    -mllvm -debug \
    main.cpp -S -o main.s \
    -I./ \
    -ffreestanding -O3
popd