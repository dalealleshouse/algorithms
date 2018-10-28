#!/usr/bin/env bash

make clean > /dev/null
make -B code-coverage > /dev/null

lcov --no-external \
    --directory ./src \
    --base-directory ./src \
    --gcov-tool llvm-gcov.sh \
    --capture -o ./src/cov.info

lcov --remove ./src/cov.info '/src/*test*' '/src/*Test*' '/src/algo*' '/src/MemAllocMock*' \
    -o ./src/cov.info

genhtml ./src/cov.info -o ./src/output
