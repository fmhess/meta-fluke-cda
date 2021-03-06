SUMMARY = "Fluke CIA"
DESCRIPTION = "Fluke Common Instrument Architecture"
HOMEPAGE = "https://https://github.com/FlukeCorp/cia-sw-src"
SECTION = "libs"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${WORKDIR}/extra_files/LICENSE;md5=fce4b53e0185d2bc7e836efefa539090"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

DEPENDS += "freetype \
	libpng \
	openssl \
	fontconfig \
	linux-gpib-user \
	sqlite3 \
"

SRC_URI = "git://github.com/FlukeCorp/cia-sw-src.git;protocol=https;branch=fmhess-develop;destsuffix=git/cia/src;name=ciasrc \
	git://github.com/FlukeCorp/cia-sw-utils.git;protocol=https;branch=develop;destsuffix=git/cia/utils;name=ciautils \
	git://github.com/FlukeCorp/cia-sw-thirdparty.git;protocol=https;branch=fmhess-develop;destsuffix=git/cia/thirdparty;name=ciathirdparty \
	file://extra_files/LICENSE \
	file://git/cia/CMakeLists.txt \
"
SRCREV_FORMAT = "ciasrc_ciautils_ciathirdparty"
SRCREV ?= "${AUTOREV}"
PV = "git_${SRCREV}"
PR = "r0"

S = "${WORKDIR}/git/cia"

inherit cmake

EXTRA_OECMAKE += " \
    -DBUILD_SHARED_LIBS=ON \
"
#    -DCMAKE_INSTALL_PREFIX:PATH=/opt/cia \
#

#FIXME: need more specific FILES to specify which subpackage the files go in?
FILES_${PN} += " \
"
#    /opt/cia \
#
