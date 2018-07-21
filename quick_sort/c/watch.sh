#!/usr/bin/env bash

# you need to have privileged to run address-san
DOCKER_CMD="^(\./\w*\.[ch])=docker.exe run --privileged --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c make -B"

inotify-hookable -q -w ./ -C "$DOCKER_CMD $1"
