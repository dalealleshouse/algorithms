#!/bin/bash

inotify-hookable -q -f strassen.c -f strassen.h -f strassen_test.c -c 'docker.exe run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c'
