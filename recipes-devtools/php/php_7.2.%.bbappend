#fix broken detection of valgrind in php7.2.7 configure

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://0002-disable-valgrind-in-php-configure-ac.patch \
"
