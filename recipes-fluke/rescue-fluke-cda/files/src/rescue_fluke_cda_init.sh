#!/bin/sh
# This is a little script intended to be used as the kernel's init program, 
# to do minimal setup for the rescue_fluke_cda program.

# mount proc and sysfs filesystems needed by kexec
mount /proc
mount /tmp
mount -t sysfs sysfs /sys
rescue_fluke_cda >/dev/tty1 </dev/tty1 2>&1
