#customize systemd config.

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://instrument.service \
    file://launchApp.service \
"


do_install_append() {
	echo NAutoVTs=0 >> ${D}${sysconfdir}/systemd/logind.conf
	echo ReserveVT=0 >> ${D}${sysconfdir}/systemd/logind.conf

	install -m 644 ${WORKDIR}/instrument.service ${D}${systemd_system_unitdir}/
	install -m 644 ${WORKDIR}/launchApp.service ${D}${systemd_system_unitdir}/
	ln -sr ${D}${systemd_system_unitdir}/instrument.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${systemd_system_unitdir}/launchApp.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}
