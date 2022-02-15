
#require recipes-images/angstrom/angstrom-image.bb
inherit core-image

DEPENDS += "lighttpd \
	iproute2 \
	linux-gpib-user \
	linux-gpib-kernel \
    ttf-roboto \
    fluke-drivers \
    libgpiod \
"

DEPENDS_append_fluke-cda-nighthawk = " \
	e2fsprogs \
"

DEPENDS_append_fluke-cda-vanquish = " \
	libnrp \
	librsnrpz \
	nrpzmodule \
"

IMAGE_INSTALL_append = " \
	qtbase qtbase-tools qtbase-plugins freetype fontconfig fontconfig-utils \
	ttf-roboto \
	libusb-compat \
	mtd-utils \
	tslib tslib-tests \
	avahi-daemon avahi-autoipd \
	php-cli php-cgi php-fpm php-phar php lighttpd-module-fastcgi \
	rt-tests \
	util-linux \
	lighttpd \
	iproute2 \
	linux-gpib-user \
	kernel-module-fluke-gpib \
	kernel-module-fmh-gpib \
	kernel-module-gpio-fluke \
	libgpiod \
	libgpiod-tools \
"
IMAGE_INSTALL_append_fluke-cda-caldera = " \
	kernel-module-fluke-keypad \
	kernel-module-altera-inttimer \
"

IMAGE_INSTALL_append_fluke-cda-nighthawk = " \
	e2fsprogs \
	e2fsprogs-resize2fs \
	kernel-module-altera-inttimer \
"

IMAGE_INSTALL_append_fluke-cda-triclamp = " \
	kernel-module-fluke-keypad \
	kernel-module-altera-inttimer \
"

IMAGE_INSTALL_append_fluke-cda-vanquish = " \
	libnrp \
	librsnrpz \
	packagegroup-fonts-truetype \
	kernel-module-nrpzmodule \
	kernel-module-fluke-fn-fast-uart \
	kernel-module-altera-inttimer \
"

#IMAGE_INSTALL += "fcgi \
#"

#EXTRA_IMAGE_FEATURES = "dbg-pkgs debug-tweaks "

#IMAGE_INSTALL := "${@oe_filter_out('gcc', '${IMAGE_INSTALL}', d)}"

export IMAGE_BASENAME = "fluke-console-image"

fluke_console_image_postprocess_common() {
	# add an image build date file
	date -u > ${IMAGE_ROOTFS}${sysconfdir}/rootfs_build_timestamp

	#disable auto startup of some systemd services
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/sockets.target.wants/systemd-networkd.socket
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/network-online.target.wants/systemd-networkd-wait-online.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/multi-user.target.wants/systemd-networkd.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/multi-user.target.wants/busybox-syslog.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/multi-user.target.wants/rngd.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/syslog.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/multi-user.target.wants/busybox-klogd.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/sysinit.target.wants/systemd-timesyncd.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/getty.target.wants/getty@tty1.service
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/dbus-org.freedesktop.network1.service
	# mask some systemd services
	ln -sr ${IMAGE_ROOTFS}${base_prefix}/dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/getty@tty1.service
	ln -sr ${IMAGE_ROOTFS}${base_prefix}/dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/systemd-timesyncd.service
	ln -sr ${IMAGE_ROOTFS}${base_prefix}/dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/systemd-timedated.service
	ln -sr ${IMAGE_ROOTFS}${base_prefix}/dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/systemd-networkd.service
	ln -sr ${IMAGE_ROOTFS}${base_prefix}/dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/busybox-syslog.service
	# We don't have a hardware random number generator, so rngd is useless.  Mask it to prevent it from hogging
	# the cpu at startup
	ln -sr ${IMAGE_ROOTFS}${base_prefix}/dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/rngd.service

	# turn off logging to disk
	echo Storage=volatile >> ${IMAGE_ROOTFS}${sysconfdir}/systemd/journald.conf
	echo ForwardToSyslog=no >> ${IMAGE_ROOTFS}${sysconfdir}/systemd/journald.conf

	#Prevent front panel display from being allocated as a virtual terminal by logind
	echo NAutoVTs=0 >> ${IMAGE_ROOTFS}${sysconfdir}/systemd/logind.conf
	echo ReserveVT=0 >> ${IMAGE_ROOTFS}${sysconfdir}/systemd/logind.conf
}

IMAGE_PREPROCESS_COMMAND_append = " fluke_console_image_postprocess_common; "
