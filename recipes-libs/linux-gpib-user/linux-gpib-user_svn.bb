SUMMARY = "Linux-GPIB user space"
DESCRIPTION = "https://linux-gpib.sourceforge.io/"
HOMEPAGE = "https://linux-gpib.sourceforge.io/"
BUGTRACKER = "https://sourceforge.net/p/linux-gpib/bugs/"
SECTION = "libs"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3"

#SRCREV = "${AUTOREV}"
SRCREV = "1743"
PV = "svnr${SRCREV}"
PR = "r0"

SRC_URI = "svn://svn.code.sf.net/p/linux-gpib/code/;module=trunk/linux-gpib-user;protocol=https \
	"
S = "${WORKDIR}/trunk/linux-gpib-user"

inherit autotools

#DEPENDS =+ ""

#FILES_${PN} += "${libdir}/libgdiplus.so"
#INSANE_SKIP_${PN} += "dev-so"

CONFIGUREOPTS += " --disable-documentation\
	--disable-guile-binding\
	--disable-perl-binding\
	--disable-php-binding\
	--disable-python-binding\
	--disable-tcl-binding\
	"

do_configure() {
	cd ${S}
	./bootstrap
	cd ${B}
	autotools_do_configure
}
