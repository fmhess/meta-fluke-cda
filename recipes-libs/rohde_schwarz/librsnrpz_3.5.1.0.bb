SUMMARY = "Rohde & Schwarz NRP power sensor remote control library rsnrpz for Linux"
DESCRIPTION = "https://www.rohde-schwarz.com"
HOMEPAGE = "https://www.rohde-schwarz.com"
SECTION = "libs"
LICENSE = "Proprietary"
#their tarball doesn't actually have a license file, so we'll just pick something to make bitbake happy
LIC_FILES_CHKSUM = "file://rsnrpz.h;md5=68662dea5fc9df6f7a97684a2c82ff25"

DEPENDS = "libnrp"

PV = "3.5.1.0"
PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "file://rsnrpz-${PV}.tar.gz\
	"

S = "${WORKDIR}/rsnrpz"

inherit autotools

# extra options to pass to the configure script
EXTRA_OECONF += "\
	"
