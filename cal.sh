#!/bin/sh
find . -type f -name "*.c" -exec cat {} \; | grep -v '^$' | wc -l
