#customize systemd config.

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_fluke-cda-nighthawk = "\
    file://instrument.service \
    file://launchApp.service \
"


do_install_append_fluke-cda-nighthawk() {
	echo NAutoVTs=0 >> ${D}${sysconfdir}/systemd/logind.conf
	echo ReserveVT=0 >> ${D}${sysconfdir}/systemd/logind.conf

	install -m 644 ${WORKDIR}/instrument.service ${D}${systemd_system_unitdir}/
	install -m 644 ${WORKDIR}/launchApp.service ${D}${systemd_system_unitdir}/
	ln -sr ${D}${systemd_system_unitdir}/instrument.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${systemd_system_unitdir}/launchApp.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/

	#disable auto startup of some systemd services
	rm -f ${D}${sysconfdir}/systemd/system/sockets.target.wants/systemd-networkd.socket
	rm -f ${D}${sysconfdir}/systemd/system/network-online.target.wants/systemd-networkd-wait-online.service
	rm -f ${D}${sysconfdir}/systemd/system/multi-user.target.wants/systemd-networkd.service
	rm -f ${D}${sysconfdir}/systemd/system/multi-user.target.wants/systemd-resolved.service
	rm -f ${D}${sysconfdir}/systemd/system/sysinit.target.wants/systemd-timesyncd.service
	rm -f ${D}${sysconfdir}/systemd/system/getty.target.wants/getty@tty1.service

	rm -f ${D}${sysconfdir}/systemd/system/dbus-org.freedesktop.network1.service
	rm -f ${D}${sysconfdir}/systemd/system/dbus-org.freedesktop.resolve1.service
	# mask some systemd services
	ln -sr ${D}${base_prefix}/dev/null ${D}${sysconfdir}/systemd/system/getty@tty1.service
	ln -sr ${D}${base_prefix}/dev/null ${D}${sysconfdir}/systemd/system/systemd-timesyncd.service
	ln -sr ${D}${base_prefix}/dev/null ${D}${sysconfdir}/systemd/system/systemd-timedated.service
	ln -sr ${D}${base_prefix}/dev/null ${D}${sysconfdir}/systemd/system/systemd-networkd.service
	ln -sr ${D}${base_prefix}/dev/null ${D}${sysconfdir}/systemd/system/systemd-resolved.service
}
