#This builds a rescue kernel similar to our normal kernel but with a modified config
#to support kexec, and ideally to disable various unnecessary options to give a faster
#rescue boot.  This file is based on the meta-altera kernel recipe for linux-altera-ltsi.

PR = "r1"
LINUX_VERSION = "4.9.78"
KERNEL_REPO = "git://github.com/fmhess/linux-socfpga.git"
LINUX_VERSION_SUFFIX = "-ltsi-${MACHINE}"
SRCREV = "${AUTOREV}"

#We have to set KERNEL_PACKAGE_NAME to something other than the
#default of "kernel" or yocto will disable this package, only allowing the
#default kernel package specified by PREFERRED_PROVIDER_virtual/kernel
#to be built.  Also, this kernel will get deployed into a subdir with the
#name specified by KERNEL_PACKAGE_NAME.
KERNEL_PACKAGE_NAME = "fluke-rescue-kernel"

#suppress generating device trees, to avoid bug in kernel-devicetree.bbclass
#where the device tree files are not put into the KERNEL_PACKAGE_NAME
#subdir.  This causes them to collide with the device tree files from the
#default kernel.  This kernel uses the default kernel's device tree anyways, 
#so we don't need to generate the device tree in this recipe.
KERNEL_DEVICETREE = ""

require recipes-kernel/linux/linux-altera.inc

#FILESEXTRAPATHS_prepend := "${THISDIR}/config:"

#SRC_URI_append_cyclone5 = " file://config_lbdaf.cfg "

