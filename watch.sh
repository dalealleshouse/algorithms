#!/usr/bin/env bash

inotify-hookable -q -w ./ -C "\.[ch]$=$1"
