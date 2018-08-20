#!/usr/bin/env bash

make -B code-coverage > /dev/null

for F in `find ./ -name "*.c" ! -name "*test*" ! -name "algo*"`
do
    llvm-cov gcov $F -n
done
