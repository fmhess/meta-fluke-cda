SUMMARY = "Rohde & Schwarz NRP power sensor kernel modules"
DESCRIPTION = "Rohde & Schwarz NRP power sensor kernel modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit module

#SRCREV = "${AUTOREV}"
PR = "r0"

SRC_URI = "\
	file://nrpzmodule-4.2.3.tar.gz\
"

S = "${WORKDIR}/nrpzmodule"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES_${PN} += "nrpz"

EXTRA_OEMAKE += "KDIR=${STAGING_KERNEL_DIR}"

MODULES_INSTALL_TARGET = "install"
#MODULES_MODULE_SYMVERS_LOCATION = "."
