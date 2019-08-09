DESCRIPTION = "Fluke rescue program"
DEPENDS = ""
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://COPYING;md5=1ebbd3e34237af26da5dc08a4e440464"
RDEPENDS_${PN} = "busybox"

SRC_URI = "file://src/"

S = "${WORKDIR}/src/"
PR = "r0"

EXTRA_OEMAKE = "'CC=${CC}' 'CFLAGS=${CFLAGS}'"

do_install () {
	oe_runmake install DESTDIR=${D} prefix=${prefix} exec_prefix=${exec_prefix}
}
