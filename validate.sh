#!/usr/bin/env bash

# Force the script to exit when it encounters a non-0 return value
set -e

function print_success() {
    echo -en "\033[32m"
    echo $1
    echo -en "\033[0m"
}

function print_error() {
    echo -en "\033[31m"
    echo $1
    echo -en "\033[0m"
}


function test_build() {
    make -B $1 --jobs=8
    print_success "$1 Success"
}

# Clear the screen so results don't get confused between runs
clear

# Validate that all formatting conforms to the style specified in .clang-format
./validate_format.py -r \
    --exclude src/hashing/farmhash.c \
    src
print_success "Format Check Passed"

# TODO: cpplint
# make lint
cpplint src/*.c
cpplint src/list_data_structures/*.[ch]
cpplint locality/*.c
print_success "Lint Passed"

# TODO: clang-tidy
# make tidy
clang-tidy src/test_runner.c
clang-tidy src/list_data_structures/*.[ch]
clang-tidy locality/*.c
print_success "Tidy Static Analysis Passed"

# Build with undefined and address clang sanitizers and run all tests
test_build address-san

# Build with undefined and memory clang sanitizers and run all tests
test_build memory-san

# Build with undefined and thread clang sanitizers and run all tests
test_build thread-san

# Build with no sanitizers and max optimization level (O3) and run all tests
test_build release

print_success "Build Passed"
