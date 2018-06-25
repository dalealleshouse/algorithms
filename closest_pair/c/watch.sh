#!/bin/bash

inotify-hookable -q -f closest_pair.c -f closest_pair.h -f closest_pair_test.c -c 'docker.exe run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c'
