SUMMARY = "Fluke CDA customization for Kraken"
DESCRIPTION = "Kraken application, etc."
AUTHOR = "Fluke"
#SECTION = "base"
HOMEPAGE = ""

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ae270d4118d7e64218492af5b3f3249b"

#PR = "r0"

DEPENDS = " \
	linux-gpib-user \
"
# RDEPENDS_${PN} += " \
# 	systemd \
# "

SRC_URI = "git://github.com/ADorchak/sumo-rootfs-extras.git;protocol=https;branch=master"

SRCREV ?= "${AUTOREV}"

FILES_${PN} += " \
	${base_prefix}/www/* \
	${base_prefix}/home/* \
	${base_prefix}/tmp/customization_data/* \
	${base_prefix}${systemd_system_unitdir} \
	${base_prefix}${sysconfdir}/systemd/system/multi-user.target.wants/ \
"

S = "${WORKDIR}/git"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
# SRC_URI_append = "\
# 	file://instrument.service \
# 	file://launchApp.service \
# "

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
	(
		cd ${S}/common/extraFiles/target_root/ &&
			find -type f -exec sh -c 'install -D -m $(stat -c "%a" "$1") "$1" ${D}/"$1"' sh \{\} \;
	)
	(
		cd ${S}/Kraken/extraFiles/target_root/ &&
			find -type f -exec sh -c 'install -D -m $(stat -c "%a" "$1") "$1" ${D}/"$1"' sh \{\} \;
	)
	#fixup ssh dir permissions
	chmod 700 ${D}${ROOT_HOME}/.ssh

	#install systemd services for starting instrument app
# 	install -d ${D}${systemd_system_unitdir}
# 	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
# 	install -m 644 ${WORKDIR}/instrument.service ${D}${systemd_system_unitdir}/
# 	install -m 644 ${WORKDIR}/launchApp.service ${D}${systemd_system_unitdir}/
# 	ln -sr ${D}${systemd_system_unitdir}/instrument.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
# 	ln -sr ${D}${systemd_system_unitdir}/launchApp.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}
