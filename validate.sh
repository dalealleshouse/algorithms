#!/usr/bin/env bash

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
  make -B $1

  if [ $? -eq 0 ]
  then
    print_success "$1 Success"
  else
    print_error "$1 Failed"
    exit $?
  fi
}

clear

./validate_format.py -r \
    --exclude src/hashing/farmhash.c \
    src
    
print_success "Format Check Passed"

test_build all
test_build address-san
test_build memory-san
test_build thread-san
test_build release
