# Fix broken detection of valgrind in php7.2.7 configure.
# Also customize php-fpm.conf (there is a php-fpm.conf file
# in ${THISDIR}/files which will take precedence over the
# one from the original php package).

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://0002-disable-valgrind-in-php-configure-ac.patch \
"
FILES_${PN}-fpm += "${sysconfdir}/systemd/system/multi-user.target.wants/php-fpm.service"

do_install_append_fluke-cda-nighthawk() {
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${systemd_system_unitdir}/php-fpm.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}


# add lighttpd user/group
MAYBE_USERADD_CLASS ??= ""
MAYBE_USERADD_CLASS_fluke-cda-nighthawk = "useradd"
inherit ${MAYBE_USERADD_CLASS}

USERADD_PACKAGES_fluke-cda-nighthawk = "${PN}-fpm"
GROUPADD_PARAM_${PN}-fpm_fluke-cda-nighthawk = "-g 1000 lighttpd"
USERADD_PARAM_${PN}-fpm_fluke-cda-nighthawk = "-u 1000 -g lighttpd -d ${base_prefix}/www/pages -s ${base_sbindir}/nologin lighttpd"
