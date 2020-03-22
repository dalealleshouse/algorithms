#!/usr/bin/env bash
LINT_CMD="^(\./\w*\.[ch])=clear && clang-tidy *.c -checks=bugprone-*,readablity-*,modernize-*,misc-*,clang-analyzer-*,cert-*,-clang-diagnostic-unused-command-line-argument -header-filter=.*"

inotify-hookable -q -w ./ -C "$LINT_CMD"
