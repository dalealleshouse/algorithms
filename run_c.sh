#!/usr/bin/env bash

docker run --privileged --rm --tty \
    -v $(pwd):/build \
    --user "$(id -u):$(id -g)" \
    dalealleshouse/algo_test_runner_c:latest \
    $1
