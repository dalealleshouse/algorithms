#!/usr/bin/env bash

function test_build() {
    make -B $1

    if [ $? -eq 0 ]
    then
        echo -en "\033[32m"
        echo $1 Success
        echo -en "\033[0m"
    else
        echo -en "\033[31m"
        echo $1 Failed
        echo -en "\033[0m"
        exit $?
    fi
}

test_build all
test_build address-san
# disabled due to problems with shared libaries and false positives
# test_build memory-san
test_build thread-san
test_build release
