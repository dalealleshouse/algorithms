#!/usr/bin/env bash

function run_c_tests() {
    docker run --privileged --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c \
        ./validate.sh > run_result.txt

    RESULTS=`tail run_result.txt`

    if [[ "$RESULTS" == *Success* ]]
    then
        echo `pwd` passed
    else
        echo `pwd` failure
        exit -1
    fi
}

function run_c_code_coverage() {
    docker run --privileged --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c \
        ./coverage.sh
}

# Run the dockerized tests inside every subdirectory named c
for D in `find . -type d -name 'c' ! -path './docker/c'`
do
    cd $D
    run_c_tests
    cd ../../
done

exit 0
