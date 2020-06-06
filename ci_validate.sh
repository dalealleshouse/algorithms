#!/usr/bin/env bash

docker run --privileged --rm --tty \
    -v $(pwd)/src:/build/src \
    dalealleshouse/algo_test_runner_c:2.2 \
    ./validate.sh
