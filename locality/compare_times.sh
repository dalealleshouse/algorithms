#!/usr/bin/env bash

set -e

make -B

time ./row
time ./column
