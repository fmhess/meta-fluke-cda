#adapted this file from sumo release of meta-altera, since the zeus release only has a recipe
#for u-boot-socfpga version v2019.12
require u-boot-socfpga-common.inc
require u-boot-socfpga-env.inc
require ${COREBASE}/meta/recipes-bsp/u-boot/u-boot.inc

PR = "r1"

SRC_URI_append = "\
	"

DEPENDS += "dtc-native bc-native"
DEPENDS += "u-boot-mkimage-native"
