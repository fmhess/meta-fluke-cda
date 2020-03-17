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
RDEPENDS_${PN} += " \
	systemd \
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

SRC_URI = "git://github.com/ADorchak/sumo-rootfs-extras.git;protocol=https;branch=sumo"

SRCREV ?= "${AUTOREV}"

FILES_${PN} += " \
	${base_prefix}/www/* \
	${base_prefix}/home/* \
	${base_prefix}/tmp/customization_data/* \
	${base_prefix}/config/ \
	${base_prefix}${systemd_system_unitdir} \
	${base_prefix}${sysconfdir}/systemd/system/multi-user.target.wants/ \
"

S = "${WORKDIR}/git"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = "\
	file://instrument.service \
	file://launchApp.service \
"

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
	(
		cd ${S}/common/extraFiles/target_root/ &&
			find -type f -exec sh -c 'install -D -m $(stat -c "%a" "$1") "$1" ${D}/"$1"' sh \{\} \; 
	)
	(
		cd ${S}/Caldera/extraFiles/target_root/ &&
			find -type f -exec sh -c 'install -D -m $(stat -c "%a" "$1") "$1" ${D}/"$1"' sh \{\} \; 
	)
	install -d ${D}/home/Test
	install -d ${D}/home/Proto
	install -d ${D}/config

	#install systemd services for starting instrument app
	install -d ${D}${systemd_system_unitdir}
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	install -m 644 ${WORKDIR}/instrument.service ${D}${systemd_system_unitdir}/
	install -m 644 ${WORKDIR}/launchApp.service ${D}${systemd_system_unitdir}/
	ln -sr ${D}${systemd_system_unitdir}/instrument.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${systemd_system_unitdir}/launchApp.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}

