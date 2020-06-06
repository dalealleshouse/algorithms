#!/usr/bin/env bash

docker run --privileged --rm \
    -v $(pwd)/src:/build/src \
    dalealleshouse/algo_test_runner_c:2.0 \
    ./validate.sh
