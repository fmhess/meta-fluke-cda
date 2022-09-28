DESCRIPTION = "/boot files used by Fluke rescue program"
DEPENDS = ""
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://COPYING;md5=1ebbd3e34237af26da5dc08a4e440464"
RDEPENDS_${PN} = "\
"

SRC_URI = "file://src/"

S = "${WORKDIR}/src"
PR = "r0"

FILES_${PN} += "/boot/*.sh"
# CONFFILES_${PN} += ""

# EXTRA_OEMAKE = "'CC=${CC}' 'CFLAGS=${CFLAGS}'"

do_install () {
	install -d ${D}${root_prefix}/boot
	install -m 644 ${WORKDIR}/src/boot/kexec_load.sh ${D}${root_prefix}/boot/
	install -m 644 ${WORKDIR}/src/boot/kexec_load_alternate.sh ${D}${root_prefix}/boot/
}
