#!/bin/bash

inotify-hookable -q -f strassen.c -f strassen.h -f strassen_test.c -c 'clear && make'
