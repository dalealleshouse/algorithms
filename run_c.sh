#!/usr/bin/env bash

CMD=$1

if [ -z "$CMD" ]
then
    CMD="make -B"
fi

docker run --privileged --rm -it -v $(pwd):/src \
dalealleshouse/algo_test_runner_c $CMD
