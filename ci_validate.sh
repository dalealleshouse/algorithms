#!/usr/bin/env bash

cd sorting/c
docker run --privileged --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c \
  ./validate.sh > run_result.txt

exit -1


