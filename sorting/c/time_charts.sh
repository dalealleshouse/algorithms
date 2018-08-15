#!/usr/bin/env bash

docker run --privileged --rm -v $(pwd):/src \
    dalealleshouse/algo_test_runner_c make -B shared

docker run --privileged --rm --ulimit stack=268435456 -v $(pwd):/src \
    dalealleshouse/algo_test_runner_py python3 compare_times.py
