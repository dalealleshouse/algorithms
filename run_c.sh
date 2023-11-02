#!/usr/bin/env bash

# add this line into the docker run command to increase the stack size. This
# is useful for testing large inputs on recursive algorithms.
# --ulimit stack=8277716992:8277716992 \

podman run --privileged --rm --tty -it \
    -v $(pwd):/build \
    dalealleshouse/algo_test_runner_c:4.0 \
    $1
