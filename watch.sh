#!/usr/bin/env bash

inotify-hookable -q -w ./ -C "\.[ch]$=make $1 --jobs=8"
