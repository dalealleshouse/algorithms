#!/usr/bin/env bash

if [[ $# -eq 0 ]] ; then
  echo 'please provide a version number e.g.:'
  echo './build 1.1'
  exit 0
fi

case "$1" in
  push) docker push dalealleshouse/algo_test_runner_c ;;
  *) docker build -t dalealleshouse/algo_test_runner_c:$1 \
    -t dalealleshouse/algo_test_runner_c \
    ./
    docker system prune -f ;;
esac
