#############################################################################
# Copyright (c) 2018 Fluke Corporation, Inc. All rights reserved.
#
# Repository: https://bitbucket.org/flukept/sw-layers-meta-fluke-ccp-thirdparty
# Origin    : Common Computing Platform
#
#############################################################################
# Use of the software source code and warranty disclaimers are
# identified in the Software Agreement associated herewith.
#############################################################################
#
# Common Computing Platform recipe for Google's Roboto Fonts
#
#############################################################################

#############################################################################
# Recipe Information
#############################################################################

SUMMARY = "Google Roboto Fonts"
DESCRIPTION = "Roboto font files from Google, commonly used on Fluke tools"
AUTHOR = "Google"
SECTION = "x11/fonts"
HOMEPAGE = "https://fonts.google.com/specimen/Roboto"
BUGTRACKER = "https://github.com/google/roboto/issues"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"

#PR = "r1"

#############################################################################
# Sources
#############################################################################

SRC_URI = "git://github.com/google/roboto.git"

SRCREV = "ab4876f650a4aeb9b661c83fee47f908c739bb70"

S = "${WORKDIR}/git"

#############################################################################
# Packaging
#############################################################################

# The qt variant is used to allow Qt to find the fonts on systems that
# do not support fontconfig
PACKAGES = "\
    ${PN} \
    ${PN}-qt \
"

FILES_${PN} = "${datadir}/fonts"
FILES_${PN}-qt = "${libdir}"

FONT_PACKAGES = "${PN}"

#############################################################################
# Tasks
#############################################################################

inherit fontcache
inherit allarch

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_configure[noexec] = "1"

do_install () {
    # We are using the hinted version for the time being
    install -d ${D}${datadir}/fonts/ttf/
    install -d ${D}${libdir}/fonts/
    for i in src/hinted/*.ttf; do
        fontname=$(basename "$i")

        # Install into the /usr/share directory for fontconfig to find
        install -m 644 $i ${D}${datadir}/fonts/ttf/${fontname}

        # Install into the /usr/lib/fonts directory for Qt's
        # QBasicFontDatabase to find (if system doesn't use fontconfig)
        install -m 644 $i ${D}${libdir}/fonts/${fontname}
    done
}

