
require recipes-images/angstrom/fluke-console-image.bb

DEPENDS_append_fluke-cda-nighthawk = " \
	customization-fluke-cda-nighthawk \
"
IMAGE_INSTALL_append_fluke-cda-nighthawk = " \
	customization-fluke-cda-nighthawk \
	kernel-devicetree \
"

export IMAGE_BASENAME = "fluke-full-image"
