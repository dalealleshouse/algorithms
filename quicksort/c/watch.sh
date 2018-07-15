#!/usr/bin/env bash

# inotify-hookable -q  -f ./*.c -f ./*.h -c 'docker.exe run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c'
inotify-hookable -q -w ./ \
    -C "^(\./\w*\.[ch])=docker.exe run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c"
