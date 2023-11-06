#!/usr/bin/env bash

# add this line into the docker run command to increase the stack size. This
# is useful for testing large inputs on recursive algorithms.
# --ulimit stack=8277716992:8277716992 \

# Check if arguments are provided
if [[ $# -eq 0 ]]; then
  echo 'Please provide a version number e.g.:'
  echo './build_container.sh 1.1'
  exit 0
fi

# Check if podman or docker is installed
if command -v podman &> /dev/null; then
  CONTAINER_TOOL="podman"
elif command -v docker &> /dev/null; then
  CONTAINER_TOOL="docker"
else
  echo "Error: Neither podman nor docker is installed."
  exit 1
fi

case "$1" in
  push) $CONTAINER_TOOL push dalealleshouse/algo_test_runner_c ;;
  *) $CONTAINER_TOOL build -t dalealleshouse/algo_test_runner_c:$1 \
       -t dalealleshouse/algo_test_runner_c \
       ./
     $CONTAINER_TOOL system prune -f ;;
esac


# podman run --privileged --rm --tty -it \
#     -v $(pwd):/build \
#     dalealleshouse/algo_test_runner_c:4.0 \
#     $1

# docker run --privileged --rm --tty -it \
#     -v $(pwd):/build \
#     --user "$(id -u):$(id -g)" \
#     dalealleshouse/algo_test_runner_c:latest \
#     $1
