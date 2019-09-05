# This is a small image used for the initrd for the rescue kernel.
# Ideally, it should just contain what is needed to perform
# necessary rescue operations, and to boot into the production kernel.

#require recipes-images/angstrom/console-base-image.bb
#require recipes-core/images/core-image-minimal.bb
inherit image

DEPENDS += " \
	kexec-tools \
	rescue-fluke-cda \
	mtd-utils \
"

IMAGE_INSTALL_append = " \
	kexec \
	rescue-fluke-cda \
	mtd-utils \
"

# remove some stuff we don't need, we want to get the compressed image size below 16M
DEPENDS_remove = " \
	systemd \
	pam \
"
IMAGE_INSTALL_remove = " \
	busybox-udhcpc \
	busybox-syslogd \
	systemd \
	systemd-compat-units \
	systemd-vconsole-setup \
	systemd-extra-utils \
	systemd-container \
	systemd-serialgetty \
	dbus-1 \
	pam-plugin-keyinit \
	pam-plugin-loginuid \
	shadow \
	pam-plugin-shells \
	pam-plugin-faildelay \
	pam-plugin-limits \
	pam-plugin-lastlog \
	pam-plugin-env \
	pam-plugin-nologin \
	libpam-runtime \
	pam-plugin-rootok \
	pam-plugin-motd \
	shadow-base \
	pam-plugin-securetty \
	pam-plugin-group \
	pam-plugin-mail \
	pam-plugin-unix \
	pam-plugin-deny \
	pam-plugin-permit \
	pam-plugin-warn \
	libpam \
	iptables \
	libcurl4 \
"

#EXTRA_IMAGE_FEATURES = "dbg-pkgs debug-tweaks "

#IMAGE_INSTALL := "${@oe_filter_out('gcc', '${IMAGE_INSTALL}', d)}"

#we only need a compressed ext4 image for the initrd
IMAGE_FSTYPES_remove = "ubi tar.xz wic.gz"
IMAGE_FSTYPES_append = " ext4.gz"
IMAGE_ROOTFS_SIZE = "65536"
IMAGE_OVERHEAD_FACTOR = "1.0" 

export IMAGE_BASENAME = "fluke-initrd-image"

fluke_initrd_image_postprocess_common() {
}

ROOTFS_POSTPROCESS_COMMAND_append = " fluke_initrd_image_postprocess_common; "
