# Fix broken detection of valgrind in php7.2.7 configure.
# Also customize php-fpm.conf (there is a php-fpm.conf file
# in ${THISDIR}/files which will take precedence over the
# one from the original php package).

# we put a php-fpm.conf file in our FILESEXTRAPATHS which should override the one
# from the original package, since we are prepending
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://0002-disable-valgrind-in-php-configure-ac.patch \
"
FILES_${PN}-fpm += "${sysconfdir}/systemd/system/multi-user.target.wants/php-fpm.service"

fluke_common_do_install() {
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${systemd_system_unitdir}/php-fpm.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}
do_install_append_fluke-cda-nighthawk() {
	fluke_common_do_install
}
do_install_append_fluke-cda-caldera() {
	fluke_common_do_install
}

# add lighttpd user/group
MAYBE_USERADD_CLASS ??= ""
MAYBE_USERADD_CLASS_fluke-cda-nighthawk = "useradd"
MAYBE_USERADD_CLASS_fluke-cda-caldera = "useradd"
inherit ${MAYBE_USERADD_CLASS}

USERADD_PACKAGES = "${PN}-fpm"
GROUPADD_PARAM_${PN}-fpm = "-g 1000 lighttpd"
USERADD_PARAM_${PN}-fpm = "-u 1000 -g lighttpd -d ${base_prefix}/www/pages -s ${base_sbindir}/nologin lighttpd"
