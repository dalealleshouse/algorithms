#!/usr/bin/env bash

declare -a libs=(utils list_data_structures data_structures graph_search)

function clean_libs() {
    for D in `find . -type d -name 'c' ! -path './docker/c'`
    do
        mkdir -p $D/include
        rm -rf $D/include/*
    done
}


function build_lib() {
    pushd ./
    cd $1/c
    ../../run_c.sh "make -B clean" > /dev/null
    ../../run_c.sh "make -B library LIB_NAME=$1"
    popd
}

function copy_libs() {
    for D in `find . -type d -name 'c' ! -path './docker/c'`
    do
        cp $1/c/*.a $D/include
        cp $1/c/*.h $D/include
        sed -i -e 's#include/##g' $D/include/*.h
    done
}

clean_libs

for lib in "${libs[@]}"
do
    echo $lib
    build_lib $lib
    copy_libs $lib
done

exit 0
