#!/bin/sh

# The /boot/kexec_load.sh script is run by the rescue_fluke_cda program to load
# the main kernel for booting.
# The /boot/kexec_load_alternate.sh script may alternately be used by entering
# the rescue_fluke_cda boot menu before it auto-boots and selecting the 
# appropriate option.

# This file may be edited to change the kernel to boot or its boot parameters.
# To allow the kernel to use all available cores, remove the "isolcpus=1" option.
# To enable kernel output to the serial console, change the "console=null" 
# option to "console=ttyS0,115200".
kexec --load zImage-5.4.13-altera \
	--append "console=null vt.global_cursor_default=0 vt.cur_default=1 coherent_pool=256K isolcpus=1 root=/dev/mmcblk0p2 rw rootwait rootfstype=ext4" \
	2> /dev/console;
