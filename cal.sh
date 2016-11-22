#!/bin/sh
find . -type f -name "*.c" -exec cat {} \; | grep -v '^$' | wc -l
find . -type f -name "*.asm" -exec cat {} \; | grep -v '^$' | wc -l
find . -type f -name "*.S" -exec cat {} \; | grep -v '^$' | wc -l
