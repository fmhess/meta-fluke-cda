
DESCRIPTION = "Extra target packages for Fluke CDA version of meta-toochain-qt5"
LICENSE = "MIT"

inherit packagegroup

PACKAGEGROUP_DISABLE_COMPLEMENTARY = "1"

RDEPENDS_${PN} = " \
    linux-gpib-user \
    tslib \
    libgpiod-dev \
    fontconfig \
    googletest-staticdev \
    googletest-dbg \
    googletest-dev \
    googletest \
    libeigen-dev \
"

RDEPENDS_${PN}_append_fluke-cda-vanquish = " \
    libnrp \
    librsnrpz \
"
