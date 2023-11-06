#!/usr/bin/env bash

if [[ $# -eq 0 ]] ; then
  echo 'please provide a version number e.g.:'
  echo './build_container.sh 1.1'
  exit 0
fi

case "$1" in
  push) podman push dalealleshouse/algo_test_runner_c:$2 \
      podman push dalealleshouse/algo_test_runner_c:latest;;
  *) podman build -t dalealleshouse/algo_test_runner_c:$1 \
    -t dalealleshouse/algo_test_runner_c \
    ./
    podman system prune -f ;;
esac
