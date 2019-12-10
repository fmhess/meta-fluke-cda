#This is a tweak of fluke-full-image for Kraken.  It just
#replaces customization-fluke-cda-nighthawk with customization-fluke-cda-kraken.

require recipes-images/angstrom/fluke-full-image.bb

DEPENDS_remove = " \
	customization-${MACHINE} \
"
DEPENDS_append = " \
	customization-fluke-cda-kraken \
"

IMAGE_INSTALL_remove = " \
	customization-${MACHINE} \
"
IMAGE_INSTALL_append = " \
	customization-fluke-cda-kraken \
"

export IMAGE_BASENAME = "fluke-cda-kraken-image"
