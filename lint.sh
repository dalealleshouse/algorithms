#!/usr/bin/env bash
LINT_CMD="^(\./\w*\.[ch])=clang-tidy *.c -checks=-clang-diagnostic-unused-command-line-argument"

inotify-hookable -q -w ./ -C "$LINT_CMD"
