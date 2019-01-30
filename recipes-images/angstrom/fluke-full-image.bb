
require recipes-images/angstrom/fluke-console-image.bb

DEPENDS_append_fluke-cda-nighthawk = " \
	customization-fluke-cda-nighthawk \
"
IMAGE_INSTALL_append_fluke-cda-nighthawk = " \
	customization-fluke-cda-nighthawk \
	kernel-devicetree \
"

export IMAGE_BASENAME = "fluke-full-image"

postprocess_nighthawk() {
	ln -srf ${IMAGE_ROOTFS}/dev/null ${IMAGE_ROOTFS}/${sysconfdir}/systemd/network/10-en.network
	ln -srf ${IMAGE_ROOTFS}/dev/null ${IMAGE_ROOTFS}/${sysconfdir}/systemd/network/11-eth.network

	# add Fluke CDA developer accounts and set root password
	sed -i '/root/d' ${IMAGE_ROOTFS}${sysconfdir}/passwd
	cat ${IMAGE_ROOTFS}${base_prefix}/tmp/customization_data/passwd_append >> ${IMAGE_ROOTFS}${sysconfdir}/passwd
	rm ${IMAGE_ROOTFS}${base_prefix}/tmp/customization_data/passwd_append
}

ROOTFS_POSTPROCESS_COMMAND_fluke-cda-nighthawk += "postprocess_nighthawk; "
