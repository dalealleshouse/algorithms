#!/usr/bin/env bash

# add this line into the docker run command to increase the stack size. This
# is useful for testing large inputs on recursive algorithms.
# --ulimit stack=8277716992:8277716992 \

docker run --privileged --rm --tty -it \
    -v $(pwd):/build \
    --user "$(id -u):$(id -g)" \
    dalealleshouse/algo_test_runner_c:latest \
    $1
