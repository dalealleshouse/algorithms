#!/usr/bin/env bash

# Check if podman or docker is installed
if command -v podman &> /dev/null; then
  CONTAINER_TOOL="podman"
  OPTIONS="--privileged --rm --tty -it -v $(pwd):/build dalealleshouse/algo_test_runner_c:4.1 $1"
elif command -v docker &> /dev/null; then
  CONTAINER_TOOL="docker"
  OPTIONS="--privileged --rm --tty -it -v $(pwd):/build --user \"$(id -u):$(id -g)\" dalealleshouse/algo_test_runner_c:4.1 $1"
else
  echo "Error: Neither podman nor docker is installed."
  exit 1
fi

$CONTAINER_TOOL run $OPTIONS
