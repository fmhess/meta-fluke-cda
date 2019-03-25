PR = "r253"
KERNEL_REPO = "git://github.com/fmhess/linux-socfpga.git"
LINUX_VERSION_SUFFIX = "-ltsi-${MACHINE}"
SRCREV = "${AUTOREV}"

KERNEL_DEVICETREE_fluke-cda-vanquish = "Vanquish_soc.dtb Vanquish_soc_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-richmond = "Richmond_005.dtb Richmond_005_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-triclamp = "Triclamp_soc.dtb Triclamp_soc_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-nighthawk = "Nighthawk_soc.dtb Nighthawk_soc_overlay.dtb" 
KERNEL_DEVICETREE_fluke-cda-caldera = "Caldera_soc.dtb Caldera_soc_overlay.dtb"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
    file://fluke-cda.cfg \
"

SRC_URI_append_fluke-cda-caldera = "\
    file://fluke-cda-caldera.cfg \
"

SRC_URI_append_fluke-cda-triclamp = "\
    file://fluke-cda-triclamp.cfg \
"
