#!/usr/bin/env bash

docker run --privileged --rm --tty -it \
    -v $(pwd):/build \
    --user "$(id -u):$(id -g)" \
    --ulimit stack=8277716992:8277716992 \
    dalealleshouse/algo_test_runner_c:latest \
    $1
