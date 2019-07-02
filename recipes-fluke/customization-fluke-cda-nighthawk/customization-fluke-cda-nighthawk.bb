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
#qtbase, qtdeclarative, qtquickcontrols2-qmlplugins used by "/home/Nighthawk/bin/fp" executable
DEPENDS = " \
	fontconfig \
	freetype \
	libpng \
	openssl \
	linux-gpib-user \
	qtbase \
	qtdeclarative \
"
RDEPENDS_${PN} += " \
	qtquickcontrols2-qmlplugins \
	systemd \
"

SRC_URI = "git://github.com/ADorchak/sumo-rootfs-extras.git;protocol=https;branch=master"

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
			find -type f \! -executable -exec install -D -m 644 \{\} ${D}/\{\} \; &&
			find -type f -executable -exec install -D -m 755 \{\} ${D}/\{\} \; 
	)
	(
		cd ${S}/Nighthawk/extraFiles/target_root/ &&
			find -type f \! -executable -exec install -D -m 644 \{\} ${D}/\{\} \; &&
			find -type f -executable -exec install -D -m 755 \{\} ${D}/\{\} \; 
	)
	#fixup ssh dir permissions
	chmod 700 ${D}${ROOT_HOME}/.ssh 
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
