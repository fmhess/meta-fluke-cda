# This is a small image used for the initrd for the rescue kernel.
# Ideally, it should just contain what is needed to perform
# necessary rescue operations, and to boot into the production kernel.

#require recipes-images/angstrom/console-base-image.bb
#require recipes-core/images/core-image-minimal.bb
inherit image

DEPENDS += " \
	kexec-tools \
"

IMAGE_INSTALL_append = " \
	kexec \
"

IMAGE_INSTALL_remove = " \
"

#EXTRA_IMAGE_FEATURES = "dbg-pkgs debug-tweaks "

#IMAGE_INSTALL := "${@oe_filter_out('gcc', '${IMAGE_INSTALL}', d)}"

#we only need a compressed ext4 image for the initrd
IMAGE_FSTYPES_remove = "ubi tar.xz wic.gz"
IMAGE_FSTYPES_append = " ext4.gz"
#We used to turn off huge_file feature of ext4, since kernel will not be able to 
#mount it rw without kernel config CONFIG_LBDAF.  But we use CONFIG_LBDAF=y now
#so it is no longer needed.
#EXTRA_IMAGECMD_ext4 += "-O ^huge_file"
IMAGE_ROOTFS_SIZE = "65536"
IMAGE_OVERHEAD_FACTOR = "1.0" 

export IMAGE_BASENAME = "fluke-initrd-image"

fluke_initrd_image_postprocess_common() {
	#create device files
	mknod -m 600 ${IMAGE_ROOTFS}/dev/console c 5 1
	mknod -m 666 ${IMAGE_ROOTFS}/dev/tty c 5 0
	mknod -m 620 ${IMAGE_ROOTFS}/dev/tty0 c 4 0
	mknod -m 620 ${IMAGE_ROOTFS}/dev/tty1 c 4 1
	mknod -m 660 ${IMAGE_ROOTFS}/dev/ram0 b 1 0
}

ROOTFS_POSTPROCESS_COMMAND_append = " fluke_initrd_image_postprocess_common; "
