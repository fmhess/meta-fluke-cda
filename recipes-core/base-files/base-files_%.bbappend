
#FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

do_install_append_fluke-cda-nighthawk() {
	cat >> ${D}${sysconfdir}/fstab <<EOF
gadget               /dev/gadget          gadgetfs   noauto,user,group     0  0
EOF
}
