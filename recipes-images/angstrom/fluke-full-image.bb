
require recipes-images/angstrom/fluke-console-image.bb

DEPENDS_append_fluke-cda-nighthawk = " \
	customization-${MACHINE} \
"
DEPENDS_append_fluke-cda-caldera = " \
	customization-${MACHINE} \
"

IMAGE_INSTALL_append_fluke-cda-nighthawk = " \
	customization-${MACHINE} \
	kernel-devicetree \
"
IMAGE_INSTALL_append_fluke-cda-caldera = " \
	customization-${MACHINE} \
	kernel-devicetree \
"

export IMAGE_BASENAME = "fluke-full-image"

fluke_full_image_postprocess_common() {
}
fluke_full_image_postprocess_nighthawk() {
	#modify gpib board type to use fmh_gpib driver
	sed -i 's/^[[:blank:]]*board_type.*/	board_type = "fmh_gpib"/' ${IMAGE_ROOTFS}${sysconfdir}/gpib.conf
	sed -i '/board_type.*=.*"fluke_hybrid"/d' ${IMAGE_ROOTFS}${sysconfdir}/gpib.conf
	sed -i '/board_type.*=.*"fluke_unaccel"/d' ${IMAGE_ROOTFS}${sysconfdir}/gpib.conf
}

ROOTFS_POSTPROCESS_COMMAND_append = " fluke_full_image_postprocess_common; "
ROOTFS_POSTPROCESS_COMMAND_append_fluke-cda-nighthawk = " fluke_full_image_postprocess_nighthawk; "

# we do the following using SORT_PASSWD_POSTPROCESS_COMMAND instead of
# ROOTFS_POSTPROCESS_COMMAND to avoid having our multiple root accounts 
# getting sorted by username in the passwd file (we want the one named 
# "root" to be first).
fluke_full_image_postsort_passwd_common() {
	# add Fluke CDA developer accounts and set root password
	sed -i '/root/d' ${IMAGE_ROOTFS}${sysconfdir}/passwd
	cat ${IMAGE_ROOTFS}${base_prefix}/tmp/customization_data/passwd_append >> ${IMAGE_ROOTFS}${sysconfdir}/passwd
	rm ${IMAGE_ROOTFS}${base_prefix}/tmp/customization_data/passwd_append
}
SORT_PASSWD_POSTPROCESS_COMMAND_append_fluke-cda-nighthawk = " fluke_full_image_postsort_passwd_common; "
SORT_PASSWD_POSTPROCESS_COMMAND_append_fluke-cda-caldera = " fluke_full_image_postsort_passwd_common; "
