#!/usr/bin/env bash

make -B code-coverage > /dev/null

for F in `find ./ -name "*.gcda" ! -name "*test*" ! -name "algo*"`
do
    llvm-cov gcov -f -b $F
done

lcov --directory ./src \
    --base-directory ./src \
    --gcov-tool ./llvm-gcov.sh \
    --capture -o ./src/cov.info

genhtml ./src/cov.info -o ./src/output
