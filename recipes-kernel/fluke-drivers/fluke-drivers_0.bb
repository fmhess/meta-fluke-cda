SUMMARY = "Miscellaneous Fluke kernel modules"
DESCRIPTION = "Miscellaneous Fluke kernel modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit module

#SRCREV = "${AUTOREV}"
PR = "r0"

SRC_URI = "file://src/"

S = "${WORKDIR}/src/"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES_${PN} += "fluke-drivers"

EXTRA_OEMAKE += "LINUX_SRCDIR=${STAGING_KERNEL_DIR}"

MODULES_INSTALL_TARGET = "install"
MODULES_MODULE_SYMVERS_LOCATION = "drivers/"
