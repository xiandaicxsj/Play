#!/bin/sh
qemu-system-i386 -drive file=a.img,index=0,if=floppy -m 256M -monitor stdio
-gdb tcp::1234 -S
