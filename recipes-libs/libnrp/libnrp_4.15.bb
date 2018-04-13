SUMMARY = "Rohde & Schwarz power sensor NRP library for Linux"
DESCRIPTION = "https://www.rohde-schwarz.com"
HOMEPAGE = "https://www.rohde-schwarz.com"
SECTION = "libs"
LICENSE = "Proprietary"
#their tarball doesn't actually have a license file, so we'll just pick something to make bitbake happy
LIC_FILES_CHKSUM = "file://version.h;md5=f5ad084ad7627a064cfed509a116e9cf"

DEPENDS = "udev"
RDEPENDS_${PN} = "udev"

PV = "4.15"
PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "file://libnrp-${PV}.tar.gz\
	file://nrpzmodule-4.2.3.tar.gz\
	"

S = "${WORKDIR}/libnrp"

inherit autotools pkgconfig

# extra options to pass to the configure script
EXTRA_OECONF += "\
	"

# librnp has a broken nrpzmodule.h (it's supposed to be a symbolic link) so
# fix it here
do_compile_prepend() {
	ln -sf ${WORKDIR}/nrpzmodule/nrpzmodule.h ${S}/nrpzmodule.h
}
