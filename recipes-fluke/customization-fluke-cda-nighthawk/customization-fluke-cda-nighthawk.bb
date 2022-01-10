SUMMARY = "Fluke CDA customization for Nighthawk"
DESCRIPTION = "Nighthawk application, etc."
AUTHOR = "Fluke"
SECTION = ""
HOMEPAGE = ""

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ae270d4118d7e64218492af5b3f3249b"

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
	file://extra_files/ \
"

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
	(
		cd ${S}/common/extraFiles/target_root/ &&
			find -type f -exec sh -c 'install -D -m $(stat -c "%a" "$1") "$1" ${D}/"$1"' sh \{\} \;
	)
	(
		cd ${S}/Nighthawk/extraFiles/target_root/ &&
			find -type f -exec sh -c 'install -D -m $(stat -c "%a" "$1") "$1" ${D}/"$1"' sh \{\} \;
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
	install -d ${D}${sbindir}
	install -m 755 ${WORKDIR}/extra_files/doDtreeOverlay ${D}${sbindir}/
	ln -sr ${D}${systemd_system_unitdir}/instrument.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	# The launchApp.service is installed as part of the application, we are just pre-setting up a symlink for it
	# to make it start on boot when it gets copied into /lib/systemd/system/ later.
	ln -sr ${D}${systemd_system_unitdir}/launchApp.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/

	#auto-load kernel modules needed for serial gadget
	install -d ${D}${sysconfdir}/modules-load.d
	install -m 644 ${WORKDIR}/extra_files/fluke_gadgets.conf ${D}${sysconfdir}/modules-load.d/
}
