SUMMARY = "Linux-GPIB user space"
DESCRIPTION = "https://linux-gpib.sourceforge.io/"
HOMEPAGE = "https://linux-gpib.sourceforge.io/"
BUGTRACKER = "https://sourceforge.net/p/linux-gpib/bugs/"
SECTION = "libs"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

DEPENDS += "bison-native flex-native"

#SRCREV = "${AUTOREV}"
SRCREV = "1869"
PV = "svnr${SRCREV}"
PR = "r0"

SRC_URI = "svn://svn.code.sf.net/p/linux-gpib/code/;module=trunk/linux-gpib-user;protocol=https \
	file://extra_files/etc/gpib.conf"
	
S = "${WORKDIR}/trunk/linux-gpib-user"

inherit autotools

EXTRA_OECONF += " --disable-documentation\
	--disable-guile-binding\
	--disable-perl-binding\
	--disable-php-binding\
	--disable-python-binding\
	--disable-tcl-binding\
"

FILES_${PN} += " \
	${datadir}/usb/* \
"

do_install_append() {
	install -m 0644 ${WORKDIR}/extra_files/etc/gpib.conf ${D}${sysconfdir}/gpib.conf
	
	#Remove udev rules for usb gpib controllers (we don't want them auto configured if someone
	#were to plug one into the instrument).
	rm ${D}${sysconfdir}/udev/rules.d/99-agilent_82357a.rules
	rm ${D}${sysconfdir}/udev/rules.d/99-ni_usb_gpib.rules
}
