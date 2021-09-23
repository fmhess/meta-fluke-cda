
require recipes-images/angstrom/fluke-console-image.bb

DEPENDS_append_fluke-cda-nighthawk = " \
	noto-sans-hinted \
	noto-sans-cjk-regular \
	noto-sans-cjk-bold \
	customization-${MACHINE} \
	fluke-run-once \
"
DEPENDS_append_fluke-cda-caldera = " \
	customization-${MACHINE} \
	fluke-run-once \
"

IMAGE_INSTALL_append = " \
	fbgrab \
	sqlite3 \
	libsqlite3-dev \
	x11vnc \
	python3 \
"

IMAGE_INSTALL_append_fluke-cda-nighthawk = " \
	noto-sans-hinted-regular \
	noto-sans-hinted-bold \
	noto-sans-cjk-regular \
	noto-sans-cjk-bold \
	customization-${MACHINE} \
	kernel-devicetree \
	fluke-run-once \
"
IMAGE_INSTALL_append_fluke-cda-caldera = " \
	customization-${MACHINE} \
	kernel-devicetree \
	fluke-run-once \
"

export IMAGE_BASENAME = "fluke-full-image"

IMAGE_FSTYPES = " ubi"
IMAGE_FSTYPES_append_fluke-cda-nighthawk = " wic.gz"
IMAGE_FSTYPES_remove_fluke-cda-nighthawk = "ubi"
IMAGE_FSTYPES_remove = "tar.xz"
UBI_VOLNAME = "root-fs"

fluke_full_image_postprocess_common() {
}
fluke_full_image_postprocess_nighthawk() {
	#modify gpib board type to use fmh_gpib driver
	sed -i 's/^[[:blank:]]*board_type.*/	board_type = "fmh_gpib"/' ${IMAGE_ROOTFS}${sysconfdir}/gpib.conf
	sed -i '/board_type.*=.*"fluke_hybrid"/d' ${IMAGE_ROOTFS}${sysconfdir}/gpib.conf
	sed -i '/board_type.*=.*"fluke_unaccel"/d' ${IMAGE_ROOTFS}${sysconfdir}/gpib.conf
}

IMAGE_PREPROCESS_COMMAND_append = " fluke_full_image_postprocess_common; "
IMAGE_PREPROCESS_COMMAND_append_fluke-cda-nighthawk = " fluke_full_image_postprocess_nighthawk; "

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
