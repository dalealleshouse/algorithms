#!/usr/bin/env bash

function run_tests() {
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

cd closest_pair/c/
run_tests

cd ../../inversion_count/c
run_tests

cd ../../quick_select/c
run_tests

cd ../../sorting/c
run_tests

cd ../../strassen_matrix_multiplication/c
run_tests

exit 0
