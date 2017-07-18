#!/bin/sh
qemu-system-i386 -cpu qemu32,-apic -hda a.img -m 32M -monitor stdio
