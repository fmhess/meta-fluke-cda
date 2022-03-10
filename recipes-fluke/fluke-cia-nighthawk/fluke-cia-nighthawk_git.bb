SUMMARY = "Fluke Nighthawk application"
DESCRIPTION = "Fluke Nighthawk application"
HOMEPAGE = "https://https://github.com/FlukeCorp/nighthawk-sw-src"
SECTION = "libs"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${WORKDIR}/extra_files/LICENSE;md5=fce4b53e0185d2bc7e836efefa539090"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

DEPENDS += " \
    fluke-cia \
	qtbase \
	qtdeclarative \
	python3-xlrd-native \
	libeigen \
	libusb1 \
"
RDEPENDS_${PN} += " \
	qtquickcontrols2-qmlplugins \
	systemd \
"

SRC_URI = "git://github.com/FlukeCorp/nighthawk-sw-src.git;protocol=https;branch=develop;destsuffix=git/;name=nighthawksrc \
 	git://github.com/FlukeCorp/cia-sw-utils.git;protocol=https;branch=develop;destsuffix=git/cia/utils;name=ciautils \
	file://extra_files/LICENSE \
"
# 	git://github.com/FlukeCorp/nighthawk-sw-utils.git;protocol=https;branch=develop;destsuffix=git/cia/utils;name=nighthawkutils \
# 	git://github.com/FlukeCorp/nighthawk-sw-thirdparty.git;protocol=https;branch=fmhess-develop;destsuffix=git/cia/thirdparty;name=nighthawkthirdparty \
#
#SRCREV_FORMAT = "nighthawksrc_nighthawkutils_nighthawkthirdparty"
SRCREV_FORMAT = "nighthawksrc_ciautils"
SRCREV ?= "${AUTOREV}"
PV = "git_${SRCREV}"
PR = "r0"

S = "${WORKDIR}/git"

FLUKE_CIA_DIR = "/opt/fluke/cia"
prefix = "${FLUKE_CIA_DIR}"
#Linux FHS special case weirdness
localstatedir = "/var${prefix}"
sysconfdir = "/etc${prefix}"
#runstatedir is not something yocto handles, so we have to set CMAKE_INSTALL_RUNSTATEDIR
#later ourselves
#runstatedir = "/run${prefix}"

inherit cmake
inherit cmake_qt5
inherit python3native

OECMAKE_C_FLAGS_append = " -Wno-psabi -I${STAGING_DIR_TARGET}${FLUKE_CIA_DIR}/include"
OECMAKE_CXX_FLAGS_append = " -Wno-psabi -I${STAGING_DIR_TARGET}${FLUKE_CIA_DIR}/include"
OECMAKE_RPATH = "${libdir}"
EXTRA_OECMAKE += " \
	-DBUILD_SHARED_LIBS:BOOL=ON \
	-DUSE_CIALEGACY_LOCALSTATEDIR:BOOL=OFF \
	-DLINK_PREBUILT_CIA_LIBS:BOOL=ON \
	-DCMAKE_INSTALL_RUNSTATEDIR=/run${prefix} \
	-DSYSTEMD_SYSTEM_UNITDIR=${systemd_system_unitdir} \
"

#adjusted.json shouldn't be overwritten if it already exists, so put it in CONFFILES
CONFFILES_${PN} += " \
    ${localstatedir}/caldata/adjusted.json \
"

FILES_${PN} += " \
    ${datadir}/caldata/ \
    ${datadir}/error/ \
    ${datadir}/procdata/ \
    ${datadir}/range/ \
    ${datadir}/seq/ \
    ${datadir}/settingsNV/ \
    ${datadir}/updatedata/ \
    ${datadir}/NighthawkSplash.bin \
    ${systemd_system_unitdir}/ \
"
