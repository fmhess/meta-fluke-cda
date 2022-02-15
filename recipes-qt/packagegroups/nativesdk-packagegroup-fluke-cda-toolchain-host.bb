
DESCRIPTION = "Extra host packages for Fluke CDA version of meta-toochain-qt5"
LICENSE = "MIT"

inherit packagegroup nativesdk

PACKAGEGROUP_DISABLE_COMPLEMENTARY = "1"

RDEPENDS_${PN} = " \
	python3-xlrd \
"
