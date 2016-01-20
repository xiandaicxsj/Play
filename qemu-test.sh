#!/bin/sh
qemu-system-i386 -drive file=a.img,index=0,if=floppy -m 3G --enable-kvm
