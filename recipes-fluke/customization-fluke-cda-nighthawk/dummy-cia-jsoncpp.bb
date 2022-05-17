SUMMARY = "Dummy jsoncpp recipe to generate license package"
DESCRIPTION = "Dummy jsoncpp recipe."
AUTHOR = "https://github.com/open-source-parsers/jsoncpp"
#SECTION = "base"
HOMEPAGE = ""

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://jsoncpp/LICENSE;md5=5d73c165a0f9e86a1342f32d19ec5926 \
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
