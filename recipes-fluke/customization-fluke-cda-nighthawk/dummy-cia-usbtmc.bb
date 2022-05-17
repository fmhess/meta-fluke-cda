SUMMARY = "Dummy usbtmc recipe to generate license package"
DESCRIPTION = "Dummy usbtmc recipe."
AUTHOR = "Tektronix"
#SECTION = "base"
HOMEPAGE = ""

LICENSE = "LGPLv2.1+"
LIC_FILES_CHKSUM = "file://usbtmc/COPYING.LESSER;md5=3000208d539ec061b899bce1d9ce9404 \
"

SRC_URI = "git://github.com/FlukeCorp/cia-sw-thirdparty.git;protocol=https;branch=develop"

SRCREV ?= "${AUTOREV}"

FILES_${PN} += " \
"

S = "${WORKDIR}/git"

#FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = "\
"

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
}

RPROVIDES_${PN} += "${PN}-lic"
