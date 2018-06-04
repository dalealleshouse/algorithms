#!/bin/bash

inotify-hookable -q -f inversions.c -f inversions_test.c -c 'clear && make'
