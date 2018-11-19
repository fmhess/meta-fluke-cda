#add missing patch to sumo to fix mesa build

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://0008-use-PKG_CHECK_VAR-for-defining-WAYLAND_PROTOCOLS_DAT.patch \
"

EXTRA_OEMAKE += "WAYLAND_PROTOCOLS_DATADIR=${STAGING_DATADIR}/wayland-protocols"
