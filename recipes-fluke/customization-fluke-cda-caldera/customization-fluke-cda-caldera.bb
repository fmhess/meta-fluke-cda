SUMMARY = "Fluke CDA customization for Nighthawk"
DESCRIPTION = "Fluke Caldera application, etc."
AUTHOR = "Fluke"
SECTION = ""
HOMEPAGE = ""

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f19e4f8ee79c4b8ff1fbb449f7e56c54"

#PR = "r0"

#libQt5Widgets.so.5, libQt5Gui.so.5, libQt5Core.so.5 used by /home/Caldera/bin/caldera.elf
#libssl.so.1.0.2, libcrypto.so.1.0.2 used by /home/Caldera/bin/update
DEPENDS = " \
	openssl \
	qtbase \
"
# DEPENDS = " \
# 	fontconfig \
# 	freetype \
# 	libpng \
# 	openssl \
# 	linux-gpib-user \
# 	qtbase \
# 	qtdeclarative \
# "
# RDEPENDS_${PN} += " \
# 	qtquickcontrols2-qmlplugins \
# "

SRC_URI = "git://github.com/ADorchak/sumo-rootfs-extras.git;protocol=https;branch=master"

SRCREV ?= "${AUTOREV}"

FILES_${PN} += " \
	${base_prefix}/www/* \
	${base_prefix}/home/* \
	${base_prefix}/tmp/customization_data/* \
	${base_prefix}/config/ \
"

S = "${WORKDIR}/git"

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
	(
		cd ${S}/common/extraFiles/target_root/ &&
			find -type f \! -executable -exec install -D -m 644 \{\} ${D}/\{\} \; &&
			find -type f -executable -exec install -D -m 755 \{\} ${D}/\{\} \; 
	)
	(
		cd ${S}/Caldera/extraFiles/target_root/ &&
			find -type f \! -executable -exec install -D -m 644 \{\} ${D}/\{\} \; &&
			find -type f -executable -exec install -D -m 755 \{\} ${D}/\{\} \; 
	)
	install -d ${D}/home/Test
	install -d ${D}/home/Proto
	install -d ${D}/config
}

