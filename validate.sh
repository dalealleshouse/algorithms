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

print_success "Generating Compilation Database"
make comp-db > /dev/null
print_success "Compilation Database Created"

# Validate that all formatting conforms to the style specified in .clang-format
./validate_format.py -r \
  --clang-format-executable clang-format-15 \
  --exclude src/hashing/include/farmhash.c \
  src
  print_success "Format Check Passed"

# make lint
cpplint src/*.c
cpplint src/list_data_structures/*.[ch]
cpplint src/data_structures/*.[ch]
cpplint src/hashing/*.[ch]
cpplint src/cache/*.[ch]
cpplint src/sorting/*.[ch]
cpplint src/quick_select/*.[ch]
cpplint src/matrix_operations/*.[ch]
cpplint src/running_median/*.[ch]
cpplint src/closest_pair/*.[ch]
cpplint src/inversion_count/*.[ch]
cpplint locality/*.[ch]
print_success "Lint Passed"

# make tidy
clang-tidy-15 src/test_runner.c
clang-tidy-15 src/list_data_structures/*.[ch]
clang-tidy-15 src/data_structures/*.[ch]
clang-tidy-15 src/hashing/*.[ch]
clang-tidy-15 src/cache/*.[ch]
clang-tidy-15 src/sorting/*.[ch]
clang-tidy-15 src/quick_select/*.[ch]
clang-tidy-15 src/matrix_operations/*.[ch]
clang-tidy-15 src/running_median/*.[ch]
clang-tidy-15 src/closest_pair/*.[ch]
clang-tidy-15 src/inversion_count/*.[ch]
clang-tidy-15 locality/*.[ch]
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
