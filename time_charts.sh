#!/usr/bin/env bash

# This is most likely broken, it has not been repaired after the reconfiguration
# of the build system

docker run --privileged --rm -v $(pwd):/src \
    dalealleshouse/algo_test_runner_c make -B shared

docker run --privileged --rm -v $(pwd):/src \
    dalealleshouse/algo_test_runner_py python3 compare_times.py
