#!/usr/bin/env bash

docker run --privileged --rm --tty \
    -v $(pwd)/src:/build/src \
    -v $(pwd)/compile_commands.json:/build/compile_commands.json \
    -v $(pwd)/code_coverage:/build/code_coverage \
    --user "$(id -u):$(id -g)" \
    dalealleshouse/algo_test_runner_c:latest \
    $1
