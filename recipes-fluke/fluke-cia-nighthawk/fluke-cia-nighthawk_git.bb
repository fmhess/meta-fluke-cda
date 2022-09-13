inherit cmake
inherit cmake_qt5
inherit python3native

SUMMARY = "Fluke Nighthawk application"
DESCRIPTION = "Fluke Nighthawk application"
HOMEPAGE = "https://https://github.com/FlukeCorp/nighthawk-sw-src"
SECTION = "libs"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${WORKDIR}/extra_files/LICENSE;md5=d7eb911ffc729ee7300764c645b5c5eb"

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
	fluke-cia \
	qtquickcontrols2-qmlplugins \
	systemd \
	bash \
	python3-core \
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

# Legacy install
FLUKE_CIA_DIR = "/home/Nighthawk"
prefix = "${FLUKE_CIA_DIR}"
datarootdir = "${prefix}/bin"
datadir = "${datarootdir}"
libexecdir = "${prefix}/bin"
localstatedir = "${prefix}/bin"
sysconfdir = "${prefix}/bin"
docdir = "${prefix}/share/doc"
#runstatedir is not something yocto handles, so we have to set CMAKE_INSTALL_RUNSTATEDIR
#later ourselves
#runstatedir = "/tmp"
EXTRA_OECMAKE += " \
	-DBUILD_SHARED_LIBS:BOOL=OFF \
	-DUSE_CIALEGACY_LOCALSTATEDIR:BOOL=ON \
	-DLINK_PREBUILT_CIA_LIBS:BOOL=ON \
	-DCMAKE_INSTALL_RUNSTATEDIR=/tmp \
	-DSYSTEMD_SYSTEM_UNITDIR=${systemd_system_unitdir} \
"

# # Linux FHS install
# FLUKE_CIA_DIR = "/opt/fluke/cia"
# prefix = "${FLUKE_CIA_DIR}"
# #Linux FHS special case weirdness
# localstatedir = "/var${prefix}"
# sysconfdir = "/etc${prefix}"
# #runstatedir is not something yocto handles, so we have to set CMAKE_INSTALL_RUNSTATEDIR
# #later ourselves
# #runstatedir = "/run${prefix}"
# EXTRA_OECMAKE += " \
# 	-DBUILD_SHARED_LIBS:BOOL=ON \
# 	-DUSE_CIALEGACY_LOCALSTATEDIR:BOOL=OFF \
# 	-DLINK_PREBUILT_CIA_LIBS:BOOL=ON \
# 	-DCMAKE_INSTALL_RUNSTATEDIR=/run${prefix} \
# 	-DSYSTEMD_SYSTEM_UNITDIR=${systemd_system_unitdir} \
# "
# OECMAKE_RPATH = "${libdir}"

OECMAKE_C_FLAGS_append = " -Wno-psabi -I${STAGING_DIR_TARGET}${FLUKE_CIA_DIR}/include"
OECMAKE_CXX_FLAGS_append = " -Wno-psabi -I${STAGING_DIR_TARGET}${FLUKE_CIA_DIR}/include"

#adjusted.json shouldn't be overwritten if it already exists, so put it in CONFFILES
CONFFILES_${PN} += " \
    ${localstatedir}/caldata/adjusted.json \
"

# we add ${prefix} to FILES for the sake of the "legacy install" to /home/Nighthawk
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
    ${prefix} \
"
