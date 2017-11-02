
require recipes-images/angstrom/small-image.bb

DEPENDS += "lighttpd \
"
#DEPENDS += "fcgi \
#"
DEPENDS += "iproute2 \
"

IMAGE_INSTALL += "lighttpd \
"
#IMAGE_INSTALL += "fcgi \
#"
IMAGE_INSTALL += "iproute2 \
"
#EXTRA_IMAGE_FEATURES = "dbg-pkgs debug-tweaks "

#IMAGE_INSTALL := "${@oe_filter_out('gcc', '${IMAGE_INSTALL}', d)}"

export IMAGE_BASENAME = "fluke-console-image"
