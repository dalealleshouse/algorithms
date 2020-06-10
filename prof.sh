#!/usr/bin/env bash

make clean > /dev/null
make profile > /dev/null
gprof src/test_runner
