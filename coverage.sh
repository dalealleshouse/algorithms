#!/usr/bin/env bash

make -B code-coverage > /dev/null

lcov --no-external \
    --directory ./src \
    --base-directory ./src \
    --gcov-tool ./llvm-gcov.sh \
    --capture -o ./src/cov.info

lcov --remove ./src/cov.info '*test.c' '*algo_timer.c' \
    -o ./src/cov.info

genhtml ./src/cov.info -o ./code_coverage
