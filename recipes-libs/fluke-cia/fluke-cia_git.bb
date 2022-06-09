inherit cmake
inherit python3native

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

SRC_URI = "git://github.com/FlukeCorp/cia-sw-src.git;protocol=https;branch=develop;destsuffix=git/cia/src;name=ciasrc \
	git://github.com/FlukeCorp/cia-sw-utils.git;protocol=https;branch=develop;destsuffix=git/cia/utils;name=ciautils \
	git://github.com/FlukeCorp/cia-sw-thirdparty.git;protocol=https;branch=develop;destsuffix=git/cia/thirdparty;name=ciathirdparty \
	file://extra_files/LICENSE \
	file://git/cia/CMakeLists.txt \
"
SRCREV_FORMAT = "ciasrc_ciautils_ciathirdparty"
SRCREV ?= "${AUTOREV}"
PV = "git_${SRCREV}"
PR = "r0"

S = "${WORKDIR}/git/cia"

# Legacy install
prefix = "/home/Nighthawk"
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
	-DCMAKE_INSTALL_RUNSTATEDIR=/tmp \
"

# # Linux FHS install
# prefix = "/opt/fluke/cia"
# #Linux FHS special case weirdness
# localstatedir = "/var${prefix}"
# sysconfdir = "/etc${prefix}"
# #runstatedir is not something yocto handles, so we have to set CMAKE_INSTALL_RUNSTATEDIR
# #later ourselves
# #runstatedir = "/run${prefix}"
# EXTRA_OECMAKE += " \
#     -DBUILD_SHARED_LIBS:BOOL=ON \
# 	-DUSE_CIALEGACY_LOCALSTATEDIR:BOOL=OFF \
# 	-DCMAKE_INSTALL_RUNSTATEDIR=/run${prefix} \
# "
# OECMAKE_RPATH = "${libdir}"

OECMAKE_C_FLAGS_append = " -Wno-psabi "
OECMAKE_CXX_FLAGS_append = " -Wno-psabi "

# we add ${prefix} to FILES for the sake of the "legacy install" to /home/Nighthawk
FILES_${PN} += " \
    ${datadir} \
    ${prefix} \
"
