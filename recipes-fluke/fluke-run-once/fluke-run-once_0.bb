SUMMARY = "Fluke CDA run-once commands"
DESCRIPTION = "Miscellaneous run-once commands performed during first boot of Fluke CDA system."
DEPENDS = ""
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://COPYING;md5=1ebbd3e34237af26da5dc08a4e440464"

RDEPENDS_${PN} = "gptfdisk"

SRC_URI = "file://src/"

S = "${WORKDIR}/src"
PR = "r0"

#pkg_posinst_ontarget should get run once on the first boot if this
#package is part of an image
pkg_postinst_ontarget_${PN}() {
	# relocate backup GPT to proper location at end of the physical disk
	sgdisk --move-second-header /dev/mmcblk0
}

#we need to make the package install something or yocto will skip it entirely
do_install () {
	install -d ${D}${sysconfdir}
}
