#!/usr/bin/env bash

set -e

make -B
python compare_times.py
