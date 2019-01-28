# Fix broken detection of valgrind in php7.2.7 configure.
# Also customize php-fpm.conf (there is a php-fpm.conf file
# in ${THISDIR}/files which will take precedence over the
# one from the original php package).

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://0002-disable-valgrind-in-php-configure-ac.patch \
"
FILES_${PN}-fpm += "${sysconfdir}/systemd/system/multi-user.target.wants/php-fpm.service"

do_install_append() {
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${systemd_system_unitdir}/php-fpm.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}
