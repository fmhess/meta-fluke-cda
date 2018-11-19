
#require recipes-images/angstrom/small-image.bb
require recipes-images/angstrom/console-base-image.bb

DEPENDS += "lighttpd \
	iproute2 \
	linux-gpib-user \
    ttf-roboto \
"
DEPENDS_append_fluke-cda-vanquish = " \
	libnrp \
	librsnrpz \
"
IMAGE_INSTALL_append = " \
	qtbase qtbase-tools qtbase-plugins freetype fontconfig fontconfig-utils \
	packagegroup-fonts-truetype  \
	ttf-droid-sans \
	ttf-droid-sans-mono \
	ttf-droid-sans-fallback \
	ttf-droid-sans-japanese \
	ttf-droid-serif \
	ttf-roboto \
	libusb-compat \
	mtd-utils \
	tslib tslib-tests \
	avahi-daemon avahi-autoipd \
	php-cli php-cgi php-fpm php-phar php lighttpd-module-fastcgi \
	rt-tests \
	util-linux \
	lighttpd \
	iproute2 \
	linux-gpib-user \
"

IMAGE_INSTALL_append_fluke-cda-vanquish = " \
	libnrp \
	librsnrpz \
"

#IMAGE_INSTALL += "fcgi \
#"

#EXTRA_IMAGE_FEATURES = "dbg-pkgs debug-tweaks "

#IMAGE_INSTALL := "${@oe_filter_out('gcc', '${IMAGE_INSTALL}', d)}"

export IMAGE_BASENAME = "fluke-console-image"
