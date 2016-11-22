#!/bin/sh
readelf -s kernel.bin > debug.info
objdump -d kernel.bin >>debug.info
