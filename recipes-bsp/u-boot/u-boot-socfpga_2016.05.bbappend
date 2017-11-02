PR = "r1"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_fluke-cda-vanquish = "\
	file://0001-Add-Fluke-CDA-Vanquish-board-to-uboot.patch \
"

