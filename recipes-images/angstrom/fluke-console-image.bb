
require recipes-images/angstrom/small-image.bb

DEPENDS += "lighttpd \
	iproute2 \
	linux-gpib-user \
"
DEPENDS_fluke-cda-vanquish += "\
	libnrp \
	librsnrpz \
"
IMAGE_INSTALL += "lighttpd \
	iproute2 \
	linux-gpib-user \
"
IMAGE_INSTALL_fluke-cda-vanquish += "\
	libnrp \
	librsnrpz \
"

#IMAGE_INSTALL += "fcgi \
#"

#EXTRA_IMAGE_FEATURES = "dbg-pkgs debug-tweaks "

#IMAGE_INSTALL := "${@oe_filter_out('gcc', '${IMAGE_INSTALL}', d)}"

export IMAGE_BASENAME = "fluke-console-image"
