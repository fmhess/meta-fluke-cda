SUMMARY = "Fluke CDA customization for Nighthawk"
DESCRIPTION = "Nighthawk application, etc."
AUTHOR = "Fluke"
SECTION = ""
HOMEPAGE = ""

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f19e4f8ee79c4b8ff1fbb449f7e56c54"

#PR = "r0"

#fontconfig, freetype, libpng, libcrypto (provided by openssl) used by "/home/Nighthawk/bin/update" executable
#linux-gpib-user used by "/home/Nighthawk/bin/remoteapp" executable
#qtbase, qtdeclarative used by "/home/Nighthawk/bin/fp" executable
DEPENDS = " \
	fontconfig \
	freetype \
	libpng \
	openssl \
	linux-gpib-user \
	qtbase \
	qtdeclarative \
 "

SRC_URI = "git://github.com/ADorchak/sumo-rootfs-extras.git;protocol=https;branch=master"

SRCREV ?= "${AUTOREV}"

FILES_${PN} += " \
	${base_prefix}/www/* \
	${base_prefix}/home/Nighthawk/* \
	${base_prefix}/home/setup_environment_target \
	${base_prefix}/tmp/customization_data/* \
"

S = "${WORKDIR}/git"

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
	(
		cd ${S}/Nighthawk/extraFiles/target_root/ &&
			find -type f \! -executable -exec install -D -m 644 \{\} ${D}/\{\} \; &&
			find -type f -executable -exec install -D -m 755 \{\} ${D}/\{\} \; 
	)
}

