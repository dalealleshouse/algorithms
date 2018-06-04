#!/bin/bash

inotify-hookable -q -f inversions.py -f inversions_test.py -c 'clear && pytest -v'
